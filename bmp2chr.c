/* 8bit Bitmap to GameGear character pattern generator & palette */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 情報ヘッダ */
struct DatHead {
    int isize;             /* 情報ヘッダサイズ */
    int width;             /* 幅 */
    int height;            /* 高さ */
    unsigned short planes; /* プレーン数 */
    unsigned short bits;   /* 色ビット数 */
    unsigned int ctype;    /* 圧縮形式 */
    unsigned int gsize;    /* 画像データサイズ */
    int xppm;              /* X方向解像度 */
    int yppm;              /* Y方向解像度 */
    unsigned int cnum;     /* 使用色数 */
    unsigned int inum;     /* 重要色数 */
};

int main(int argc, char* argv[])
{
    FILE* fpR = NULL;
    FILE* fpW = NULL;
    int rc = 0;
    char fh[14];
    int pal[256];
    struct DatHead dh;
    unsigned char bh, bl;
    unsigned char mh[4];
    int i, j, k, y, x, a;
    unsigned char bmp[131072];
    unsigned char vram[16384];
    unsigned char cram[64];

    /* 引数チェック */
    rc++;
    if (argc < 3) {
        fprintf(stderr, "usage: bmp2chr input.bmp output.ptn output.pal\n");
        goto ENDPROC;
    }

    /* 読み込みファイルをオープン */
    rc++;
    if (NULL == (fpR = fopen(argv[1], "rb"))) {
        fprintf(stderr, "ERROR: Could not open: %s\n", argv[1]);
        goto ENDPROC;
    }

    /* ファイルヘッダを読み込む */
    rc++;
    if (sizeof(fh) != fread(fh, 1, sizeof(fh), fpR)) {
        fprintf(stderr, "ERROR: Invalid file header.\n");
        goto ENDPROC;
    }

    /* 先頭2バイトだけ読む */
    rc++;
    if (strncmp(fh, "BM", 2)) {
        fprintf(stderr, "ERROR: Inuput file is not bitmap.\n");
        goto ENDPROC;
    }

    /* 情報ヘッダを読み込む */
    rc++;
    if (sizeof(dh) != fread(&dh, 1, sizeof(dh), fpR)) {
        fprintf(stderr, "ERROR: Invalid bitmap file header.\n");
        goto ENDPROC;
    }

    printf("INPUT: width=%d, height=%d, bits=%d(%d), cmp=%d\n", dh.width, dh.height, (int)dh.bits, dh.cnum, dh.ctype);

    /* 256x512でなければエラー扱い */
    rc++;
    if (256 != dh.width || 512 != dh.height) {
        fprintf(stderr, "ERROR: Invalid input bitmap size. (128x128 only)");
        goto ENDPROC;
    }

    /* 8ビットカラー以外は弾く */
    rc++;
    if (8 != dh.bits) {
        fprintf(stderr, "ERROR: This program supports only 8bit color.\n");
        goto ENDPROC;
    }

    /* 無圧縮以外は弾く */
    rc++;
    if (dh.ctype) {
        fprintf(stderr, "ERROR: This program supports only none-compress type.\n");
        goto ENDPROC;
    }

    /* パレットの先頭32色をCRAM形式に変換 */
    rc++;
    if (sizeof(pal) != fread(pal, 1, sizeof(pal), fpR)) {
        fprintf(stderr, "ERROR: Could not read palette data.\n");
        goto ENDPROC;
    }
    for (int i = 0; i < 32; i++) {
        unsigned char r = (pal[i] & 0x00F00000) >> 16;
        unsigned char g = (pal[i] & 0x0000F000) >> 12;
        unsigned char b = (pal[i] & 0x000000F0) >> 4;
        cram[i * 2] = r | g;
        cram[i * 2 + 1] = b;
    }

    /* 画像データを上下反転しながら読み込む */
    rc++;
    for (i = 511; 0 <= i; i--) {
        if (256 != fread(&bmp[i * 256], 1, 256, fpR)) {
            fprintf(stderr, "ERROR: Could not read graphic data.\n");
            goto ENDPROC;
        }
    }

    /* Bitmap を CHR に変換 */
    for (i = 0; i < 512; i++) {
        int vp = i * 32;
        int bp = i % 32 * 8 + i / 32 * 8 * 256;
        for (j = 0; j < 8; j++, vp += 4, bp += 256) {
            vram[vp] = (bmp[bp] & 0b0001) << 7;
            vram[vp] |= (bmp[bp + 1] & 0b0001) << 6;
            vram[vp] |= (bmp[bp + 2] & 0b0001) << 5;
            vram[vp] |= (bmp[bp + 3] & 0b0001) << 4;
            vram[vp] |= (bmp[bp + 4] & 0b0001) << 3;
            vram[vp] |= (bmp[bp + 5] & 0b0001) << 2;
            vram[vp] |= (bmp[bp + 6] & 0b0001) << 1;
            vram[vp] |= bmp[bp + 7] & 0b0001;

            vram[vp + 1] = (bmp[bp] & 0b0010) << 6;
            vram[vp + 1] |= (bmp[bp + 1] & 0b0010) << 5;
            vram[vp + 1] |= (bmp[bp + 2] & 0b0010) << 4;
            vram[vp + 1] |= (bmp[bp + 3] & 0b0010) << 3;
            vram[vp + 1] |= (bmp[bp + 4] & 0b0010) << 2;
            vram[vp + 1] |= (bmp[bp + 5] & 0b0010) << 1;
            vram[vp + 1] |= bmp[bp + 6] & 0b0010;
            vram[vp + 1] |= (bmp[bp + 7] & 0b0010) >> 1;

            vram[vp + 2] = (bmp[bp] & 0b0100) << 5;
            vram[vp + 2] |= (bmp[bp + 1] & 0b0100) << 4;
            vram[vp + 2] |= (bmp[bp + 2] & 0b0100) << 3;
            vram[vp + 2] |= (bmp[bp + 3] & 0b0100) << 2;
            vram[vp + 2] |= (bmp[bp + 4] & 0b0100) << 1;
            vram[vp + 2] |= bmp[bp + 5] & 0b0100;
            vram[vp + 2] |= (bmp[bp + 6] & 0b0100) >> 1;
            vram[vp + 2] |= (bmp[bp + 7] & 0b0100) >> 2;

            vram[vp + 3] = (bmp[bp] & 0b1000) << 4;
            vram[vp + 3] |= (bmp[bp + 1] & 0b1000) << 3;
            vram[vp + 3] |= (bmp[bp + 2] & 0b1000) << 2;
            vram[vp + 3] |= (bmp[bp + 3] & 0b1000) << 1;
            vram[vp + 3] |= bmp[bp + 4] & 0b1000;
            vram[vp + 3] |= (bmp[bp + 5] & 0b1000) >> 1;
            vram[vp + 3] |= (bmp[bp + 6] & 0b1000) >> 2;
            vram[vp + 3] |= (bmp[bp + 7] & 0b1000) >> 3;
        }
    }

    /* 書き込みファイルをオープンしてchrを書き込む */
    rc++;
    if (NULL == (fpW = fopen(argv[2], "wb"))) {
        fprintf(stderr, "ERROR: Could not open: %s\n", argv[2]);
        goto ENDPROC;
    }
    if (sizeof(vram) != fwrite(vram, 1, sizeof(vram), fpW)) {
        fprintf(stderr, "ERROR: File write error: %s\n", argv[2]);
        goto ENDPROC;
    }
    fclose(fpW);
    fpW = NULL;

    /* 書き込みファイルをオープンしてpalを書き込む */
    rc++;
    if (NULL == (fpW = fopen(argv[3], "wb"))) {
        fprintf(stderr, "ERROR: Could not open: %s\n", argv[2]);
        goto ENDPROC;
    }
    if (sizeof(cram) != fwrite(cram, 1, sizeof(cram), fpW)) {
        fprintf(stderr, "ERROR: File write error: %s\n", argv[3]);
        goto ENDPROC;
    }
    fclose(fpW);
    fpW = NULL;

    rc = 0;

    printf("succeed.\n");

    /* 終了処理 */
ENDPROC:
    if (fpR) {
        fclose(fpR);
    }
    if (fpW) {
        fclose(fpW);
    }
    return rc;
}
