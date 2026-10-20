import re, subprocess, json, os
ROOT='/home/claude/lssc-2027/gamemodes'
DEF=re.compile(r'^((?:stock |public |static |forward )*)((?:[A-Za-z_]\w*:)?)([A-Za-z_]\w*)\((.*)\)(;?)\s*$')
skip=set()
def enclosing(path,line):
    L=open(path).read().split('\n')
    for i in range(min(line-1,len(L)-1),-1,-1):
        m=DEF.match(L[i])
        if m and (m.group(5)==';' or (i+1<len(L) and L[i+1].strip()=='{')): return m.group(3)
    return None
for it in range(30):
    subprocess.run(['git','checkout','-q','--','.'],cwd=ROOT)
    subprocess.run(['python3','/home/claude/wfix/constfix.py',','.join(sorted(skip))],cwd=ROOT,capture_output=True)
    out=subprocess.run(['/home/claude/wfix/check.sh','1'],capture_output=True,text=True).stdout
    errs=[l for l in open('/home/claude/wfix/w.txt') if ' error ' in l]
    print(it, 'errors',len(errs), out.strip().split('\n')[-2] if out else '')
    if not errs: break
    new=set()
    for e in errs:
        m=re.match(r'(.+?)\((\d+)\) : error (\d+)',e)
        if not m: continue
        f=os.path.join(ROOT,m.group(1)); ln=int(m.group(2))
        fn=enclosing(f,ln)
        if fn: new.add(fn)
        # for arg mismatch: also skip the callee named on the line? leave caller
    if not new - skip:
        print('stuck', errs[:5]); break
    skip|=new
json.dump(sorted(skip),open('/home/claude/wfix/skip.json','w'))
print('skip',len(skip))
