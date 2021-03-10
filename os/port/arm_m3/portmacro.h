/*
    Each @$FWNAME port has a unique portmacro.h header file.
*/
#ifndef PORTMACRO_H
#define PORTMACRO_H
#define port_yield()                                    \
{                                                       \
    /* Set a PendSV to request a context switch. */     \
    port_NVIC_INT_CTRL_REG = port_NVIC_PENDSVSET_BIT;   \
                                                        \
    /* Barriers are normally not required but do ensure the code is completely \
    * within the specified behaviour for the architecture. */  \
    __asm volatile ( "dsb" ::: "memory" );                     \
    __asm volatile ( "isb" );                                  \
}
#define port_NVIC_INT_CTRL_REG     ( *( ( volatile uint32_t * ) 0xe000ed04 ) )
#define port_NVIC_PENDSVSET_BIT    ( 1UL << 28UL )


#endif
