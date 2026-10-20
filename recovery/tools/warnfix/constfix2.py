# secondo giro: const per singolo parametro nelle funzioni escluse dal primo giro
import re, os, sys, json
ROOT='/home/claude/lssc-2027/gamemodes'
FILES=[os.path.join(dp,f) for dp,_,fs in os.walk(ROOT+'/modules') for f in fs if f.endswith('.inc')]+[ROOT+'/lsrp.pwn']
DEF=re.compile(r'^((?:stock |public |static |forward )*)((?:[A-Za-z_]\w*:)?)([A-Za-z_]\w*)\((.*)\)(;?)\s*$')
TARGET=set(json.load(open('/home/claude/wfix/skip.json')))
SKIP=set(sys.argv[1].split(',')) if len(sys.argv)>1 and sys.argv[1] else set()
def split_params(p):
    out=[];depth=0;cur=''
    for ch in p:
        if ch in '{[(': depth+=1
        if ch in '}])': depth-=1
        if ch==',' and depth==0: out.append(cur);cur=''
        else: cur+=ch
    if cur.strip(): out.append(cur)
    return out
def body_of(lines,i):
    depth=0; out=[]
    for j in range(i+1,len(lines)):
        out.append(lines[j]); depth+=lines[j].count('{')-lines[j].count('}')
        if depth==0 and j>i+1: break
    return '\n'.join(out)
def written(name, body):
    n=re.escape(name)
    if re.search(r'\b'+n+r'\s*\[[^\]]*\](\s*\[[^\]]*\])?\s*(=(?!=)|\+\+|--|[-+*/|&^%]=|<<=|>>=)', body): return True
    if re.search(r'(\+\+|--)\s*'+n+r'\s*\[', body): return True
    if re.search(r'\b'+n+r'\s*=(?!=)', body): return True
    # usato come argomento di qualunque funzione: lo decide il compilatore (errore -> si toglie)
    if re.search(r'\b(format|strcat|strmid|strins|strdel|strpack|strunpack|valstr|GetPlayerName|GetPlayerIp|cache_get_value_name|cache_get_value_index|tolower|toupper)\s*\(\s*'+n+r'\b', body): return True
    if re.search(r'#emit[^\n]*\b'+n+r'\b', body): return True
    return False
done={}
for fn in FILES:
    lines=open(fn).read().split('\n'); mod=False
    decl={}
    for i,l in enumerate(lines):
        m=DEF.match(l)
        if not m or m.group(3) not in TARGET or m.group(3) in SKIP: continue
        if m.group(5)==';': continue
        if not (i+1<len(lines) and lines[i+1].strip()=='{'): continue
        if m.group(3).startswith('On') and 'public' in m.group(1): continue
        body=body_of(lines,i)
        ps=split_params(m.group(4)); any_=False
        for k,p in enumerate(ps):
            s=p.strip()
            if s.startswith('const ') or s.startswith('&') or '[' not in s or s.startswith('{'): continue
            name=re.sub(r'^[A-Za-z_]\w*:','',s.split('[')[0]).strip()
            if written(name,body): continue
            lead=p[:len(p)-len(p.lstrip())]; ps[k]=lead+'const '+s; any_=True
        if any_:
            lines[i]=m.group(1)+m.group(2)+m.group(3)+'('+','.join(ps)+')'
            done[m.group(3)]=lines[i]; mod=True
    if mod: open(fn,'w').write('\n'.join(lines))
# allinea i forward
for fn in FILES:
    lines=open(fn).read().split('\n'); mod=False
    for i,l in enumerate(lines):
        m=DEF.match(l)
        if m and m.group(5)==';' and m.group(3) in done:
            d=DEF.match(done[m.group(3)])
            lines[i]=m.group(1)+d.group(2)+d.group(3)+'('+d.group(4)+');'; mod=True
    if mod: open(fn,'w').write('\n'.join(lines))
print(len(done))
