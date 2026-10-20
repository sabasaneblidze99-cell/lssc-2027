"""Corregge i warning 213 (tag mismatch) riga per riga, senza cambiare il bytecode:
letterali -> costanti con nome (true/false, DIALOG_STYLE_*, ...) oppure override di tag (Tag:expr / _:expr).
Uso: python3 tagfix.py <file warning>"""
import re, os, sys, glob, collections

ROOT = '/home/claude/lssc-2027/gamemodes'
INC = '/home/claude/lssc-2027/toolchain/include'

# ------------------------------------------------------------------ firme
SIG = {}   # nome -> lista di tag dei parametri ('' = nessuno, '...' = variadico)
RET = {}   # nome -> tag del valore di ritorno
VTAG = {}  # nome variabile -> tag
EFIELD = {}  # campo enum -> tag
HDR = re.compile(r'^\s*(?:native|forward|stock|public|static|stock static)?\s*(?:native\s+)?((?:[A-Za-z_]\w*:)?)([A-Za-z_]\w*)\s*\((.*?)\)\s*(?:=\s*\w+\s*)?;?\s*$')

def split_args(s):
    out = []; depth = 0; cur = ''; q = None
    i = 0
    while i < len(s):
        ch = s[i]
        if q:
            cur += ch
            if ch == '\\' and i + 1 < len(s):
                cur += s[i + 1]; i += 2; continue
            if ch == q: q = None
        elif ch in '"\'':
            q = ch; cur += ch
        elif ch in '([{':
            depth += 1; cur += ch
        elif ch in ')]}':
            depth -= 1; cur += ch
        elif ch == ',' and depth == 0:
            out.append(cur); cur = ''
        else:
            cur += ch
        i += 1
    if cur.strip() or out: out.append(cur)
    return out

def ptag(p):
    p = p.strip()
    if p.startswith('...') or re.search(r'\}\s*:\s*\.\.\.', p) or p.endswith('...'):
        return '...'
    p = re.sub(r'^const\s+', '', p)
    p = p.lstrip('&').strip()
    p = re.sub(r'^const\s+', '', p)
    m = re.match(r'([A-Za-z_]\w*):', p)
    return m.group(1) if m else ''

def scan(path):
    txt = open(path, errors='replace').read()
    txt = re.sub(r'/\*.*?\*/', '', txt, flags=re.S)
    LL = txt.split('\n')
    for li, line in enumerate(LL):
        line = re.sub(r'//.*', '', line)
        m = HDR.match(line)
        real = re.match(r'^\s*(native|forward|stock|public|static)\b', line) or (not line.startswith((' ', '\t')) and li + 1 < len(LL) and LL[li + 1].strip() == '{')
        if m and real and m.group(2) not in ('if', 'while', 'for', 'switch', 'return', 'sizeof', 'case'):
            name = m.group(2)
            if name not in SIG or line.lstrip().startswith(('native', 'forward')):
                SIG[name] = [ptag(p) for p in split_args(m.group(3))] if m.group(3).strip() else []
                RET[name] = (m.group(1) or '').rstrip(':')
        for vm in re.finditer(r'(?:\bnew|\bstatic|,|\(|^\s*)\s*(?:const\s+)?([A-Za-z_]\w*):([A-Za-z_]\w*)\s*(?=[\[=,;)\s])', line):
            tag, nm = vm.group(1), vm.group(2)
            if tag not in ('case', 'default', 'native', 'forward', 'public', 'stock', 'http', 'https') and not tag.startswith('SV_'):
                VTAG.setdefault(nm, tag)
for f in glob.glob(INC + '/**/*.inc', recursive=True): scan(f)
for f in glob.glob(ROOT + '/modules/**/*.inc', recursive=True) + [ROOT + '/lsrp.pwn']: scan(f)
# campi enum: righe tipo "\tbool:pField," dentro gli enum
for f in glob.glob(ROOT + '/modules/**/*.inc', recursive=True):
    for line in open(f):
        m = re.match(r'^\s+([A-Za-z_]\w*):([A-Za-z_]\w*)(\[[^\]]*\])?\s*,?\s*$', line)
        if m: EFIELD.setdefault(m.group(2), m.group(1))

