import re, os, sys
root = sys.argv[1]
idv = r'(?:\w*playerid|\w*Playerid|targetid|target|pid|driverid|passenger\w*|giveplayerid|i|j|p|pl|other|req|issuer|victim|killerid|forplayerid|ownerid|\w*Pid)'
n1 = n2 = 0
for dp, _, fs in os.walk(root):
    for f in fs:
        if not f.endswith(('.inc', '.pwn')) or f == 'enums.inc':
            continue
        p = os.path.join(dp, f)
        t = open(p, encoding='utf-8').read()
        o = t
        t = re.sub(r'\b(%s)\s*(<|>=)\s*1000\b' % idv, lambda m: '%s %s MAX_PLAYERS' % (m.group(1), m.group(2)), t)
        t = re.sub(r'^(new (?:bool:|Float:|Text:|PlayerText:|Text3D:)?\w+)\[1000\]', r'\1[MAX_PLAYERS]', t, flags=re.M)
        t = re.sub(r'\b(%s)\s*(==|!=)\s*65535\b' % idv, lambda m: '%s %s INVALID_PLAYER_ID' % (m.group(1), m.group(2)), t)
        t = re.sub(r'\b(%s)\s*=\s*65535;' % idv, lambda m: '%s = INVALID_PLAYER_ID;' % m.group(1), t)
        if t != o:
            n1 += 1
            open(p, 'w', encoding='utf-8').write(t)
print('files changed', n1)
