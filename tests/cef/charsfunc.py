"""Test vista chars (M15): lista 3 slot, scelta, modulo di creazione, validazione, errori dal server."""
import asyncio, json, os, sys
from playwright.async_api import async_playwright
sys.path.insert(0, os.path.dirname(__file__))
from hudfunc import MOCK
HERE = os.path.dirname(os.path.abspath(__file__))
async def main():
    os.makedirs(os.path.join(HERE, "shots"), exist_ok=True)
    async with async_playwright() as p:
        b = await p.chromium.launch(); errs = []; res = {}
        pg = await b.new_page(viewport={"width": 1920, "height": 1080})
        pg.on("pageerror", lambda e: errs.append(str(e)))
        await pg.add_init_script(MOCK)
        await pg.goto("file://" + os.path.abspath(os.path.join(HERE, "../../cef/assets/index.html"))); await pg.wait_for_timeout(200)
        f = lambda n, *a: pg.evaluate("([n,a]) => window.__fire(n,a)", [n, list(a)])
        em = lambda n: pg.evaluate("(n)=>window.__emits.filter(e=>e[0]==n)", n)
        await f("lssc:chars_begin", 3)
        await f("lssc:chars_add", 0, 1, "Giorgi_Beridze", 7, 15400, 7, "3 სთ წინ")
        await f("lssc:chars_add", 1, 0, "", 0, 0, 0, ""); await f("lssc:chars_add", 2, 0, "", 0, 0, 0, "")
        await f("lssc:open", "chars", ""); await pg.wait_for_timeout(200)
        await pg.screenshot(path=os.path.join(HERE, "shots", "chars_list.png"))
        res["slots"] = await pg.evaluate("document.querySelectorAll('.cs-slot').length")
        await pg.click(".cs-slot.empty"); await pg.wait_for_timeout(100)
        await pg.fill("#cs-first", "giorgi9"); await pg.fill("#cs-last", "kapanadze")
        await pg.fill("#cs-d", "14"); await pg.fill("#cs-m", "5"); await pg.fill("#cs-y", "1998")
        res["preview"] = await pg.inner_text(".cs-preview")
        await pg.screenshot(path=os.path.join(HERE, "shots", "chars_new.png"))
        await pg.keyboard.press("Enter"); await pg.wait_for_timeout(100)
        res["create"] = await em("lssc:char_create")
        await f("lssc:chars_err", 2); await pg.wait_for_timeout(100)
        res["errShown"] = await pg.inner_text(".cs-err"); res["keptFirst"] = await pg.input_value("#cs-first")
        await pg.screenshot(path=os.path.join(HERE, "shots", "chars_err.png"))
        await pg.keyboard.press("Escape"); await pg.wait_for_timeout(100)
        res["backToList"] = await pg.evaluate("document.querySelectorAll('.cs-slot').length"); res["back"] = await em("lssc:char_back")
        await pg.click(".cs-slot:not(.empty)"); res["pick"] = await em("lssc:char_pick")
        await f("lssc:close", "chars"); res["closed"] = await pg.evaluate("document.getElementById('view-chars').hidden")
        # account nuovo: 3 vuoti + chars_new
        await f("lssc:chars_begin", 3)
        for s in range(3): await f("lssc:chars_add", s, 0, "", 0, 0, 0, "")
        await f("lssc:open", "chars", ""); await f("lssc:chars_new", 0); await pg.wait_for_timeout(100)
        res["newNoBack"] = await pg.evaluate("[...document.querySelectorAll('.cs-btn')].map(b=>b.textContent)")
        for w, h in [(1280, 720), (2560, 1440)]:
            await pg.set_viewport_size({"width": w, "height": h}); await pg.wait_for_timeout(100)
            r = await pg.evaluate("(()=>{const r=document.querySelector('.cs-wrap').getBoundingClientRect();return [r.left,r.top,r.right,r.bottom]})()")
            res["inside_%d" % w] = r[0] >= 0 and r[1] >= 0 and r[2] <= w and r[3] <= h
        res["focus"] = await pg.evaluate("window.__focus")
        print(json.dumps(res, ensure_ascii=False, indent=1)); print("errors", errs)
        await b.close()
asyncio.run(main())
