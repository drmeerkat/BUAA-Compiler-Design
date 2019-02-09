    .data
        str_n:.asciiz ""
        str_0:.asciiz ""
        str_1:.asciiz " x = "
        str_2:.asciiz " y = "
        str_3:.asciiz " SWAP x = "
        str_4:.asciiz " SWAP y = "
        str_5:.asciiz " OVERFLOW!          "
        str_6:.asciiz " complete number: "
        str_7:.asciiz "  "
        str_8:.asciiz "          "
        str_9:.asciiz " ---------------------------------------------------------------          "
        str_10:.asciiz " b = "
        str_11:.asciiz " "
        str_12:.asciiz "          The total is "
        str_13:.asciiz " 10! = "
    .text
        addi $sp, $sp, 4
        move $fp, $sp
        j Func_main
    Func_factorial: #1
        addi $sp, $sp, -4 #3
        sw $ra, 0($sp) #3
        addi $sp, $sp, -72 #3
        move $fp, $sp
        li $t0, 1 #4
        bgt $a1, $t0, Label_0 #4
        li $v1, 1 #5
        lw $ra, 72($fp) #5
        addi $sp, $sp, 80 #5
        move $fp, $sp
        jr $ra #5
    Label_0: #6
        li $t4, 1 #7
        subu $t2, $a1, $t4 #7
        addi $sp, $sp, -4
        sw $a1, 76($fp) #8 push for caller
        move $a1, $t2 #8
        jal Func_factorial #9
        lw $a1, 76($fp) #9 load for caller
        move $t1, $v1 #9
        mul $t2, $a1, $t1 #10
        move $v1, $t2 #11 #2
        lw $ra, 72($fp) #11
        addi $sp, $sp, 80 #11
        move $fp, $sp
        jr $ra #11
    Func_mod: #13
        addi $sp, $sp, -4 #16
        sw $ra, 0($sp) #16
        addi $sp, $sp, -72 #16
        move $fp, $sp
        div $t0, $a1, $a2 #17
        mul $t3, $t0, $a2 #18
        subu $t2, $a1, $t3 #19
        move $a1, $t2 #20 x mod
        move $v1, $a1 #21 x mod
        lw $ra, 72($fp) #21
        addi $sp, $sp, 84 #21
        move $fp, $sp
        jr $ra #21
    Func_swap: #23
        addi $sp, $sp, -4 #26
        sw $ra, 0($sp) #26
        addi $sp, $sp, -76 #26
        move $fp, $sp
        la $a0, str_1 #28
        li $v0, 4 #28
        syscall #28
        move $a0, $a1 #28 x swap
        li $v0, 1 #28
        syscall #28
        la $a0, str_n #28
        li $v0, 4 #28
        syscall #28
        la $a0, str_2 #29
        li $v0, 4 #29
        syscall #29
        move $a0, $a2 #29 y swap
        li $v0, 1 #29
        syscall #29
        la $a0, str_n #29
        li $v0, 4 #29
        syscall #29
        sw $a1, 72($fp) #30 temp swap
        move $a1, $a2 #31 x swap
        sw $s0, 40($fp) #32save for caller
        lw $s0, 72($fp) #32 temp swap
        move $a2, $s0 #32 y swap
        la $a0, str_3 #33
        li $v0, 4 #33
        syscall #33
        move $a0, $a1 #33 x swap
        li $v0, 1 #33
        syscall #33
        la $a0, str_n #33
        li $v0, 4 #33
        syscall #33
        la $a0, str_4 #34
        li $v0, 4 #34
        syscall #34
        move $a0, $a2 #34 y swap
        li $v0, 1 #34
        syscall #34
        la $a0, str_n #34
        li $v0, 4 #34
        syscall #34
        lw $s0, 40($fp) #35
        lw $ra, 76($fp) #35
        addi $sp, $sp, 88 #35
        move $fp, $sp
        jr $ra #35
    Func_complete_num: #37
        addi $sp, $sp, -4 #39
        sw $ra, 0($sp) #39
        addi $sp, $sp, -636 #39
        move $fp, $sp
        move $s4, $a1 #54 j complete_num
    Label_1: #55
        li $t1, 1024 #56
        bge $s4, $t1, Label_2 #56
        li $t3, -1 #57
        move $s5, $t3 #57 n complete_num
        move $s6, $s4 #58 s complete_num
        li $t0, 1 #59
        move $s3, $t0 #59 i complete_num
    Label_3: #60
        bge $s3, $s4, Label_4 #61
        div $t3, $s4, $s3 #62
        mul $t1, $t3, $s3 #63
        move $s7, $t1 #64 x1 complete_num
        addi $sp, $sp, -4
        sw $a1, 640($fp) #65 push for caller
        move $a1, $s4 #65
        addi $sp, $sp, -4
        move $a2, $s3 #66
        jal Func_mod #67
        lw $a1, 640($fp) #67 load for caller
        move $t3, $v1 #67
        li $t4, 0 #68
        bne $t3, $t4, Label_5 #68
        li $t3, 1 #69
        addu $t1, $s5, $t3 #69
        move $s5, $t1 #70 n complete_num
        subu $t0, $s6, $s3 #71
        move $s6, $t0 #72 s complete_num
        li $t4, 128 #73
        blt $s5, $t4, Label_6 #73
        la $a0, str_5 #74
        li $v0, 4 #74
        syscall #74
        la $a0, str_n #74
        li $v0, 4 #74
        syscall #74
    Label_6: #75
        li $t1, 128 #76
        bge $s5, $t1, Label_7 #76
        move $t4, $s5 #77find subscript reg
        li $t0, 158
        sub $t4, $t0, $t4
        mul $t4, $t4, 4
        add $t4, $t4, $fp #77calculate subscript
        sw $s3, 0($t4) #77 k complete_num
    Label_7: #78
    Label_5: #79
        li $t3, 1 #80
        addu $t1, $s3, $t3 #80
        move $s3, $t1 #81 i complete_num
        j Label_3 #82
    Label_4: #83
        li $t0, 0 #84
        bne $s6, $t0, Label_8 #84
        la $a0, str_6 #85
        li $v0, 4 #85
        syscall #85
        move $a0, $s4 #85 j complete_num
        li $v0, 1 #85
        syscall #85
        la $a0, str_n #85
        li $v0, 4 #85
        syscall #85
        li $t2, 0 #86
        move $s3, $t2 #86 i complete_num
    Label_9: #87
        bgt $s3, $s5, Label_10 #88
        move $t1, $s3 #89find subscript reg
        li $t2, 158
        sub $t1, $t2, $t1
        mul $t1, $t1, 4
        add $t1, $t1, $fp #89calculate subscript
        sw $s1, 44($fp) #89save for caller
        lw $s1, 0($t1) #89 k complete_num
        move $t3, $s1 #89
        la $a0, str_7 #90
        li $v0, 4 #90
        syscall #90
        move $a0, $t3 #90 #12
        li $v0, 1 #90
        syscall #90
        la $a0, str_n #90
        li $v0, 4 #90
        syscall #90
        li $t1, 1 #91
        addu $t4, $s3, $t1 #91
        move $s3, $t4 #92 i complete_num
        j Label_9 #93
    Label_10: #94
        la $a0, str_8 #95
        li $v0, 4 #95
        syscall #95
        la $a0, str_n #95
        li $v0, 4 #95
        syscall #95
    Label_8: #96
        li $t0, 1 #97
        addu $t3, $s4, $t0 #97
        move $s4, $t3 #98 j complete_num
        j Label_1 #99
    Label_2: #100
        la $a0, str_9 #101
        li $v0, 4 #101
        syscall #101
        la $a0, str_n #101
        li $v0, 4 #101
        syscall #101
        li $t2, 0 #102
        move $t9, $t2 #102 a complete_num
        li $t3, 0 #103
        move $s3, $t3 #103 i complete_num
    Label_11: #104
        li $t4, 1024 #105
        bge $s3, $t4, Label_12 #105
        mul $t1, $t9, $s3 #106
        sw $t1, 76($fp) #107 b complete_num
        mul $t0, $s4, $s5 #108
        sw $t0, 72($fp) #109 c complete_num
        mul $t4, $t9, $s3 #110
        move $t9, $t4 #111 a complete_num
        sw $s2, 48($fp) #112save for caller
        lw $s2, 76($fp) #112 b complete_num
        addu $t3, $t9, $s2 #112
        sw $s0, 40($fp) #113save for caller
        lw $s0, 72($fp) #113 c complete_num
        addu $t1, $t3, $s0 #113
        move $t9, $t1 #114 a complete_num
        li $t3, 1 #115
        addu $t1, $s3, $t3 #115
        move $s3, $t1 #116 i complete_num
        j Label_11 #117
    Label_12: #118
        sw $t9, 76($fp) #119 b complete_num
        li $t1, 0 #120
        move $s3, $t1 #120 i complete_num
    Label_13: #121
        li $t2, 1024 #122
        bge $s3, $t2, Label_14 #122
        lw $s1, 76($fp) #123 b complete_num
        addu $t4, $s1, $s3 #123
        sw $t4, 76($fp) #124 b complete_num
        li $t0, 1 #125
        addu $t3, $s3, $t0 #125
        move $s3, $t3 #126 i complete_num
        j Label_13 #127
    Label_14: #128
        la $a0, str_10 #129
        li $v0, 4 #129
        syscall #129
        lw $a0, 76($fp) #129 b complete_num
        li $v0, 1 #129
        syscall #129
        la $a0, str_n #129
        li $v0, 4 #129
        syscall #129
        la $a0, str_9 #130
        li $v0, 4 #130
        syscall #130
        la $a0, str_n #130
        li $v0, 4 #130
        syscall #130
        li $t2, 0 #131
        move $t6, $t2 #131 h complete_num
        li $t3, 1 #132
        move $t8, $t3 #132 leap complete_num
        li $t4, 2 #133
        move $t5, $t4 #133 m complete_num
    Label_15: #134
        li $t0, 1024 #135
        bgt $t5, $t0, Label_16 #135
        li $t4, 2 #136
        div $t2, $t5, $t4 #136
        sw $t2, 96($fp) #137 k2 complete_num
        li $t1, 2 #138
        move $s3, $t1 #138 i complete_num
    Label_17: #139
        lw $s2, 96($fp) #140 k2 complete_num
        bgt $s3, $s2, Label_18 #140
        div $t4, $t5, $s3 #141
        mul $t2, $t4, $s3 #142
        move $t7, $t2 #143 x2 complete_num
        addi $sp, $sp, -4
        sw $a1, 640($fp) #144 push for caller
        move $a1, $t5 #144
        addi $sp, $sp, -4
        move $a2, $s3 #145
        jal Func_mod #146
        lw $a1, 640($fp) #146 load for caller
        move $t4, $v1 #146
        li $t0, 0 #147
        bne $t4, $t0, Label_19 #147
        li $t2, 0 #148
        move $t8, $t2 #148 leap complete_num
    Label_19: #149
        li $t0, 1 #150
        addu $t3, $s3, $t0 #150
        move $s3, $t3 #151 i complete_num
        j Label_17 #152
    Label_18: #153
        li $t2, 1 #154
        bne $t8, $t2, Label_20 #154
        la $a0, str_11 #155
        li $v0, 4 #155
        syscall #155
        move $a0, $t5 #155 m complete_num
        li $v0, 1 #155
        syscall #155
        la $a0, str_n #155
        li $v0, 4 #155
        syscall #155
        li $t1, 1 #156
        addu $t4, $t6, $t1 #156
        move $t6, $t4 #157 h complete_num
        li $t0, 10 #158
        div $t3, $t6, $t0 #158
        li $t4, 10 #159
        mul $t1, $t3, $t4 #159
        move $t7, $t1 #160 x2 complete_num
        bne $t7, $t6, Label_21 #161
        la $a0, str_8 #162
        li $v0, 4 #162
        syscall #162
        la $a0, str_n #162
        li $v0, 4 #162
        syscall #162
    Label_21: #163
    Label_20: #164
        li $t3, 1 #165
        move $t8, $t3 #165 leap complete_num
        li $t1, 1 #166
        addu $t4, $t5, $t1 #166
        move $t5, $t4 #167 m complete_num
        j Label_15 #168
    Label_16: #169
        la $a0, str_12 #170
        li $v0, 4 #170
        syscall #170
        move $a0, $t6 #170 h complete_num
        li $v0, 1 #170
        syscall #170
        la $a0, str_n #170
        li $v0, 4 #170
        syscall #170
        lw $s1, 44($fp) #171
        lw $s2, 48($fp) #171
        lw $s0, 40($fp) #171
        lw $ra, 636($fp) #171
        addi $sp, $sp, 644 #171
        move $fp, $sp
        jr $ra #171
    Func_main: #173
        addi $sp, $sp, -4 #174
        sw $ra, 0($sp) #174
        addi $sp, $sp, -76 #174
        move $fp, $sp
        li $t3, 10 #176
        addi $sp, $sp, -4
        move $a1, $t3 #176
        jal Func_factorial #177
        move $t4, $v1 #177
        sw $t4, 72($fp) #178 n main
        la $a0, str_13 #179
        li $v0, 4 #179
        syscall #179
        lw $a0, 72($fp) #179 n main
        li $v0, 1 #179
        syscall #179
        la $a0, str_n #179
        li $v0, 4 #179
        syscall #179
        li $t1, 5 #180
        addi $sp, $sp, -4
        move $a1, $t1 #180
        li $t2, 10 #181
        addi $sp, $sp, -4
        move $a2, $t2 #181
        jal Func_swap #182
        move $t3, $v1 #182
        li $t4, 2 #183
        addi $sp, $sp, -4
        move $a1, $t4 #183
        jal Func_complete_num #184
        move $t0, $v1 #184
        li $v0, 10 #185
        syscall #185
