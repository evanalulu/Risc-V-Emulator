.global fib_rec_s

fib_rec_s:
    li t0, 1        # t0 = 1
    blt t0, a0, rec_case
    ret

rec_case:
    addi sp, sp, -24
    sd ra, (sp)
    sd a0, 8(sp)
    
    addi a0, a0, -1     # a0 = a0 (n) - 1
    jal fib_rec_s
    
    sd a0, 16(sp)
    ld a0, 8(sp)
    
    addi a0, a0, -2     # a0 = a0 (n) - 2
    jal fib_rec_s

    ld t1, 16(sp)       # t1 = fibrec_s(n - 1)
    add a0, a0, t1      # a0 = a0 (fib_rec_s(n - 2)) + t1 (fib_rec_s(n - 1))

    ld ra, (sp)
    addi sp, sp, 24
    ret
