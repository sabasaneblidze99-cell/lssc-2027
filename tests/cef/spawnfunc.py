"""Test vista spawn: rendering opzioni, scelta con tastiera/click, ESC bloccato, obbligo combattimento. Screenshot in shots/."""
import asyncio, json, os, sys
from playwright.async_api import async_playwright
sys.path.insert(0, os.path.dirname(__file__))
from hudfunc import MOCK
HERE = os.path.dirname(os.path.abspath(__file__))
async def main():
    os.makedirs(os.path.join(HERE, "shots"), exist_ok=True)
    async with async_playwright() as p:
        b = await p.chromium.launch(); errs = []; res = {}
        for (w, h) in [(1280, 720), (1920, 1080), (2560, 1440)]:
            pg = await b.new_page(viewport={"width": w, "height": h})
            pg.on("pageerror", lambda e: errs.append(str(e)))
            await pg.add_init_script(MOCK)
            await pg.goto("file://" + os.path.abspath(os.path.join(HERE, "../../cef/assets/index.html"))); await pg.wait_for_timeout(200)
            f = lambda n, *a: pg.evaluate("([n,a]) => window.__fire(n,a)", [n, list(a)])
            await f("lssc:spawn_begin", 0, 90)
            for i, (ic, t, s) in enumerate([("pin", "ბოლო ადგილი", "Ganton - გამოხვედი 4 საათის წინ"), ("home", "სახლი", "Idlewood"),
                                           ("badge", "პოლიციის განყოფილება (LSPD)", "ფრაქციის შტაბი"), ("briefcase", "სამსახური", "Autista bus"), ("city", "ქალაქი", "Unity Station")]):
                await f("lssc:spawn_add", i, ic, t, s)
            await f("lssc:open", "spawn", ""); await pg.wait_for_timeout(250)
            await pg.screenshot(path=os.path.join(HERE, "shots", "spawn_%d.png" % w))
            box = await pg.evaluate("(()=>{const r=document.querySelector('.sp-wrap').getBoundingClientRect();return [r.left,r.top,r.right,r.bottom]})()")
            res["inside_%d" % w] = box[0] >= 0 and box[1] >= 0 and box[2] <= w and box[3] <= h
            await pg.keyboard.press("Escape"); await pg.wait_for_timeout(50)
            res["escBlocked_%d" % w] = await pg.evaluate("!document.getElementById('view-spawn').hidden")
            await pg.keyboard.press("ArrowRight"); await pg.keyboard.press("Enter"); await pg.wait_for_timeout(50)
            res["pick_%d" % w] = await pg.evaluate("window.__emits.filter(e=>e[0]=='lssc:spawn_pick')")
            await f("lssc:close", "spawn"); await pg.wait_for_timeout(50)
            res["closed_%d" % w] = await pg.evaluate("document.getElementById('view-spawn').hidden")
            res["focus_%d" % w] = await pg.evaluate("window.__focus")
            if w == 1920:
                await f("lssc:spawn_begin", 1, 90); await f("lssc:spawn_add", 0, "pin", "ბოლო ადგილი", "Ganton - გამოხვედი 20 წუთის წინ")
                await f("lssc:open", "spawn", ""); await pg.wait_for_timeout(200)
                await pg.screenshot(path=os.path.join(HERE, "shots", "spawn_forced.png"))
                res["forcedCards"] = await pg.evaluate("document.querySelectorAll('.sp-card').length")
                await pg.keyboard.press("1"); await pg.wait_for_timeout(50)
                res["forcedPick"] = await pg.evaluate("window.__emits.filter(e=>e[0]=='lssc:spawn_pick').slice(-1)")
            await pg.close()
        print(json.dumps(res, ensure_ascii=False, indent=1)); print("errors", errs)
        await b.close()
asyncio.run(main())
