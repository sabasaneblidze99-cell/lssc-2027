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
def ascii_line(line):
    """Il file unico e' SOLO ASCII: i caratteri non ASCII nelle stringhe diventano \\xHH; (stessi byte UTF-8 nel .amx),
    cosi' nessun editor (Qawno salva in ANSI) puo' trasformare il georgiano in '?'. Nei commenti diventano '?'."""
    out = []; i = 0; q = None; n = len(line)
    while i < n:
        ch = line[i]
        if q:
            if ch == '\\' and i + 1 < n:
                out.append(line[i:i + 2]); i += 2; continue
            if ch == q:
                q = None
            if ord(ch) > 127:
                out.append(''.join('\\x%02X;' % b for b in ch.encode('utf-8')))
            else:
                out.append(ch)
            i += 1; continue
        if ch in '"\'':
            q = ch; out.append(ch); i += 1; continue
        if line.startswith('//', i):
            out.append(''.join(c if ord(c) < 128 else '?' for c in line[i:])); break
        out.append(ch if ord(ch) < 128 else '?')
        i += 1
    return ''.join(out)
text = "".join(ascii_line(l) for l in expand(sys.argv[1]))
open(sys.argv[2], "w", encoding="ascii").write(text)
