org $0000

.init
    IM 1
    LD SP, $E000
    CALL init_vdp
    CALL load_pattern
    CALL load_palette

    DI
    ; 書き込みアドレス $22D4 (ここからHELLO,WORLD!を書けばだいたい画面真ん中)
    LD A, $D4
    OUT ($BF), A
    LD A, $62
    OUT ($BF), A
    EI

    LD C, $BE
    LD HL, text
    LD B, 24
    OTIR

    JMP end

text:
    DB "H", $00, "E", $00, "L", $00, "L", $00, "O", $00, ",", $00
    DB "W", $00, "O", $00, "R", $00, "L", $00, "D", $00, "!", $00

.init_vdp
    DI
    ; Register: モード制御 #1
    LD A, $86
    OUT ($BF), A
    LD A, $80
    OUT ($BF), A

    ; Register: モード制御 #2
    LD A, $C0
    OUT ($BF), A
    LD A, $81
    OUT ($BF), A

    ; Register: NameTable = $2000
    LD A, $F9
    OUT ($BF), A
    LD A, $82
    OUT ($BF), A

    ; Register: unused
    LD A, $FF
    OUT ($BF), A
    LD A, $83
    OUT ($BF), A

    ; Register: unused
    LD A, $FF
    OUT ($BF), A
    LD A, $84
    OUT ($BF), A

    ; Register: OAM = $2800
    LD A, $D1
    OUT ($BF), A
    LD A, $85
    OUT ($BF), A

    ; Register: SpritePattern = $0000
    LD A, $FB
    OUT ($BF), A
    LD A, $86
    OUT ($BF), A
    EI
    RET

.load_pattern
    DI
    LD A, $00
    OUT ($BF), A
    LD A, $40
    OUT ($BF), A
    EI
    LD D, 64
    LD C, $BE
    LD HL, $4000
    LD B, $00
load_pattern_loop:
    OTIR
    DEC D
    jnz load_pattern_loop
    RET

.load_palette
    DI
    LD A, $00
    OUT ($BF), A
    LD A, $C0
    OUT ($BF), A
    EI
    LD C, $BE
    LD HL, $8000
    LD B, 64
    OTIR
    RET

.end
    HALT
    JMP end
