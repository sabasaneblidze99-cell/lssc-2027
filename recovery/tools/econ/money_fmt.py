#!/usr/bin/env python3
"""Passaggio al lari (una volta sola, sui moduli Pawn):
 - "$%d" / "$%i" nei testi formattati -> "%s" e l'argomento corrispondente -> Money_Str(arg)
 - "$%s" -> "₾%s" (l'argomento e' gia' formattato dai formatter in tetri)
 - "$0" iniziali dei TextDraw -> "₾0"
Stampa ogni modifica. Uso: money_fmt.py [--dry] file..."""
import re, sys
SPEC = re.compile(r'%[-+ 0-9.]*[a-zA-Z%]')
def split_args(s, start):
    """s[start] == '(' : ritorna (lista di (inizio, fine) degli argomenti, indice della ')')"""
    depth = 0; i = start; args = []; cur = start + 1; q = None
    while i < len(s):
        c = s[i]
        if q:
            if c == '\\': i += 2; continue
            if c == q: q = None
        elif c in '"\'': q = c
        elif c in '([{': depth += 1
        elif c in ')]}':
            depth -= 1
            if depth == 0:
                args.append((cur, i)); return args, i
        elif c == ',' and depth == 1:
            args.append((cur, i)); cur = i + 1
        i += 1
    raise ValueError('parentesi non chiusa')
def process(src, name, dry):
    out = src; changes = 0
    # ripete finche' trova "$%d" dentro una chiamata
    while True:
        m = re.search(r'"((?:[^"\\\n]|\\.)*\$%[di](?:[^"\\\n]|\\.)*)"', out)
        if not m: break
        lit_s, lit_e = m.start(), m.end()
        # chiamata che contiene il letterale: risali alla '(' aperta
        depth = 0; k = lit_s - 1
        while k >= 0:
            c = out[k]
            if c == ')': depth += 1
            elif c == '(':
                if depth == 0: break
                depth -= 1
            k -= 1
        args, close = split_args(out, k)
        idx = [i for i, (a, b) in enumerate(args) if a <= lit_s < b][0]
        lit = out[lit_s:lit_e]
        specs = SPEC.findall(lit)
        specs = [x for x in specs if x != '%%']
        # quali specificatori sono "$%d"
        money = []
        pos = 0; si = 0
        body = lit[1:-1]
        for sm in SPEC.finditer(body):
            if sm.group(0) == '%%': continue
            if sm.group(0) in ('%d', '%i') and sm.start() > 0 and body[sm.start() - 1] == '$':
                money.append(si)
            si += 1
        fargs = args[idx + 1:]
        if len(fargs) < si:
            raise ValueError('%s: argomenti insufficienti per %s' % (name, lit[:60]))
        new = out
        for j in reversed(money):
            a, b = fargs[j]
            expr = new[a:b]
            lead = len(expr) - len(expr.lstrip())
            new = new[:a + lead] + 'Money_Str(' + expr.strip() + ')' + new[b:]
        newlit = re.sub(r'\$%[di]', '%s', lit)
        new = new[:lit_s] + newlit + new[lit_e:]
        line = out[:lit_s].count('\n') + 1
        print('%s:%d  %s' % (name, line, newlit[:90]))
        out = new; changes += 1
    n2 = len(re.findall(r'\$%s', out)); out = out.replace('$%s', '₾%s')
    n3 = len(re.findall(r'"\$0"|: \$0"', out)); out = out.replace('"$0"', '"₾0"').replace(': $0"', ': ₾0"')
    return out, changes, n2, n3
dry = '--dry' in sys.argv
tot = [0, 0, 0]
for f in [a for a in sys.argv[1:] if a != '--dry']:
    s = open(f, encoding='utf-8').read()
    o, a, b, c = process(s, f, dry)
    tot[0] += a; tot[1] += b; tot[2] += c
    if not dry and o != s: open(f, 'w', encoding='utf-8').write(o)
print('$%%d -> Money_Str: %d, $%%s -> ₾%%s: %d, $0: %d' % tuple(tot))
