"""Local-only JSON receiver and HTML dashboard; Python standard library only."""

from __future__ import annotations

import argparse
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
from pathlib import Path
import socket
import webbrowser

from protocol import MAX_BYTES, ProtocolError, SnapshotStore

STATIC = Path(__file__).resolve().parent / "web"
ASSETS = {"/": ("index.html", "text/html; charset=utf-8"),
          "/index.html": ("index.html", "text/html; charset=utf-8"),
          "/dashboard.js": ("dashboard.js", "text/javascript; charset=utf-8"),
          "/dashboard.css": ("dashboard.css", "text/css; charset=utf-8")}


def unique_keys(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ProtocolError("duplicate JSON key")
        result[key] = value
    return result


def invalid_constant(value):
    raise ProtocolError(f"invalid JSON number: {value}")


class MonitorServer(ThreadingHTTPServer):
    daemon_threads = True

    def __init__(self):
        self.store = SnapshotStore()
        super().__init__(("127.0.0.1", 8766), MonitorHandler)


class MonitorHandler(BaseHTTPRequestHandler):
    server_version = "SimpleGameMonitor/1"

    def setup(self):
        super().setup()
        self.connection.settimeout(3)

    def reply(self, status, body, content_type="application/json; charset=utf-8"):
        if not isinstance(body, bytes):
            body = json.dumps(body, ensure_ascii=False, allow_nan=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.send_header("X-Content-Type-Options", "nosniff")
        self.send_header("Content-Security-Policy", "default-src 'self'; script-src 'self'; style-src 'self'; connect-src 'self'; frame-ancestors 'none'")
        self.end_headers()
        try:
            self.wfile.write(body)
        except (BrokenPipeError, ConnectionResetError):
            pass

    def local_request(self):
        if self.headers.get("Host") not in {"127.0.0.1:8766", "localhost:8766"}:
            self.reply(403, {"error": "localhost Host required"})
            return False
        origin = self.headers.get("Origin")
        if origin is not None and origin not in {"http://127.0.0.1:8766", "http://localhost:8766"}:
            self.reply(403, {"error": "cross-origin requests are not accepted"})
            return False
        return True

    def do_GET(self):
        if not self.local_request():
            return
        route = self.path.split("?", 1)[0]
        if route == "/api/state":
            self.reply(200, self.server.store.read())
        elif route in ASSETS:
            filename, content_type = ASSETS[route]
            try:
                self.reply(200, (STATIC / filename).read_bytes(), content_type)
            except OSError:
                self.reply(500, {"error": "dashboard asset missing"})
        elif route == "/favicon.ico":
            self.reply(204, b"")
        else:
            self.reply(404, {"error": "not found"})

    def do_POST(self):
        if not self.local_request():
            return
        if self.path != "/api/snapshot":
            self.reply(404, {"error": "not found"})
            return
        if self.headers.get_content_type() != "application/json":
            self.reply(415, {"error": "Content-Type: application/json required"})
            return
        if self.headers.get("Transfer-Encoding"):
            self.reply(400, {"error": "send Content-Length, not chunked encoding"})
            return
        try:
            length = int(self.headers.get("Content-Length", "0"))
        except ValueError:
            length = 0
        if not 0 < length <= MAX_BYTES:
            self.reply(413, {"error": "body must be between 1 byte and 2 MiB"})
            return
        try:
            raw = self.rfile.read(length)
            if len(raw) != length:
                raise ProtocolError("incomplete request body")
            message = json.loads(raw.decode("utf-8"), object_pairs_hook=unique_keys,
                                 parse_constant=invalid_constant)
            self.server.store.accept(message)
        except (ValueError, UnicodeError, RecursionError) as error:
            self.reply(400, {"error": str(error)[:240]})
            return
        except (socket.timeout, ConnectionResetError):
            self.close_connection = True
            return
        self.reply(200, {"accepted": True, "sequence": message["sequence"]})

    def log_message(self, format, *args):
        # Heartbeats are intentionally quiet; malformed requests retain HTTP responses.
        pass


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--open", action="store_true", help="open the dashboard in the default browser")
    args = parser.parse_args()
    try:
        server = MonitorServer()
    except OSError as error:
        parser.exit(1, f"127.0.0.1:8766 수신 서버를 열 수 없습니다: {error}\n")
    print("월드 모니터: http://127.0.0.1:8766/", flush=True)
    print("JSON 수신: POST /api/snapshot | 종료: Ctrl+C", flush=True)
    if args.open:
        webbrowser.open("http://127.0.0.1:8766/")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
