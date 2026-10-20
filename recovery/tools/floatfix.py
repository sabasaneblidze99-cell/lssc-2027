import re, struct, sys, os
def nice(h):
    v = int(h, 16)
    b = struct.pack('<I', v)
    f = struct.unpack('<f', b)[0]
    if f != f or f in (float('inf'), float('-inf')) or abs(f) > 1e8 or (f != 0 and abs(f) < 1e-4): return None
    for p in range(1, 9):
        r = '%.*g' % (p, f)
        if 'e' in r: continue
        if struct.pack('<f', float(r)) == b:
            return r if '.' in r else r + '.0'
    return None
root = sys.argv[1]; n = 0
for dp, _, fs in os.walk(root):
    for fn in fs:
        if not fn.endswith(('.inc', '.pwn')): continue
        p = os.path.join(dp, fn); s = open(p, encoding='utf-8').read(); o = s
        # Float:0xHEX and Float:(0xHEX)
        s = re.sub(r'Float:\((0x[0-9A-F]{8})\)', lambda m: (nice(m.group(1)) or m.group(0)), s)
        s = re.sub(r'Float:(0x[0-9A-F]{8})\b', lambda m: (nice(m.group(1)) or m.group(0)), s)
        # bare hex inside Float array initializers
        out = []; infl = False; depth = 0
        for line in s.split('\n'):
            if re.match(r'^\s*(new|static)\s+(const\s+)?Float:\w+\s*\[.*=\s*\{', line) or re.match(r'^\s*new Float:\w+\[[^\]]*\]\[[^\]]*\] = \{', line):
                infl = True; depth = 0
            if infl:
                line = re.sub(r'(?<![\w:])(0x[0-9A-F]{8})\b', lambda m: (nice(m.group(1)) or m.group(1)), line)
                depth += line.count('{') - line.count('}')
                if depth <= 0 and '}' in line: infl = False
            out.append(line)
        s = '\n'.join(out)
        if s != o: open(p, 'w', encoding='utf-8').write(s); n += 1
print('files', n)
