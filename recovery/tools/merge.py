import sys, os, difflib, shutil, re
A, B, O = sys.argv[1:4]   # cef tree, nocef tree, output
def fix_cef(t):
    t = t.replace('public CEF_OnPlayerConnect(', 'public OnPlayerConnect(').replace('public CEF_OnPlayerDisconnect(', 'public OnPlayerDisconnect(')
    return t
def merge(ta, tb):
    la, lb = ta.split('\n'), tb.split('\n')
    sm = difflib.SequenceMatcher(None, la, lb, autojunk=False)
    out = []
    for op, a0, a1, b0, b1 in sm.get_opcodes():
        if op == 'equal':
            out += la[a0:a1]; continue
        ind = ''
        ref = (la[a0:a1] or lb[b0:b1])
        if a1 > a0:
            out.append('#if defined LSRP_CEF')
            out += la[a0:a1]
            if b1 > b0:
                out.append('#else')
                out += lb[b0:b1]
            out.append('#endif')
        else:
            out.append('#if !defined LSRP_CEF')
            out += lb[b0:b1]
            out.append('#endif')
    return '\n'.join(out)
if os.path.exists(O): shutil.rmtree(O)
shutil.copytree(A, O)
n = 0
for root, _, files in os.walk(os.path.join(A, 'gamemodes')):
    for f in files:
        if not f.endswith(('.inc', '.pwn')): continue
        pa = os.path.join(root, f)
        rel = os.path.relpath(pa, A)
        pb = os.path.join(B, rel)
        ta = fix_cef(open(pa).read())
        tb = open(pb).read() if os.path.exists(pb) else ''
        m = merge(ta, tb) if ta != tb else ta
        if m != ta: n += 1
        open(os.path.join(O, rel), 'w').write(m)
print('merged files', n)