# ------------------------------------------------------------------ costanti con nome
CONST = {
    'bool': {'0': 'false', '1': 'true'},
    't_DIALOG_STYLE': {'0': 'DIALOG_STYLE_MSGBOX', '1': 'DIALOG_STYLE_INPUT', '2': 'DIALOG_STYLE_LIST', '3': 'DIALOG_STYLE_PASSWORD',
                       '4': 'DIALOG_STYLE_TABLIST', '5': 'DIALOG_STYLE_TABLIST_HEADERS'},
}
for f in glob.glob(INC + '/**/*.inc', recursive=True):
    for line in open(f, errors='replace'):
        m = re.match(r'#define\s+([A-Z][A-Z0-9_]+)\s+\(([A-Za-z_]\w*):(-?\d+)\)', line)
        if m and (not m.group(1).startswith('INVALID') or m.group(2) in ('PlayerText', 'Text', 'Text3D', 'PlayerText3D')):
            CONST.setdefault(m.group(2), {}).setdefault(m.group(3), m.group(1))
def norm(t):
    return t[2:] if t and t.startswith('t_') else t
def literal_for(tag, lit):
    tag = norm(tag)
    lit = lit.strip()
    if tag in CONST and lit in CONST[tag]:
        return CONST[tag][lit]
    if tag == 'bool' and re.fullmatch(r'-?\d+', lit):
        return 'true' if lit != '0' else 'false'
    return None

SIMPLE = re.compile(r'^[A-Za-z_]\w*(\s*\[[^\[\]]*\])*$|^-?\d+(\.\d+)?$|^0x[0-9A-Fa-f]+$|^[A-Za-z_]\w*\s*\([^()]*\)$')
def retag(tag, expr):
    """espressione con il tag atteso, stesso valore"""
    tag = norm(tag)
    e = expr.strip()
    lead = expr[:len(expr) - len(expr.lstrip())]; trail = expr[len(expr.rstrip()):]
    lit = literal_for(tag, e)
    if lit: return lead + lit + trail
    if tag == 'bool':
        m = re.fullmatch(r'(.*\?\s*)(-?\d+)(\s*:\s*)(-?\d+)', e, re.S)
        if m and literal_for('bool', m.group(2)) and literal_for('bool', m.group(4)):
            return lead + m.group(1) + literal_for('bool', m.group(2)) + m.group(3) + literal_for('bool', m.group(4)) + trail
    t = '_' if tag == '' else tag
    if SIMPLE.match(e):
        return lead + t + ':' + e + trail
    return lead + t + ':(' + e + ')' + trail

def expr_tag(e):
    e = e.strip()
    if e in ('true', 'false'): return 'bool'
    if re.fullmatch(r'-?\d+\.\d+', e): return 'Float'
    m = re.match(r'^([A-Za-z_]\w*):', e)
    if m: return m.group(1)
    m = re.fullmatch(r'([A-Za-z_]\w*)\s*\(.*\)', e, re.S)
    if m and m.group(1) in RET: return RET[m.group(1)]
    m = re.fullmatch(r'([A-Za-z_]\w*)((\s*\[[^\[\]]*\])*)', e)
    if m:
        idx = re.findall(r'\[([^\[\]]*)\]', m.group(2) or '')
        if idx and idx[-1].strip() in EFIELD: return EFIELD[idx[-1].strip()]
        if idx and re.fullmatch(r'[a-z]{1,4}[A-Z]\w*', idx[-1].strip()): return ''   # campo enum senza tag
        return VTAG.get(m.group(1), '')
    if e.startswith('!') or re.search(r'(==|!=|<=|>=|&&|\|\|)', e) and not re.search(r'\?', e): return 'bool'
    return None   # sconosciuto

