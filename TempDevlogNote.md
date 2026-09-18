## ----------------- Devlog XXIV -----------------

-- i ***__HATE__*** shadow maps!!! - Renderer? I hardly know her? --

> To recap, my project is a custom made rendering engine that focuses on diverse rendering options along with high stylistic control (very high-level explanation).

---

## ✨ What *__did__* I do? ☀️

WOAH
---
A *__lot__* has happened. Previously, I had just implemented a new entity system & a new camera system. So what's new?

I present: The Deferred Renderer!!! Basically, it's just another way of rendering. **HOWEVER**, it has some funzies:
- Mostly together render graph, just a little smthin
	- Shadow Pass
	- G-Buffer Pass
	- Lighting Pass
- New point lights (replacing the emmissive texture searching before)
- Looking to add SSAO (hemisphere sampling) in the future!!

Now, along with this, I just tested creating a build for windows and testing it on a friends windows machine, and it worked ~~(his laptop is crap, so it ran slow, but surprisingly fast)~~. Along with that, I also tried the current build on mac, and it *also* worked, so that's also great!!

However, the real prize, is the __trypophobia__ horrors made along the way!! **That's right!** When trying to get textures to work, I ran into a ton of problems (mostly related to the alpha map & image formats) that caused major headaches. Images and videos, where they are a bit compressed, can be found below.

## :miku: Thank You for Reading :3-blahaj-spinning: 

Almost there, I still need to change up the landing page, rewrite the readme, and merge the main branch, plus try to get a way to change the 3d models for users, but I'm getting there.

Github repo: [https://github.com/pkncoder/Engine](https://github.com/pkncoder/Engine/tree/temporaryWorkingBranch/)

Project landing page: [https://pkncoder.github.io/Engine](https://pkncoder.github.io/Engine/)

---
## ❓ What’s next? 🛑

- Change my Rasterizer to be a Defered Renderer & implement multiple point lights
- UI??