all:
	clang bmp2chr.c -o bmp2chr
	./bmp2chr chrdata.bmp chrdata.vram chrdata.cram 
	z80asm -b hello.asm
	dd bs=16k conv=sync if=hello.bin of=hello_00.rom
	dd bs=16k conv=sync if=chrdata.vram of=hello_01.rom
	dd bs=16k conv=sync if=chrdata.cram of=hello_02.rom
	cat hello_00.rom hello_01.rom hello_02.rom > hello.gg

