#!/usr/bin/env python3
"""Unisce lsrp.pwn + modules/*.inc in un solo file .pwn (per Qawno). Gli include <...> restano."""
import os, re, sys
root = os.path.dirname(os.path.abspath(sys.argv[1]))
seen = set()
def expand(path):
    out = []
    for line in open(path, encoding="utf-8"):
        m = re.match(r'\s*#include\s+"([^"]+)"', line)
        if m:
            p = os.path.normpath(os.path.join(root, m.group(1)))
            if p in seen: continue
            seen.add(p)
            out.append("// ===== %s =====\n" % m.group(1)); out += expand(p); out.append("\n")
        else:
            out.append(line)
    return out
open(sys.argv[2], "w", encoding="utf-8").write("".join(expand(sys.argv[1])))
