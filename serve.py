import http.server
import os
import socketserver
import threading
import webbrowser


class Handler(http.server.SimpleHTTPRequestHandler):
    ALLOWED = {
        "/WebDogfight.html",
        "/WebDogfight.js",
        "/WebDogfight.wasm",
        "/WebDogfight.data",
    }

    def do_GET(self):
        if self.path not in self.ALLOWED:
            self.send_response(404)
            self.end_headers()
            return
        super().do_GET()

    def end_headers(self):
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        super().end_headers()

    def guess_type(self, path):
        if path.endswith(".wasm"):
            return "application/wasm"
        if path.endswith(".data"):
            return "application/octet-stream"
        return super().guess_type(path)


os.chdir(os.path.dirname(os.path.abspath(__file__)))

with socketserver.TCPServer(("", 8000), Handler) as httpd:
    url = "http://localhost:8000/WebDogfight.html"
    print(f"Serving on {url}")
    threading.Timer(0.5, lambda: webbrowser.open(url)).start()
    httpd.serve_forever()
