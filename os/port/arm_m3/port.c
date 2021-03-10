
void port_PendSV_handler(void) __attribute__( ( naked ) );
void port_PendSV_handler(void)
{
    __asm volatile (
        /*turn off all interrupt*/
        "cpsid  i                                       \n"
        /*check PSP, if PSP ==0, this is the first task switch
          so we can skip 'save context' and 'select next TCB' step
         */
        "mrs    r0, psp                                 \n"
        /*if r0 == 0, jump to 1*/
        "cbz    r0,restore_context                      \n"
        /*save context*/
        "mrs    r0,psp                                  \n"
        "stmdb  r0!,{r4-r11}                            \n"
        "ldr    r1,p_current_TCB                        \n"
        "ldr    r1,[r1]                                 \n"
        "str    r0,[r1]                                 \n"
        /*select next TCB*/
        "push   {lr}                                    \n"
        "bl     switch_current_TCB                      \n"
        "pop    {lr}                                    \n"
        "restore_context:\n"
        "ldr    r0,p_current_TCB                        \n"
        "ldr    r0,[r0]                                 \n"
        "ldr    r0,[r0]                                 \n"
        "ldmia  r0!,{r4-r11}                            \n"
        "msr    psp,r0                                  \n"
        /*r1 != 0x04: lr |= 32'b0000_0000_0000_0100*/
        "orr    lr,lr,#0x04                             \n"
        /*turn on all interrupt*/
        "cpsie  i                                       \n"
        "bx     lr                                      \n"
        "p_current_TCB: .word current_TCB               \n"
    );
}
