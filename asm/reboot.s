//.set noreorder
//# void reboot(int rom, int tv, int reset, int cic)
//.globl reboot
reboot:
    li      $9, 0x04000000|0x10000000|0x20000000
    mtc0    $9, $12
    li      $8, ((0x04040000)|0xA0000000)
    lw      $9, 0x10($8)
label10:
    andi     $9, $9, 0x001|0x004
    beqzl   $9, label10
    lw      $9, 0x10($8)
    li      $9, 0x00002|0x00008
    sw      $9, 0x10($8)
    li      $8, ((0x04600000)|0xA0000000)
    li      $9, 0x01|0x02
    sw      $9, 0x10($8)
    li      $8, ((0x04400000)|0xA0000000)
    li      $9, 0x3FF
    sw      $9, 0x0C($8)
    sw      $0, 0x24($8)
    sw      $0, 0x10($8)
    li      $8, ((0x04500000)|0xA0000000)
    sw      $0, 0x00($8)
    sw      $0, 0x04($8)
    lui     $10, hi(((0x04001000)|0xA0000000))
    lui     $11, hi(label9)
    addiu    $12, $11, 32
label2:
    lw      $8, lo(label9)($11)
    sw      $8, lo(((0x04001000)|0xA0000000))($10)
    addiu    $11, $11, 4
    bne     $11, $12, label2
    addiu    $10, $10, 4
    li      $29, ((0x04001FF0)|0xA0000000)
    move    $19, $4
    move    $20, $5
    move    $21, $6
    andi     $22, $7, 0xFF
    srl     $23, $7, 8
    beqzl   $19, label3
    li      $11, ((0x10000000)|0xA0000000)
    li      $11, ((0x06000000)|0xA0000000)
label3:
    li      $8, ((0x04600000)|0xA0000000)
    sw      $9, 0x14($8)
    sw      $9, 0x18($8)
    sw      $0, 0x1C($8)
    sw      $9, 0x20($8)
    lw      $9, ($11)
    sw      $9, 0x14($8)
    srl     $9, 8
    sw      $9, 0x18($8)
    srl     $9, 8
    sw      $9, 0x1C($8)
    srl     $9, 4
    sw      $9, 0x20($8)
    li      $8, ((0x04100000)|0xA0000000)
    lw      $9, 0x0C($8)
    andi     $9, $9, 0x001
    beqz    $9, label5
    li      $10, ((0x04000000)|0xA0000000)
    lw      $9, 0x0C($8)
label4:
    andi     $9, $9, 0x020
    bnezl   $9, label4
    lw      $9, 0x0C($8)
label5:
    addiu    $12, $11, 0xFC0
label6:
    lw      $8, 0x40($11)
    sw      $8, 0x40($10)
    addiu    $11, $11, 4
    bne     $11, $12, label6
    addiu    $10, $10, 4
    li      $11, ((0x04000000+0x40)|0xA0000000)
    lui     $31, 0xA400
    jr       $11
    ori     $31, 0x1550
label9:
    lui     $13, 0xBFC0
label8:
    lw      $8, 0x07FC($13)
    addiu   $13, $13, 0x07C0
    andi    $8, $8, 0x0080
    bnezl   $8, label8
    lui     $13, 0xBFC0
    lw      $8, 0x0024($13)
    lui     $11, 0xB000
