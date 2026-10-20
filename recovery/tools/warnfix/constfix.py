import re, os, subprocess, sys, json
ROOT='/home/claude/lssc-2027/gamemodes'
FILES=[os.path.join(dp,f) for dp,_,fs in os.walk(ROOT+'/modules') for f in fs if f.endswith('.inc')]+[ROOT+'/lsrp.pwn']
DEF=re.compile(r'^((?:stock |public |static |forward )*)((?:[A-Za-z_]\w*:)?)([A-Za-z_]\w*)\((.*)\)(;?)\s*$')
def split_params(p):
    out=[];depth=0;cur=''
    for ch in p:
        if ch in '{[(': depth+=1
        if ch in '}])': depth-=1
        if ch==',' and depth==0: out.append(cur);cur=''
        else: cur+=ch
    if cur.strip(): out.append(cur)
    return out
def constify(param):
    s=param.strip()
    if s.startswith('const ') or s.startswith('&') or '[' not in s or s.startswith('{') or s=='...': return param, False
    name=s.split('[')[0]
    if '=' in s.split('[')[0]: return param,False
    lead=param[:len(param)-len(param.lstrip())]
    return lead+'const '+s, True
SKIP=set(sys.argv[1].split(',')) if len(sys.argv)>1 and sys.argv[1] else set()
changed={}
for fn in FILES:
    lines=open(fn).read().split('\n'); mod=False
    for i,l in enumerate(lines):
        m=DEF.match(l)
        if not m: continue
        # definition must be followed by '{' (or be a forward ending with ;)
        isdecl = m.group(5)==';'
        if not isdecl and not (i+1<len(lines) and lines[i+1].strip()=='{'): continue
        name=m.group(3)
        if name in SKIP: continue
        if name.startswith('On') and 'public' in m.group(1): continue  # callback open.mp: firma fissa
        ps=split_params(m.group(4)); anyc=False
        for k,p in enumerate(ps):
            np_,c=constify(p); ps[k]=np_; anyc|=c
        if anyc:
            lines[i]=m.group(1)+m.group(2)+name+'('+','.join(ps)+')'+m.group(5)
            changed.setdefault(name,[]).append(fn); mod=True
    if mod: open(fn,'w').write('\n'.join(lines))
json.dump(changed,open('/home/claude/wfix/changed.json','w'))
print(len(changed),'functions changed')
