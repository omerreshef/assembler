.entry      LIST
.extern     W
MAIN:       add r1, LIST
LOOP:       prn #480
            lea W, r6
            inc r4
            mov r1, K
            sub r2, r4
            bne END
            cmp K, #-6
            bne %END
            dec W
.entry      MAIN
            jmp %LOOP
            add L3, L3
END:        stop

STR:       .string "aaabcd"
LIST:       .data 60,-9
            .data -10
K:          .data 31
.extern L3
