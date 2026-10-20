import sys, os
rec = sys.argv[1]
mods = [l.strip() for l in open(os.path.join(rec, 'order.txt'))]
head = '''// LSSC 2027 - Los Santos Street Crime's - gamemode (sorgente ricostruito dalla v8)
#include <open.mp>
#include <a_mysql>
#include <samp_bcrypt>
#include <cef>

#pragma dynamic 32768

#include "modules/core/enums.inc"
#include "modules/core/forwards.inc"
'''
inc = ''.join('#include "%s"\n' % m[len('gamemodes/'):] for m in mods if m != 'gamemodes/lsrp.pwn')
body = open(os.path.join(rec, 'gamemodes/lsrp.pwn')).read()
open(os.path.join(rec, 'gamemodes/lsrp.pwn'), 'w').write(head + inc + '\n' + body)
os.makedirs(os.path.join(rec, 'gamemodes/modules/core'), exist_ok=True)
open(os.path.join(rec, 'gamemodes/modules/core/enums.inc'), 'w').write(open(os.path.join(rec, 'enums.txt')).read())
open(os.path.join(rec, 'gamemodes/modules/core/forwards.inc'), 'w').write(open(os.path.join(rec, 'forwards.txt')).read())
