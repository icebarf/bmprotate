# bmprotate
It's a small program I decided to write a couple years back. It was supposed to be simple: rotate a bitmap image by 90 degree clockwise and then back.

I didn't finish it back then but decided to complete it today since it was rather small.

### Build

```sh
$ cmake -S . -B build
$ cmake --build build
```

### Usage

```sh
$ ./build/bmprotate file.bmp
```
Above produces a file called `out.bmp` that's rotated by 90 degrees.


Peace.
