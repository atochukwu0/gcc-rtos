#ifndef OS_CONFIG_H
#define OS_CONFIG_H
/*Application specific definitions header*/
#include "user_config.h"
// unit: MHz
#define CONFIG_SYSCLK 72
// unit: us
#define CONFIG_OS_SYSCLK_DIV 8
#define CONFIG_OS_SYSTICK_CLK (CONFIG_SYSCLK / CONFIG_OS_SYSCLK_DIV)
#define CONFIG_OS_TICK_TIME_US 10000

#endif //OS_CONFIG_H
