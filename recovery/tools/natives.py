"""Parse native/forward prototypes from include files -> param kinds."""
import re, glob, os


def split_params(s):
    out, cur, depth, q = [], '', 0, False
    for ch in s:
        if ch == '"':
            q = not q
        if not q:
            if ch in '({[':
                depth += 1
            elif ch in ')}]':
                depth -= 1
            elif ch == ',' and depth == 0:
                out.append(cur.strip()); cur = ''
                continue
        cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def kind_of(p):
    p = p.split('=')[0].strip()
    if p.endswith('...'):
        return 'vararg'
    if '[' in p:
        return 'array'
    q = re.sub(r'^(const\s+)', '', p)
    if q.startswith('&'):
        return 'ref'
    return 'value'


PTAGS = {}
RTAGS = {}


def tag_of(p):
    p = p.split('=')[0].strip()
    p = re.sub(r'^(const\s+)', '', p).lstrip('&').strip()
    m = re.match(r'^([A-Za-z_]\w*):', p)
    return m.group(1) if m else None


def parse_includes(dirs):
    protos = {}
    rx = re.compile(r'\bnative\s+(?:([A-Za-z_]\w*):)?\s*([A-Za-z_@]\w*)\s*\(((?:[^()]|\([^()]*\))*)\)', re.S)
    for d in dirs:
        for f in glob.glob(os.path.join(d, '**', '*.inc'), recursive=True):
            try:
                t = open(f, encoding='latin1').read()
            except Exception:
                continue
            t = re.sub(r'/\*.*?\*/', '', t, flags=re.S)
            t = re.sub(r'//[^\n]*', '', t)
            for m in rx.finditer(t):
                name = m.group(2)
                ps = split_params(m.group(3))
                if name not in protos:
                    protos[name] = [kind_of(p) for p in ps]
                    PTAGS[name] = [tag_of(p) for p in ps]
                    RTAGS[name] = m.group(1)
    return protos


def parse_forwards(dirs):
    fw = {}
    rx = re.compile(r'\bforward\s+(?:[A-Za-z_]\w*:)?\s*([A-Za-z_@]\w*)\s*\(((?:[^()]|\([^()]*\))*)\)', re.S)
    for d in dirs:
        for f in glob.glob(os.path.join(d, '**', '*.inc'), recursive=True):
            t = open(f, encoding='latin1').read()
            t = re.sub(r'/\*.*?\*/', '', t, flags=re.S)
            t = re.sub(r'//[^\n]*', '', t)
            for m in rx.finditer(t):
                fw.setdefault(m.group(1), split_params(m.group(2)))
    return fw


def rename_param(p, newname):
    # replace the parameter identifier
    m = re.match(r'^(.*?)([A-Za-z_]\w*)(\s*(?:\[.*)?(?:=.*)?)$', p.strip())
    if not m:
        return p
    pre, nm, post = m.groups()
    if nm in ('const',):
        return p
    return pre + newname + post
