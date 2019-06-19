# GB-Classic-Simulator
## Instruction

`GB-Classic-Simulator`是使用C++语言编写的任天堂`Gameboy`模拟器


## Dependency

* [cmake](https://cmake.org/download/)
* [sfml](https://www.sfml-dev.org)

## Compile

```sh
$ cmake .
$ make
```

## Run

```sh
$ ./run PATH_TO_ROM_FILE
```

## Keyboard Map

| Keyboard | Gameboy |
| -------- | ------- |
| <kbd>Enter</kbd>     | Start   |
|<kbd>Backspace</kbd>  | Select  |
| <kbd>↑</kbd>  | Up      |
|  <kbd>↓</kbd> | Down    |
|   <kbd>←</kbd> | Left    |
|   <kbd>→</kbd>  | Right   |
|    <kbd>X</kbd>  | A      |
|     <kbd>Z</kbd>     | B      |

## Features & TODOs

- [x] CPU模拟 支持所有指令集
- [x] 时钟模拟
- [x] 内存模拟 支持ROM-only MBC1 MBC2 MBC3
- [x] 支持图形操作 包含Background Window Sprite
- [x] 支持对游戏进行交互操作
- [x] 支持载入ROM
- [x] 可以玩数款GB游戏
- [x] 游戏存档
- [x] 声音

### TODOs：

- [ ] 支持MBC5 HuC1
- [ ] 金手指
- [ ] Timer可能有bug

## Screenshot
Menu | Gameplay
:-------------------------:|:-------------------------:
<img src="https://w630.cc/gb1.png" width="400"> | <img src="https://w630.cc/gb2.png" width="400">
<img src="https://w630.cc/gb3.png" width="400"> | <img src="https://w630.cc/gb4.png" width="400">
<img src="https://w630.cc/gb5.png" width="400"> | <img src="https://w630.cc/gb6.png" width="400">