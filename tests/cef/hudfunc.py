import asyncio, json, os
from playwright.async_api import async_playwright
MOCK = """
window.__emits = []; window.__focus = [];
window.cef = { _h: {}, on: function(n, f){ (this._h[n] = this._h[n] || []).push(f); },
  emit: function(){ window.__emits.push(Array.prototype.slice.call(arguments)); },
  set_focus: function(v){ window.__focus.push(v); }, hide: function(){} };
window.__fire = function(n, args){ (window.cef._h[n] || []).forEach(function(f){ f.apply(null, args); }); };
"""
async def main():
    async with async_playwright() as p:
        b = await p.chromium.launch(); pg = await b.new_page(viewport={"width":1920,"height":1080})
        errs=[]; pg.on("pageerror", lambda e: errs.append(str(e)))
        await pg.add_init_script(MOCK)
        await pg.goto("file://" + os.path.abspath(os.path.join(os.path.dirname(__file__), "../../cef/assets/index.html"))); await pg.wait_for_timeout(300)
        f = lambda n, *a: pg.evaluate("([n,a]) => window.__fire(n,a)", [n, list(a)])
        res = {}
        res['ready'] = await pg.evaluate("window.__emits.map(e=>e[0])")
        await f("lssc:hud_visible", 1); await f("lssc:identity", "Giorgi_Beridze", 7)
        await f("lssc:status", 55, 0, 30, -1); await f("lssc:hud", 1500, 200, 3, 4, 8, 12, 5)
        await f("lssc:chat_mode", 1)
        await f("lssc:chat", 0, "{FF6347}<b>xss</b> test")
        res['name'] = await pg.inner_text("#st-name"); res['hp'] = await pg.inner_text("#st-v-hp")
        res['thirstHidden'] = await pg.evaluate("document.getElementById('st-r-thirst').hidden")
        res['chatHtmlSafe'] = await pg.evaluate("document.querySelector('.ch-line').innerHTML.indexOf('<b>') < 0")
        await f("lssc:veh_show", 1, "Premier"); await f("lssc:veh", 120, -1, 1, 0, 80, 0)
        res['speed'] = await pg.inner_text("#vh-speed"); res['fuelHidden'] = await pg.evaluate("document.getElementById('vh-fuel').hidden")
        await f("lssc:veh_show", 0, ""); res['vehHidden'] = await pg.evaluate("document.querySelector('.vh').hidden")
        # chat input via F6
        await pg.keyboard.press("F6"); await pg.wait_for_timeout(50)
        await pg.keyboard.type("გამარჯობა world"); await pg.keyboard.press("Enter")
        em = await pg.evaluate("window.__emits.filter(e=>e[0]=='lssc:chat_send')")
        res['chat_send'] = em
        res['decoded'] = bytes.fromhex(em[0][1]).decode() if em else None
        res['focus'] = await pg.evaluate("window.__focus")
        await f("lssc:quick_open", 3); await pg.wait_for_timeout(50)
        res['quickDisabled'] = await pg.evaluate("[...document.querySelectorAll('.qa-btn')].map(b=>b.dataset.a+':'+b.disabled)")
        await pg.click('.qa-btn[data-a="phone"]')
        res['quick'] = await pg.evaluate("window.__emits.filter(e=>e[0]=='lssc:quick')")
        for i in range(7): await f("lssc:notify", 1, "t%d"%i, "x", 4000)
        res['notifVisible'] = await pg.evaluate("document.querySelectorAll('.nt').length")
        await f("lssc:hud", 1750, 200, 4, 0, 10, 12, 6); await f("lssc:money_delta", 250, 0); await f("lssc:levelup", 4)
        res['delta'] = await pg.evaluate("document.querySelector('.money-delta') && document.querySelector('.money-delta').textContent")
        print(json.dumps(res, ensure_ascii=False, indent=1)); print("errors", errs)
        await b.close()
if __name__ == "__main__":
    asyncio.run(main())
