    .syntax unified
    .text
    .global port_PendSV_handler


    .func port_PendSV_handler
    .thumb_func
    .type port_PendSV_handler, %function
    .align
port_PendSV_handler:
@ turn off all interrupt
    cpsid i
@ check PSP, if PSP == 0, this is the first task switch
@ so we can skip 'save context' and 'select next TCB' step
    mrs r0, PSP
    @ if r0 == 0, jump to restore_context
	@ LDR R1, =is_first_switch_task
    cbz r0, restore_context

    mrs r0, psp
    stmdb r0!, {r4-r11}
    ldr r1, =current_TCB
    ldr r1, [r1]
    str r0, [r1]

select_next_TCB:
	push {lr}
    bl switch_current_TCB
	pop {lr}

restore_context:
    ldr r0, =current_TCB
    ldr r0, [r0]
    ldr r0, [r0]
    ldmia r0!, {r4-r11}
    msr psp, r0
	orr lr, lr, #0x4 ; r1 |= 0x04 : lr |= 32'b0000_0000_0000_0100

; turn on all interrupt
    cpsie i
    bx lr
    .size port_PendSV_handler, . - port_PendSV_handler
    .endfunc



