# Hello, World for GameGear

## About

ゲームギアで `HELLO,WORLD!` を表示するサンプルプログラムです。

## How to Build

MacOS or Linux の Terminal で make を実行して生成される ROM ファイル（hello.gg）を適当なゲームギアエミュレータで起動


```
 % make
clang bmp2chr.c -o bmp2chr
./bmp2chr chrdata.bmp chrdata.vram chrdata.cram 
INPUT: width=256, height=512, bits=8(256), cmp=0
succeed.
z80asm -b hello.asm
dd bs=16k conv=sync if=hello.bin of=hello_00.rom
0+1 records in
1+0 records out
16384 bytes transferred in 0.000036 secs (455095872 bytes/sec)
dd bs=16k conv=sync if=chrdata.vram of=hello_01.rom
1+0 records in
1+0 records out
16384 bytes transferred in 0.000094 secs (174414916 bytes/sec)
dd bs=16k conv=sync if=chrdata.cram of=hello_02.rom
0+1 records in
1+0 records out
16384 bytes transferred in 0.000038 secs (429496730 bytes/sec)
cat hello_00.rom hello_01.rom hello_02.rom > hello.gg
```

