"""Versioned full-snapshot validation and bounded, thread-safe receiver state."""

from __future__ import annotations

from collections import OrderedDict
import copy
import math
import threading
import time

MAX_BYTES = 2 * 1024 * 1024
STALE_SECONDS = 5.0


class ProtocolError(ValueError):
    pass


class SnapshotValidator:
    @staticmethod
    def text(obj, key, maximum=512):
        value = obj.get(key)
        if not isinstance(value, str) or len(value) > maximum:
            raise ProtocolError(f"{key}: string of at most {maximum} characters required")
        return value

    @staticmethod
    def integer(obj, key, minimum=0):
        value = obj.get(key)
        if type(value) is not int or not minimum <= value <= 9_007_199_254_740_991:
            raise ProtocolError(f"{key}: integer >= {minimum} required")
        return value

    @staticmethod
    def boolean(obj, key):
        if type(obj.get(key)) is not bool:
            raise ProtocolError(f"{key}: boolean required")

    @staticmethod
    def position(value):
        if value is None:
            return
        if not isinstance(value, dict):
            raise ProtocolError("position: object or null required")
        for key in ("x", "y"):
            number = value.get(key)
            if type(number) not in (int, float) or abs(number) > 1e9 or not math.isfinite(number):
                raise ProtocolError("position: finite x/y world coordinates required")

    @classmethod
    def health(cls, entity, optional):
        health, maximum = entity.get("health"), entity.get("maxHealth")
        if optional and health is None and maximum is None:
            return
        cls.integer(entity, "health")
        cls.integer(entity, "maxHealth", 1)
        if health > maximum:
            raise ProtocolError("health cannot exceed maxHealth")

    @classmethod
    def validate(cls, source):
        if not isinstance(source, dict) or type(source.get("schemaVersion")) is not int or source["schemaVersion"] != 1:
            raise ProtocolError("schemaVersion must be 1")
        if source.get("type") != "world.snapshot":
            raise ProtocolError("type must be world.snapshot")
        if not cls.text(source, "sessionId", 128):
            raise ProtocolError("sessionId must not be empty")
        cls.integer(source, "sequence", 1)
        cls.integer(source, "sentAtUnixMs")
        game = source.get("game")
        if not isinstance(game, dict):
            raise ProtocolError("game: object required")
        if game.get("status") not in ("selecting", "running", "paused", "menu", "defeated"):
            raise ProtocolError("game.status is not supported")
        cls.text(game, "chapter", 128)
        cls.position(game.get("playerPosition"))
        regions = source.get("regions")
        if not isinstance(regions, list) or len(regions) > 64:
            raise ProtocolError("regions: array of at most 64 regions required")
        result = copy.deepcopy(source)
        totals = dict(npcs=0, npcTracked=0, monsters=0, monsterDead=0)
        region_ids, entity_ids = set(), set()
        active = 0
        for region in result["regions"]:
            if not isinstance(region, dict):
                raise ProtocolError("region: object required")
            region_id = cls.text(region, "id", 128)
            if not region_id or region_id in region_ids:
                raise ProtocolError("duplicate or empty region ID")
            region_ids.add(region_id)
            cls.text(region, "name", 256)
            cls.text(region, "status", 64)
            cls.text(region, "detail", 1024)
            if region.get("kind") not in ("settlement", "region"):
                raise ProtocolError("region.kind must be settlement or region")
            for key in ("active", "discovered"):
                cls.boolean(region, key)
            active += int(region["active"])
            counts = dict(npcs=0, npcTracked=0, monsters=0, monsterDead=0)
            for key in ("npcs", "monsters"):
                entities = region.get(key)
                if not isinstance(entities, list) or len(entities) > 10000:
                    raise ProtocolError(f"{key}: array of at most 10000 entities required")
                for entity in entities:
                    if not isinstance(entity, dict):
                        raise ProtocolError("entity: object required")
                    entity_id = cls.text(entity, "id", 256)
                    if not entity_id or entity_id in entity_ids:
                        raise ProtocolError("duplicate or empty entity ID")
                    entity_ids.add(entity_id)
                    if len(entity_ids) > 20000:
                        raise ProtocolError("maximum 20000 total entities")
                    cls.text(entity, "state", 64)
                    cls.text(entity, "role", 64)
                    cls.position(entity.get("position"))
                    cls.health(entity, key == "npcs")
                    if key == "npcs":
                        cls.text(entity, "name", 256)
                        cls.text(entity, "detail", 1024)
                        cls.boolean(entity, "present")
                        counts["npcTracked"] += 1
                        counts["npcs"] += int(entity["present"])
                    else:
                        cls.boolean(entity, "scripted")
                        counts["monsters" if entity["health"] > 0 else "monsterDead"] += 1
            region["counts"] = counts
            for key, count in counts.items():
                totals[key] += count
        if active > 1:
            raise ProtocolError("at most one active region is allowed")
        if game["status"] == "selecting" and regions:
            raise ProtocolError("selecting state must have an empty regions array")
        result["totals"] = totals
        return result


class SnapshotStore:
    def __init__(self):
        self._lock = threading.Lock()
        self._snapshot = None
        self._received = 0.0
        self._received_unix_ms = None
        self._sequences = OrderedDict()

    def accept(self, source):
        snapshot = SnapshotValidator.validate(source)
        with self._lock:
            now = time.monotonic()
            session = snapshot["sessionId"]
            if (self._snapshot is not None and self._snapshot["sessionId"] != session
                    and now - self._received < STALE_SECONDS):
                raise ProtocolError("another game session is active; retry after its heartbeat expires")
            if snapshot["sequence"] <= self._sequences.get(session, 0):
                raise ProtocolError("sequence must increase within a session")
            self._sequences[session] = snapshot["sequence"]
            self._sequences.move_to_end(session)
            while len(self._sequences) > 32:
                self._sequences.popitem(last=False)
            self._snapshot = snapshot
            self._received = now
            self._received_unix_ms = int(time.time() * 1000)

    def read(self):
        with self._lock:
            age = time.monotonic() - self._received if self._snapshot is not None else None
            return {"connected": age is not None and age < STALE_SECONDS,
                    "ageSeconds": round(age, 2) if age is not None else None,
                    "receivedAtUnixMs": self._received_unix_ms,
                    "snapshot": self._snapshot}
