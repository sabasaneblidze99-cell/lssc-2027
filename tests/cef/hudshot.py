import asyncio, sys, os
SHOTS = os.path.join(os.path.dirname(os.path.abspath(__file__)), "shots")
os.makedirs(SHOTS, exist_ok=True)
from playwright.async_api import async_playwright
BASE = "file://" + os.path.abspath(os.path.join(os.path.dirname(__file__), "../../cef/assets/preview.html"))
RES = {"720": (1280, 720), "768": (1366, 768), "900": (1600, 900), "1080": (1920, 1080), "1440": (2560, 1440)}
async def main():
    scenes = sys.argv[1].split(",")
    ress = sys.argv[2].split(",")
    async with async_playwright() as p:
        b = await p.chromium.launch()
        errs = []
        for sc in scenes:
            for r in ress:
                w, h = RES[r]
                pg = await b.new_page(viewport={"width": w, "height": h})
                pg.on("console", lambda m: errs.append(m.text) if m.type == "error" else None)
                pg.on("pageerror", lambda e: errs.append(str(e)))
                await pg.goto(f"{BASE}?shot=1&res={r}&scene={sc}")
                await pg.wait_for_timeout(1500)
                await pg.screenshot(path=os.path.join(SHOTS, f"{sc}_{r}.png"))
                # overlap check inside iframe
                fr = pg.frames[1]
                boxes = await fr.evaluate("""() => {
                  const sel = {status:'.st', contract:'.hct', notif:'.nt-wrap', veh:'.vh', zone:'.zn', chat:'.ch', ia:'.ia'};
                  const o = {};
                  for (const k in sel) { const e = document.querySelector(sel[k]); if (e && !e.hidden && e.offsetParent !== null) { const r = e.getBoundingClientRect(); o[k] = [r.left, r.top, r.right, r.bottom]; } }
                  return o; }""")
                ks = list(boxes)
                ov = []
                for i in range(len(ks)):
                    for j in range(i+1, len(ks)):
                        a, c = boxes[ks[i]], boxes[ks[j]]
                        if a[0] < c[2] and c[0] < a[2] and a[1] < c[3] and c[1] < a[3]:
                            ov.append((ks[i], ks[j]))
                off = [k for k, v in boxes.items() if v[0] < 0 or v[1] < 0 or v[2] > w or v[3] > h]
                print(sc, r, "overlap:", ov, "offscreen:", off)
                await pg.close()
        print("JS errors:", errs[:10])
        await b.close()
asyncio.run(main())
