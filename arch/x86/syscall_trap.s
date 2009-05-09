global ring3_exit
ring3_exit:
    mov eax, 1 ; 1 = SYS_EXIT
    int 0x80
