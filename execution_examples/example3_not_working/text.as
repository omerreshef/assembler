.entry      LIST
.extern     W
MAIN:       add r3, LIST
LOOP:       prn #
            prn #wrong
            lea W, r9
            inc r6
            mov r3, K
            sub r1, a3
            bne END
            cmp K, #-6
            bne %END
            dec W
.entry      MAIN
            jmp %LOOP
            add L3, L3
END:        stop

STR:       .string "abcd"
LIST:       .data 6,-9
            .data -100
K:          .data 31
.extern L3