# ------------------------------------------------------------------ correzione di una riga
def fix_calls(line, want, found):
    """argomenti delle chiamate: tag atteso dal parametro"""
    out = line; changed = False
    pos = 0
    while True:
        m = re.compile(r'\b([A-Za-z_]\w*)\s*\(').search(out, pos)
        if not m: break
        name = m.group(1); start = m.end()
        depth = 1; i = start; q = None
        while i < len(out) and depth:
            ch = out[i]
            if q:
                if ch == '\\': i += 1
                elif ch == q: q = None
            elif ch in '"\'': q = ch
            elif ch == '(': depth += 1
            elif ch == ')': depth -= 1
            i += 1
        if depth: break
        inner = out[start:i - 1]
        sig = SIG.get(name)
        if sig is not None and name not in ('if', 'while', 'for', 'switch', 'return', 'sizeof', 'tagof'):
            args = split_args(inner) if inner.strip() else []
            new = []; ch_ = False
            for k, a in enumerate(args):
                pt = sig[k] if k < len(sig) else (sig[-1] if sig and sig[-1] == '...' else None)
                if pt is None or pt == '...':
                    new.append(a); continue
                if norm(pt) == norm(want) and want != '_':
                    et = expr_tag(a)
                    if et in ('', None):
                        na = retag(want, a)
                        if na != a: new.append(na); ch_ = True; continue
                if want == '_' and pt == '' and found:
                    et = expr_tag(a)
                    if et == found:
                        new.append(retag('', a)); ch_ = True; continue
                new.append(a)
            if ch_:
                out = out[:start] + ','.join(new) + out[i - 1:]
                changed = True
                i = start + len(','.join(new)) + 1
        pos = start
    return out, changed

def fix_line(line, want, found, functag):
    orig = line
    # return <expr>;
    m = re.match(r'^(\s*return\s+)(.*?)(;\s*)$', line)
    if m and norm(functag) == norm(want) and want != '_':
        et = expr_tag(m.group(2))
        if et in ('', None) or literal_for(want, m.group(2).strip()):
            return m.group(1) + retag(want, m.group(2)) + m.group(3)
    if m and want == '_' and functag == '' and found:
        et = expr_tag(m.group(2))
        if et == found:
            return m.group(1) + retag('', m.group(2)) + m.group(3)
    # chiamate
    line, ch = fix_calls(line, want, found)
    if ch: return line
    # assegnazioni / confronti / inizializzazioni:  lhs (=|==|!=) rhs
    def rep(mm):
        lhs, op, rhs = mm.group(1), mm.group(2), mm.group(3)
        lt = expr_tag(lhs.split(':')[-1] if ':' in lhs and not lhs.strip().startswith(('bool:', 'Float:')) else lhs)
        if re.match(r'^\s*(bool|Float|[A-Z]\w*):', lhs.strip()):
            lt = re.match(r'^\s*(\w+):', lhs.strip()).group(1)
        if lt is not None and norm(lt) == norm(want) and want != '_':
            et = expr_tag(rhs)
            if et in ('', None) or literal_for(want, rhs.strip()):
                return lhs + op + retag(want, rhs)
        if want == '_' and lt in ('', None) and found and expr_tag(rhs) == found:
            return lhs + op + retag('', rhs)
        return mm.group(0)
    pat = re.compile(r'((?:\bnew\s+|\bstatic\s+)?(?:[A-Za-z_]\w*:)?[A-Za-z_]\w*(?:\s*\[[^\[\]]*\])*\s*)(=(?!=)|==|!=)((?:[^,;()=]|\([^()]*\)|\[[^\[\]]*\])+?)(?=\s*[,;)]|\s*$|\s*&&|\s*\|\|)')
    nl = pat.sub(rep, line)
    return nl

def main():
    W = collections.defaultdict(list)
    for l in open(sys.argv[1]):
        m = re.match(r'(.+?)\((\d+)\) : warning 213: tag mismatch: expected tag (?:"(\w+)"|none \("_"\)), but found (?:"(\w+)"|none \("_"\))', l)
        if m:
            W[m.group(1)].append((int(m.group(2)), m.group(3) or '_', m.group(4) or ''))
    total = fixed = 0
    for f, items in W.items():
        p = os.path.join(ROOT, f)
        L = open(p).read().split('\n')
        seen = set()
        for ln, want, found in items:
            if (ln, want, found) in seen: continue
            seen.add((ln, want, found)); total += 1
            i = ln - 1
            # tag della funzione che contiene la riga
            j = i
            functag = ''
            while j >= 0:
                hm = re.match(r'^(?:stock |public |static )*([A-Za-z_]\w*:)?([A-Za-z_]\w*)\(', L[j])
                if hm and j + 1 < len(L) and L[j + 1].strip() == '{':
                    functag = (hm.group(1) or '').rstrip(':'); break
                j -= 1
            nl = fix_line(L[i], want, found, functag)
            if nl != L[i]:
                L[i] = nl; fixed += 1
        open(p, 'w').write('\n'.join(L))
    print('righe', total, 'modificate', fixed)

if __name__ == '__main__':
    main()
