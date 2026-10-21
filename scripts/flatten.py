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
MAXLINE = 2000   # pawncc: "error 075: input line too long" oltre ~4000 caratteri; gli escape \xHH; allungano molto le righe georgiane
def split_long(line):
    """Spezza le righe troppo lunghe dopo le virgole fuori dalle stringhe (inizializzatori di array, argomenti)."""
    body = line.rstrip('\n')
    if len(body) <= MAXLINE or body.lstrip().startswith('#'):
        return line
    parts = []; cur = []; q = None; i = 0; n = len(body)
    while i < n:
        ch = body[i]
        if q:
            if ch == '\\' and i + 1 < n:
                cur.append(body[i:i + 2]); i += 2; continue
            if ch == q: q = None
        elif ch in '"\'':
            q = ch
        elif body.startswith('//', i):
            cur.append(body[i:]); break
        cur.append(ch); i += 1
        if not q and ch == ',' and sum(map(len, cur)) > MAXLINE // 2:
            parts.append(''.join(cur)); cur = []
            while i < n and body[i] == ' ': i += 1
    parts.append(''.join(cur))
    for p_ in parts:
        if len(p_) > 4000:
            sys.exit('flatten: riga troppo lunga anche dopo la divisione (%d caratteri): %s...' % (len(p_), p_[:80]))
    return '\n\t'.join(parts) + '\n'
text = "".join(split_long(ascii_line(l)) for l in expand(sys.argv[1]))
open(sys.argv[2], "w", encoding="ascii").write(text)
