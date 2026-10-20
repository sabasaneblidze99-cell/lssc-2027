"""AMX (Pawn 3.10, 32-bit cells) loader with debug info.

Usage: requires image.bin (expanded code+data) produced by patched pawndisasm.
"""
import struct, re, os

OPS = {}
for line in open(os.path.join(os.path.dirname(__file__), '..', 'ops.txt')):
    m = re.match(r'/\*\s*(\d+)\*/\s*(NULL|"([^"]*)"),\s*(\w*)', line)
    if not m:
        continue
    num = int(m.group(1)); name = m.group(3); h = m.group(4)
    if name is None:
        continue
    if h.startswith('parm'):
        n = int(h[4:])
    elif h == 'do_proc':
        n = 0
    elif h in ('do_call', 'do_jump', 'do_sysreq'):
        n = 1
    elif h == 'do_casetbl':
        n = -1
    else:
        n = 0
    OPS[num] = (name, n)


class Sym:
    __slots__ = ('addr', 'tag', 'cs', 'ce', 'ident', 'vclass', 'dims', 'name')

    def __repr__(self):
        return f'Sym({self.name} a={self.addr} id={self.ident} vc={self.vclass} dims={self.dims} [{self.cs:x},{self.ce:x}])'


class Amx:
    def __init__(self, amxpath, imagepath):
        d = open(amxpath, 'rb').read()
        self.raw = d
        (self.size, magic, fv, av, self.flags, defsize, self.cod, self.dat, self.hea,
         self.stp, self.cip, pub, nat, lib, pubv, tags, nametable) = struct.unpack('<IHBBHHIIIIIIIIIII', d[:56])
        assert magic == 0xf1e0
        def name_at(o):
            e = d.index(b'\0', o)
            return d[o:e].decode('latin1')
        self.publics = {}
        for o in range(pub, nat, defsize):
            a, no = struct.unpack('<II', d[o:o + 8])
            self.publics[a] = name_at(no)
        self.natives = []
        for o in range(nat, lib, defsize):
            a, no = struct.unpack('<II', d[o:o + 8])
            self.natives.append(name_at(no))
        self.pubvars = {}
        for o in range(pubv, tags, defsize):
            a, no = struct.unpack('<II', d[o:o + 8])
            self.pubvars[a] = name_at(no)
        self.tagtab = {}
        for o in range(tags, nametable, defsize):
            a, no = struct.unpack('<II', d[o:o + 8])
            self.tagtab[a] = name_at(no)
        img = open(imagepath, 'rb').read()
        self.codesize = self.dat - self.cod
        self.code = struct.unpack('<%di' % (self.codesize // 4), img[:self.codesize])
        self.data = img[self.codesize:]
        self.datasize = len(self.data)
        self.parse_debug(d[self.size:])

    def dcell(self, addr):
        return struct.unpack('<i', self.data[addr:addr + 4])[0]

    def parse_debug(self, b):
        size, magic, fv, av, flags, nfiles, nlines, nsyms, ntags, nauto, nstates = struct.unpack('<iHBBhhhhhhh', b[:22])
        assert magic == 0xf1ef, hex(magic)
        # counts are int16 and may overflow: parse sequentially and stop by size
        o = 22
        def zs(o):
            e = b.index(b'\0', o)
            return b[o:e].decode('latin1'), e + 1
        nfiles &= 0xffff; nlines &= 0xffff; nsyms &= 0xffff
        self.files = []
        for _ in range(nfiles):
            a, = struct.unpack('<I', b[o:o + 4]); n, o = zs(o + 4)
            self.files.append((a, n))
        self.lines = []
        # lines may overflow int16: read until addresses stop being monotonic? we know true count from layout
        self._dbg = b
        self._lines_off = o
        self._nlines16 = nlines
        self._nsyms16 = nsyms
        self._ntags = ntags; self._nauto = nauto; self._nstates = nstates
        # try candidate line counts nlines + k*65536
        for k in range(0, 8):
            cnt = nlines + k * 65536
            ok = self._try_parse_rest(o, cnt)
            if ok:
                break
        else:
            raise Exception('cannot parse debug')

    def _try_parse_rest(self, o, nl):
        b = self._dbg
        if o + nl * 8 > len(b):
            return False
        lines = [struct.unpack('<Ii', b[o + i * 8:o + i * 8 + 8]) for i in range(nl)]
        # monotonic addresses
        o2 = o + nl * 8
        for k in range(0, 8):
            ns = self._nsyms16 + k * 65536
            r = self._try_syms(o2, ns)
            if r:
                self.lines = lines
                return True
        return False

    def _try_syms(self, o, ns):
        b = self._dbg
        syms = []
        try:
            for _ in range(ns):
                s = Sym()
                s.addr, s.tag, s.cs, s.ce, ident, vclass, dim = struct.unpack('<iHIIbbh', b[o:o + 18])
                o += 18
                e = b.index(b'\0', o); s.name = b[o:e].decode('latin1'); o = e + 1
                s.ident = ident; s.vclass = vclass
                s.dims = []
                if dim < 0 or dim > 4:
                    return False
                for _d in range(dim):
                    t, sz = struct.unpack('<HI', b[o:o + 6]); o += 6
                    s.dims.append((t, sz))
                if not s.name or any(ord(ch)<32 or ord(ch)>126 for ch in s.name):
                    return False
                syms.append(s)
            tags = {}
            for _ in range(self._ntags):
                t, = struct.unpack('<H', b[o:o + 2]); e = b.index(b'\0', o + 2)
                tags[t] = b[o + 2:e].decode('latin1'); o = e + 1
        except Exception:
            return False
        self.syms = syms
        self.dtags = tags
        return True

    # ---------------------------------------------------------------
    def disasm(self, start, end):
        """yield (addr, opname, args)"""
        c = self.code
        i = start // 4
        e = end // 4
        while i < e:
            op = c[i]
            if op not in OPS:
                yield (i * 4, '?%d' % op, [])
                i += 1
                continue
            name, n = OPS[op]
            if n == -1:  # casetbl
                cnt = c[i + 1]
                args = [c[i + 1], c[i + 2]]
                for k in range(cnt):
                    args.append((c[i + 3 + 2 * k], c[i + 4 + 2 * k]))
                yield (i * 4, name, args)
                i += 3 + 2 * cnt
                continue
            yield (i * 4, name, list(c[i + 1:i + 1 + n]))
            i += 1 + n
