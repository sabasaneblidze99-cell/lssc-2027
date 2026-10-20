import re, struct, os, sys
fn_re = re.compile(r'\b(Util_Send|Util_SendNear|SendClientMessage|Chat_Line|SetPlayerColor|Create3DTextLabel|Lang_Create3DTextLabel|SetPlayerChatBubble|TextDrawColor|PlayerTextDrawColor|PlayerTextDrawBoxColor|PlayerTextDrawBackgroundColor)\(([^,()]+),\s*(-?\d+\.\d+)\b')
def tohex(f):
    return '0x%08X' % struct.unpack('<I', struct.pack('<f', float(f)))[0]
n = 0
for dp, _, fs in os.walk(sys.argv[1]):
    for f in fs:
        if not f.endswith(('.inc', '.pwn')): continue
        p = os.path.join(dp, f); s = open(p, encoding='utf-8').read()
        t = fn_re.sub(lambda m: '%s(%s, %s' % (m.group(1), m.group(2), tohex(m.group(3))), s)
        if t != s: open(p, 'w', encoding='utf-8').write(t); n += 1
print('files', n)
