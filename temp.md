
## ----------------- Devlog XVIII -----------------

\- 🌲 **Textuers** 🧚‍♀️ - 👩‍💻 ***Shadoowwsss*** 💀-

> To recap, my project is a custom made rendering engine that focuses on diverse rendering options along with high stylistic control (very high-level explanation).

---

## ✨ What did I do? ☀️

To recap on what I did:

- Got the program to launch to black screen
  - *I needed to inject the Asset Manager to a class*
- Got objects loading correctly without shadows
- Got textures to load
- Started work on fixing the shadow map

Nothing too crazy has been changed, the texture issues were about some compatability problems and not uploading to the same index.

The shadow values are uplaoded to a 3d texture, where to calculate if an object is in sight or not, we commit triangles based on their position. The problem now is about the far plane, and hitting triangels that weren't in shadow.

---

## :miku: Thank You for Reading :3-blahaj-spinning

I still have to fix the shadows issue (obviuosly) but then I can move onto formatting the data!!

Github repo: [https://github.com/pkncoder/Engine](https://github.com/pkncoder/Engine/tree/Phase-IV/)
Project landing page: [https://pkncoder.github.io/Engine](https://pkncoder.github.io/Engine/)

---

## ❓ What's next? 🛑

1. Write the shadow cubemap geometry shader & ad it to the OpaquePass (temp main pass)
2. Make and debug UBOs replacing base uniforms
3. Refactor / clean
4. Create the real Render Graph?

---

## 🥀 Git Commits🗿
