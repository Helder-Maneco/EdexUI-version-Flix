# EdexUI-version-Flix

# Edex-CLI

> *TUI System Monitor inspirado no Edex UI, porém SEM baboseiras. Leve, rápido, dentro do terminal.*

**Stack:** C++17 + Termbox + Ruby Scripting  
**Platform:** Linux (Arch, Ubuntu, Debian)

## Features

- CPU/Memory/Disk/Network monitoring via `/proc` / `/sys`
- ASCII graphs & real-time rendering (Termbox backend)
- Customização via scripts Ruby (fork/exec IPC)
- Unix Domain Sockets para comunicação segura
- Keyboard shortcuts + themes
- ❌ Sem 3D desnecessário
- ❌ Sem efeitos sonoros irritantes
- ❌ Sem Electron bloat lol

## 📦 Installation

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target edex-cli -j4
./build/src/edex-cli
