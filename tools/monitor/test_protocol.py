"""Manual protocol regression checks; never launches the game or a network server."""

import copy
import unittest
from unittest.mock import patch

from protocol import ProtocolError, SnapshotStore, SnapshotValidator


def snapshot(sequence=1, session="test-process"):
    return {
        "schemaVersion": 1, "type": "world.snapshot", "sessionId": session,
        "sequence": sequence, "sentAtUnixMs": 1,
        "game": {"status": "running", "chapter": "chapter1", "playerPosition": {"x": 0, "y": 0}},
        "regions": [{"id": "test:0", "name": "검증 전용 마을", "kind": "settlement",
                     "active": True, "discovered": True, "status": "watch", "detail": "검증 데이터",
                     "npcs": [{"id": "npc:1", "name": "검증 NPC", "role": "civilian",
                               "state": "waiting", "detail": "", "present": True,
                               "position": {"x": 1, "y": 2}, "health": None, "maxHealth": None}],
                     "monsters": [{"id": "monster:18446744073709551615", "role": "melee",
                                   "state": "dead", "position": {"x": 3, "y": 4},
                                   "health": 0, "maxHealth": 45, "scripted": False}]}]}


class ProtocolTests(unittest.TestCase):
    def test_counts_exclude_dead_monsters_and_absent_npcs(self):
        message = snapshot()
        missing = copy.deepcopy(message["regions"][0]["npcs"][0])
        missing.update(id="npc:2", present=False, state="rescued", position=None)
        message["regions"][0]["npcs"].append(missing)
        result = SnapshotValidator.validate(message)
        self.assertEqual(result["totals"], {"npcs": 1, "npcTracked": 2, "monsters": 0, "monsterDead": 1})
        self.assertNotIn("totals", message)
        self.assertEqual(result["regions"][0]["monsters"][0]["id"], "monster:18446744073709551615")

    def test_rejected_snapshot_keeps_last_good_value_and_freshness(self):
        store = SnapshotStore()
        with patch("protocol.time.monotonic", return_value=10):
            store.accept(snapshot())
        invalid = snapshot(2)
        invalid["regions"][0]["monsters"][0]["health"] = 999
        with patch("protocol.time.monotonic", return_value=16):
            with self.assertRaises(ProtocolError):
                store.accept(invalid)
            state = store.read()
        self.assertFalse(state["connected"])
        self.assertEqual(state["snapshot"]["sequence"], 1)
        self.assertEqual(state["ageSeconds"], 6)

    def test_sequence_order_and_session_handover(self):
        store = SnapshotStore()
        with patch("protocol.time.monotonic", return_value=10):
            store.accept(snapshot(3))
            with self.assertRaises(ProtocolError):
                store.accept(snapshot(2))
            with self.assertRaises(ProtocolError):
                store.accept(snapshot(1, "new-process"))
        with patch("protocol.time.monotonic", return_value=16):
            store.accept(snapshot(1, "new-process"))
            self.assertEqual(store.read()["snapshot"]["sessionId"], "new-process")

    def test_full_replacement_removes_old_entities(self):
        store = SnapshotStore()
        store.accept(snapshot())
        empty = snapshot(2)
        empty["game"]["status"] = "selecting"
        empty["regions"] = []
        store.accept(empty)
        self.assertEqual(store.read()["snapshot"]["totals"]["npcs"], 0)

    def test_bad_ids_numbers_and_schema(self):
        for mutation in ("duplicate", "nan", "huge", "boolean-health", "status-type", "kind-type", "version"):
            with self.subTest(mutation=mutation):
                message = snapshot()
                region = message["regions"][0]
                if mutation == "duplicate":
                    region["monsters"][0]["id"] = "npc:1"
                elif mutation == "nan":
                    message["game"]["playerPosition"]["x"] = float("nan")
                elif mutation == "huge":
                    message["game"]["playerPosition"]["x"] = 10 ** 1000
                elif mutation == "boolean-health":
                    region["monsters"][0]["health"] = True
                elif mutation == "status-type":
                    message["game"]["status"] = []
                elif mutation == "kind-type":
                    region["kind"] = {}
                else:
                    message["schemaVersion"] = 2
                with self.assertRaises(ProtocolError):
                    SnapshotValidator.validate(message)


if __name__ == "__main__":
    unittest.main()
