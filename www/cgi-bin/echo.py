#!/usr/bin/env python3
import os, sys
length = int(os.environ.get("CONTENT_LENGTH") or 0)
body = sys.stdin.buffer.read(length) if length else sys.stdin.buffer.read()
sys.stdout.write("Content-Type: application/octet-stream\r\n")
sys.stdout.write("Content-Length: %d\r\n\r\n" % len(body))
sys.stdout.flush()
sys.stdout.buffer.write(body)