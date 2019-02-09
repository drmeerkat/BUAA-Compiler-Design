    .data
        var_para1: .space 4 #0
        var_para2: .space 4 #1
        var_para3: .space 4 #2
        var_para4: .space 4 #3
        var_para5: .space 4 #4
        var_para6: .space 4 #5
        var_total: .space 4 #6
        str_n:.asciiz ""
        str_0:.asciiz ""
        str_1:.asciiz "result test failed!"
        str_2:.asciiz "para1 test failed!"
        str_3:.asciiz "para2 test failed!"
        str_4:.asciiz "para3 test failed!"
        str_5:.asciiz "para4 test failed!"
        str_6:.asciiz "All test passed"
    .text
        addi $sp, $sp, 4
        move $fp, $sp
        j Func_main
    Func_add1: #7
        addi $sp, $sp, -4 #8
        sw $ra, 0($sp) #8
        addi $sp, $sp, -72 #8
        move $fp, $sp
        la $gp, var_para1 #9
        sw $s0, 40($fp) #9save for caller
        lw $s0, 0($gp) #9 para1 add1
        move $t1, $s0 #9
        li $t4, 1 #10
        addu $t2, $t1, $t4 #10
        la $gp, var_para1 #11
        sw $t2, 0($gp) #11 para1 add1
        la $gp, var_para1 #12
        sw $s1, 44($fp) #12save for caller
        lw $s1, 0($gp) #12 para1 add1
        move $t7, $s1 #12
        move $v1, $t7 #13 #2
        lw $s0, 40($fp) #13
        lw $s1, 44($fp) #13
        lw $ra, 72($fp) #13
        addi $sp, $sp, 76 #13
        move $fp, $sp
        jr $ra #13
    Func_add2: #15
        addi $sp, $sp, -4 #16
        sw $ra, 0($sp) #16
        addi $sp, $sp, -72 #16
        move $fp, $sp
        la $gp, var_para2 #17
        sw $s2, 48($fp) #17save for caller
        lw $s2, 0($gp) #17 para2 add2
        move $t9, $s2 #17
        li $t2, 1 #18
        addu $t0, $t9, $t2 #18
        la $gp, var_para2 #19
        sw $t0, 0($gp) #19 para2 add2
        la $gp, var_para2 #20
        sw $s3, 52($fp) #20save for caller
        lw $s3, 0($gp) #20 para2 add2
        move $t5, $s3 #20
        move $v1, $t5 #21 #5
        lw $s2, 48($fp) #21
        lw $s3, 52($fp) #21
        lw $ra, 72($fp) #21
        addi $sp, $sp, 76 #21
        move $fp, $sp
        jr $ra #21
    Func_add3: #23
        addi $sp, $sp, -4 #24
        sw $ra, 0($sp) #24
        addi $sp, $sp, -72 #24
        move $fp, $sp
        la $gp, var_para3 #25
        sw $s4, 56($fp) #25save for caller
        lw $s4, 0($gp) #25 para3 add3
        move $t7, $s4 #25
        li $t0, 1 #26
        addu $t8, $t7, $t0 #26
        la $gp, var_para3 #27
        sw $t8, 0($gp) #27 para3 add3
        la $gp, var_para3 #28
        sw $s5, 60($fp) #28save for caller
        lw $s5, 0($gp) #28 para3 add3
        move $t3, $s5 #28
        move $v1, $t3 #29 #8
        lw $s4, 56($fp) #29
        lw $s5, 60($fp) #29
        lw $ra, 72($fp) #29
        addi $sp, $sp, 76 #29
        move $fp, $sp
        jr $ra #29
    Func_add4: #31
        addi $sp, $sp, -4 #32
        sw $ra, 0($sp) #32
        addi $sp, $sp, -72 #32
        move $fp, $sp
        la $gp, var_para4 #33
        sw $s6, 64($fp) #33save for caller
        lw $s6, 0($gp) #33 para4 add4
        move $t5, $s6 #33
        li $t8, 1 #34
        addu $t6, $t5, $t8 #34
        la $gp, var_para4 #35
        sw $t6, 0($gp) #35 para4 add4
        la $gp, var_para4 #36
        sw $s7, 68($fp) #36save for caller
        lw $s7, 0($gp) #36 para4 add4
        move $t1, $s7 #36
        move $v1, $t1 #37 #11
        lw $s6, 64($fp) #37
        lw $s7, 68($fp) #37
        lw $ra, 72($fp) #37
        addi $sp, $sp, 76 #37
        move $fp, $sp
        jr $ra #37
    Func_add5: #39
        addi $sp, $sp, -4 #40
        sw $ra, 0($sp) #40
        addi $sp, $sp, -72 #40
        move $fp, $sp
        la $gp, var_para5 #41
        sw $s0, 40($fp) #41save for caller
        lw $s0, 0($gp) #41 para5 add5
        move $t3, $s0 #41
        li $t6, 1 #42
        addu $t4, $t3, $t6 #42
        la $gp, var_para5 #43
        sw $t4, 0($gp) #43 para5 add5
        la $gp, var_para5 #44
        sw $s1, 44($fp) #44save for caller
        lw $s1, 0($gp) #44 para5 add5
        move $t9, $s1 #44
        move $v1, $t9 #45 #14
        lw $s0, 40($fp) #45
        lw $s1, 44($fp) #45
        lw $ra, 72($fp) #45
        addi $sp, $sp, 76 #45
        move $fp, $sp
        jr $ra #45
    Func_add6: #47
        addi $sp, $sp, -4 #48
        sw $ra, 0($sp) #48
        addi $sp, $sp, -72 #48
        move $fp, $sp
        la $gp, var_para6 #49
        sw $s2, 48($fp) #49save for caller
        lw $s2, 0($gp) #49 para6 add6
        move $t1, $s2 #49
        li $t4, 1 #50
        addu $t2, $t1, $t4 #50
        la $gp, var_para6 #51
        sw $t2, 0($gp) #51 para6 add6
        la $gp, var_para6 #52
        sw $s3, 52($fp) #52save for caller
        lw $s3, 0($gp) #52 para6 add6
        move $t7, $s3 #52
        move $v1, $t7 #53 #17
        lw $s2, 48($fp) #53
        lw $s3, 52($fp) #53
        lw $ra, 72($fp) #53
        addi $sp, $sp, 76 #53
        move $fp, $sp
        jr $ra #53
    Func_minus1: #55
        addi $sp, $sp, -4 #56
        sw $ra, 0($sp) #56
        addi $sp, $sp, -72 #56
        move $fp, $sp
        la $gp, var_para1 #57
        sw $s4, 56($fp) #57save for caller
        lw $s4, 0($gp) #57 para1 minus1
        move $t9, $s4 #57
        li $t2, 1 #58
        subu $t0, $t9, $t2 #58
        la $gp, var_para1 #59
        sw $t0, 0($gp) #59 para1 minus1
        la $gp, var_para1 #60
        sw $s5, 60($fp) #60save for caller
        lw $s5, 0($gp) #60 para1 minus1
        move $t5, $s5 #60
        move $v1, $t5 #61 #20
        lw $s4, 56($fp) #61
        lw $s5, 60($fp) #61
        lw $ra, 72($fp) #61
        addi $sp, $sp, 76 #61
        move $fp, $sp
        jr $ra #61
    Func_minus2: #63
        addi $sp, $sp, -4 #64
        sw $ra, 0($sp) #64
        addi $sp, $sp, -72 #64
        move $fp, $sp
        la $gp, var_para2 #65
        sw $s6, 64($fp) #65save for caller
        lw $s6, 0($gp) #65 para2 minus2
        move $t7, $s6 #65
        li $t0, 1 #66
        subu $t8, $t7, $t0 #66
        la $gp, var_para2 #67
        sw $t8, 0($gp) #67 para2 minus2
        la $gp, var_para2 #68
        sw $s7, 68($fp) #68save for caller
        lw $s7, 0($gp) #68 para2 minus2
        move $t3, $s7 #68
        move $v1, $t3 #69 #23
        lw $s6, 64($fp) #69
        lw $s7, 68($fp) #69
        lw $ra, 72($fp) #69
        addi $sp, $sp, 76 #69
        move $fp, $sp
        jr $ra #69
    Func_minus3: #71
        addi $sp, $sp, -4 #72
        sw $ra, 0($sp) #72
        addi $sp, $sp, -72 #72
        move $fp, $sp
        la $gp, var_para3 #73
        sw $s0, 40($fp) #73save for caller
        lw $s0, 0($gp) #73 para3 minus3
        move $t5, $s0 #73
        li $t8, 1 #74
        subu $t6, $t5, $t8 #74
        la $gp, var_para3 #75
        sw $t6, 0($gp) #75 para3 minus3
        la $gp, var_para3 #76
        sw $s1, 44($fp) #76save for caller
        lw $s1, 0($gp) #76 para3 minus3
        move $t1, $s1 #76
        move $v1, $t1 #77 #26
        lw $s0, 40($fp) #77
        lw $s1, 44($fp) #77
        lw $ra, 72($fp) #77
        addi $sp, $sp, 76 #77
        move $fp, $sp
        jr $ra #77
    Func_minus4: #79
        addi $sp, $sp, -4 #80
        sw $ra, 0($sp) #80
        addi $sp, $sp, -72 #80
        move $fp, $sp
        la $gp, var_para4 #81
        sw $s2, 48($fp) #81save for caller
        lw $s2, 0($gp) #81 para4 minus4
        move $t3, $s2 #81
        li $t6, 1 #82
        subu $t4, $t3, $t6 #82
        la $gp, var_para4 #83
        sw $t4, 0($gp) #83 para4 minus4
        la $gp, var_para4 #84
        sw $s3, 52($fp) #84save for caller
        lw $s3, 0($gp) #84 para4 minus4
        move $t9, $s3 #84
        move $v1, $t9 #85 #29
        lw $s2, 48($fp) #85
        lw $s3, 52($fp) #85
        lw $ra, 72($fp) #85
        addi $sp, $sp, 76 #85
        move $fp, $sp
        jr $ra #85
    Func_minus5: #87
        addi $sp, $sp, -4 #88
        sw $ra, 0($sp) #88
        addi $sp, $sp, -72 #88
        move $fp, $sp
        la $gp, var_para5 #89
        sw $s4, 56($fp) #89save for caller
        lw $s4, 0($gp) #89 para5 minus5
        move $t1, $s4 #89
        li $t4, 1 #90
        subu $t2, $t1, $t4 #90
        la $gp, var_para5 #91
        sw $t2, 0($gp) #91 para5 minus5
        la $gp, var_para5 #92
        sw $s5, 60($fp) #92save for caller
        lw $s5, 0($gp) #92 para5 minus5
        move $t7, $s5 #92
        move $v1, $t7 #93 #32
        lw $s4, 56($fp) #93
        lw $s5, 60($fp) #93
        lw $ra, 72($fp) #93
        addi $sp, $sp, 76 #93
        move $fp, $sp
        jr $ra #93
    Func_minus6: #95
        addi $sp, $sp, -4 #96
        sw $ra, 0($sp) #96
        addi $sp, $sp, -72 #96
        move $fp, $sp
        la $gp, var_para6 #97
        sw $s6, 64($fp) #97save for caller
        lw $s6, 0($gp) #97 para6 minus6
        move $t9, $s6 #97
        li $t2, 1 #98
        subu $t0, $t9, $t2 #98
        la $gp, var_para6 #99
        sw $t0, 0($gp) #99 para6 minus6
        la $gp, var_para6 #100
        sw $s7, 68($fp) #100save for caller
        lw $s7, 0($gp) #100 para6 minus6
        move $t5, $s7 #100
        move $v1, $t5 #101 #35
        lw $s6, 64($fp) #101
        lw $s7, 68($fp) #101
        lw $ra, 72($fp) #101
        addi $sp, $sp, 76 #101
        move $fp, $sp
        jr $ra #101
    Func_func1: #103
        addi $sp, $sp, -4 #108
        sw $ra, 0($sp) #108
        addi $sp, $sp, -72 #108
        move $fp, $sp
        la $gp, var_total #109
        sw $s0, 40($fp) #109save for caller
        lw $s0, 0($gp) #109 total func1
        move $t7, $s0 #109
        li $t8, 6 #110
        bge $t7, $t8, Label_0 #110
        la $gp, var_total #111
        sw $s1, 44($fp) #111save for caller
        lw $s1, 0($gp) #111 total func1
        move $t1, $s1 #111
        li $t4, 2 #112
        addu $t2, $t1, $t4 #112
        la $gp, var_total #113
        sw $t2, 0($gp) #113 total func1
        jal Func_add1 #114
        move $t6, $v1 #114
        sw $t6, 24($fp) #115
        jal Func_minus1 #115
        move $t7, $v1 #115
        lw $t6, 24($fp) #115
        addu $t8, $t6, $t7 #116
        sw $t8, 32($fp) #117
        jal Func_minus2 #117
        move $t1, $v1 #117
        lw $t8, 32($fp) #117
        subu $t2, $t8, $t1 #118
        sw $t2, 8($fp) #119
        jal Func_add2 #119
        move $t5, $v1 #119
        lw $t2, 8($fp) #119
        addu $t6, $t2, $t5 #120
        addi $sp, $sp, -4
        sw $a1, 88($fp) #121 push for caller
        move $a1, $t6 #121
        jal Func_minus1 #122
        addi $fp, $fp, 4 #122 half para/adjust fp
        move $t0, $v1 #122
        addi $sp, $sp, -4
        sw $a1, 4($sp) #123 push for last callee
        move $a1, $t0 #123
        jal Func_add2 #124
        addi $fp, $fp, 8 #124 half para/adjust fp
        move $t2, $v1 #124
        addi $sp, $sp, -4
        sw $a2, 84($fp) #125 push for caller
        move $a2, $t2 #125
        addi $sp, $sp, -4
        sw $a3, 80($fp) #126 push for caller
        move $a3, $a3 #126
        jal Func_add4 #127
        addi $fp, $fp, 16 #127 half para/adjust fp
        move $t5, $v1 #127
        addi $sp, $sp, -4
        sw $t5, 0($sp) #128
        jal Func_func1 #129
        addi $fp, $fp, 4 #129 half para/adjust fp
        lw $a1, 0($sp) #129 load for last callee
        lw $a2, 84($fp) #129 load for caller
        lw $a3, 80($fp) #129 load for caller
        move $t7, $v1 #129
        addi $sp, $sp, -4
        sw $a2, 84($fp) #130 push for caller
        move $a2, $t7 #130
        jal Func_add3 #131
        addi $fp, $fp, 8 #131 half para/adjust fp
        move $t9, $v1 #131
        addi $sp, $sp, -4
        sw $a3, 80($fp) #132 push for caller
        move $a3, $t9 #132
        sw $s2, 48($fp) #133save for caller
        lw $s2, 88($fp) #133 para1 func1
        addi $sp, $sp, -4
        sw $a1, 12($sp) #133 push for last callee
        move $a1, $s2 #133
        sw $s3, 52($fp) #134save for caller
        lw $s3, 84($fp) #134 para2 func1
        addi $sp, $sp, -4
        sw $a2, 12($sp) #134 push for last callee
        move $a2, $s3 #134
        sw $s4, 56($fp) #135save for caller
        lw $s4, 80($fp) #135 para3 func1
        addi $sp, $sp, -4
        sw $a3, 12($sp) #135 push for last callee
        move $a3, $s4 #135
        sw $s5, 60($fp) #136save for caller
        lw $s5, 76($fp) #136 para4 func1
        addi $sp, $sp, -4
        sw $s5, 0($sp) #136
        jal Func_func1 #137
        addi $fp, $fp, 12 #137 half para/adjust fp
        lw $a1, 8($sp) #137 load for last callee
        lw $a2, 4($sp) #137 load for last callee
        lw $a3, 0($sp) #137 load for last callee
        move $t5, $v1 #137
        addi $sp, $sp, -4
        sw $t5, 0($sp) #138
        jal Func_func1 #139
        lw $a1, 88($fp) #139 load for caller
        lw $a2, 84($fp) #139 load for caller
        lw $a3, 80($fp) #139 load for caller
        move $t7, $v1 #139
        addu $t8, $t7, $a1 #140
        addu $t1, $t8, $a2 #141
        addu $t4, $t1, $a3 #142
        sw $s6, 64($fp) #143save for caller
        lw $s6, 76($fp) #143 para4 func1
        addu $t7, $t4, $s6 #143
        move $v1, $t7 #144 #56
        lw $s0, 40($fp) #144
        lw $s1, 44($fp) #144
        lw $s2, 48($fp) #144
        lw $s3, 52($fp) #144
        lw $s4, 56($fp) #144
        lw $s5, 60($fp) #144
        lw $s6, 64($fp) #144
        lw $ra, 72($fp) #144
        addi $sp, $sp, 92 #144
        move $fp, $sp
        jr $ra #144
    Label_0: #145
        addu $t0, $a1, $a2 #146
        addu $t3, $t0, $a3 #147
        sw $s7, 68($fp) #148save for caller
        lw $s7, 76($fp) #148 para4 func1
        addu $t6, $t3, $s7 #148
        move $v1, $t6 #149 #59
        lw $s0, 40($fp) #149
        lw $s1, 44($fp) #149
        lw $s2, 48($fp) #149
        lw $s3, 52($fp) #149
        lw $s4, 56($fp) #149
        lw $s5, 60($fp) #149
        lw $s6, 64($fp) #149
        lw $s7, 68($fp) #149
        lw $ra, 72($fp) #149
        addi $sp, $sp, 92 #149
        move $fp, $sp
        jr $ra #149
    Func_main: #151
        addi $sp, $sp, -4 #152
        sw $ra, 0($sp) #152
        addi $sp, $sp, -80 #152
        move $fp, $sp
        li $t9, 0 #155
        sw $t9, 72($fp) #155 flag main
        li $t0, 1 #156
        la $gp, var_para1 #156
        sw $t0, 0($gp) #156 para1 main
        li $t1, 0 #157
        la $gp, var_para2 #157
        sw $t1, 0($gp) #157 para2 main
        li $t2, 1 #158
        la $gp, var_para3 #158
        sw $t2, 0($gp) #158 para3 main
        li $t3, 0 #159
        la $gp, var_para4 #159
        sw $t3, 0($gp) #159 para4 main
        li $t4, 1 #160
        la $gp, var_para5 #160
        sw $t4, 0($gp) #160 para5 main
        li $t5, 1 #161
        la $gp, var_para6 #161
        sw $t5, 0($gp) #161 para6 main
        li $t6, 0 #162
        la $gp, var_total #162
        sw $t6, 0($gp) #162 total main
        la $gp, var_para1 #163
        sw $s0, 40($fp) #163save for caller
        lw $s0, 0($gp) #163 para1 main
        move $t8, $s0 #163
        addi $sp, $sp, -4
        move $a1, $t8 #164
        la $gp, var_para2 #165
        sw $s1, 44($fp) #165save for caller
        lw $s1, 0($gp) #165 para2 main
        move $t1, $s1 #165
        addi $sp, $sp, -4
        move $a2, $t1 #166
        la $gp, var_para3 #167
        sw $s2, 48($fp) #167save for caller
        lw $s2, 0($gp) #167 para3 main
        move $t4, $s2 #167
        addi $sp, $sp, -4
        move $a3, $t4 #168
        la $gp, var_para4 #169
        sw $s3, 52($fp) #169save for caller
        lw $s3, 0($gp) #169 para4 main
        move $t7, $s3 #169
        addi $sp, $sp, -4
        sw $t7, 0($sp) #170
        jal Func_func1 #171
        move $t9, $v1 #171
        sw $t9, 76($fp) #172 result main
        la $a0, str_0 #173
        li $v0, 4 #173
        syscall #173
        lw $a0, 76($fp) #173 result main
        li $v0, 1 #173
        syscall #173
        la $a0, str_n #173
        li $v0, 4 #173
        syscall #173
        sw $s4, 56($fp) #174save for caller
        lw $s4, 76($fp) #174 result main
        li $t1, 38 #174
        beq $s4, $t1, Label_1 #174
        la $a0, str_1 #175
        li $v0, 4 #175
        syscall #175
        la $a0, str_n #175
        li $v0, 4 #175
        syscall #175
        li $t3, 1 #176
        sw $t3, 72($fp) #176 flag main
    Label_1: #177
        la $gp, var_para1 #178
        sw $s5, 60($fp) #178save for caller
        lw $s5, 0($gp) #178 para1 main
        move $t5, $s5 #178
        li $t6, -2 #179
        beq $t5, $t6, Label_2 #179
        la $a0, str_2 #180
        li $v0, 4 #180
        syscall #180
        la $a0, str_n #180
        li $v0, 4 #180
        syscall #180
        li $t8, 1 #181
        sw $t8, 72($fp) #181 flag main
    Label_2: #182
        la $gp, var_para2 #183
        sw $s6, 64($fp) #183save for caller
        lw $s6, 0($gp) #183 para2 main
        move $t0, $s6 #183
        li $t1, 3 #184
        beq $t0, $t1, Label_3 #184
        la $a0, str_3 #185
        li $v0, 4 #185
        syscall #185
        la $a0, str_n #185
        li $v0, 4 #185
        syscall #185
        li $t3, 1 #186
        sw $t3, 72($fp) #186 flag main
    Label_3: #187
        la $gp, var_para3 #188
        sw $s7, 68($fp) #188save for caller
        lw $s7, 0($gp) #188 para3 main
        move $t5, $s7 #188
        li $t6, 4 #189
        beq $t5, $t6, Label_4 #189
        la $a0, str_4 #190
        li $v0, 4 #190
        syscall #190
        la $a0, str_n #190
        li $v0, 4 #190
        syscall #190
        li $t8, 1 #191
        sw $t8, 72($fp) #191 flag main
    Label_4: #192
        la $gp, var_para4 #193
        lw $s0, 0($gp) #193 para4 main
        move $t0, $s0 #193
        li $t1, 3 #194
        beq $t0, $t1, Label_5 #194
        la $a0, str_5 #195
        li $v0, 4 #195
        syscall #195
        la $a0, str_n #195
        li $v0, 4 #195
        syscall #195
        li $t3, 1 #196
        sw $t3, 72($fp) #196 flag main
    Label_5: #197
        lw $s1, 72($fp) #198 flag main
        li $t6, 1 #198
        subu $t4, $s1, $t6 #198
        beq $t4, $0, Label_6 #199
        la $a0, str_6 #200
        li $v0, 4 #200
        syscall #200
        la $a0, str_n #200
        li $v0, 4 #200
        syscall #200
    Label_6: #201
        li $v0, 10 #202
        syscall #202
