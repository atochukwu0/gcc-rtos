#include <task.h>
#include <config.h>
#include <os.h>
#include <stdint.h>

Task_Control_Block_t tcb_list[CONFIG_MAX_TASK_NUM];

//int max_task_num = CONFIG_MAX_TASK_NUM + 1;
// for skipping the first task (task_idle), set current_task_id to 1
int current_task_id = 0;
int next_task_id = 0;
int is_first_switch_task = 1;

uint32_t now_tick = 0;

struct OS_STRUCT{
	int current_task_id;
	int next_task_id;
	int is_first_switch_task;
	Task_Control_Block_t *current_TCB;
} os_data;

#define STACK_IDLE_SIZE 32
stack_t stack_idle[STACK_IDLE_SIZE];
void init_task() {
	create_task(task_idle, 0, stack_idle, STACK_IDLE_SIZE);
	os_data.current_TCB = &tcb_list[0];
	__asm volatile(
		// PSP = 0
		"msr psp, #0\n"
	);
}

void switch_current_TCB() {
    if (current_task_id >= next_task_id - 1) {
		// check if not create any task, switch to idle task
		if (next_task_id >= 2) {
			current_task_id = 1;
		} else {
			current_task_id = 0;
			os_data.current_TCB = &tcb_list[0];
			return;
		}
    } else {
        ++current_task_id;
    }
	// check if a task is in delay
	// if all tasks are in waiting for delay
	os_data.current_TCB = &tcb_list[0];
	for (int i = current_task_id; i < next_task_id; ++i) {
		Task_Control_Block_t *checking_TCB = &tcb_list[i];
		// BUG!! now ticks is not update
		if (checking_TCB->delay_ticks == 0 || checking_TCB->delay_ticks <= now_tick) {
			checking_TCB->delay_ticks = 0;
			os_data.current_TCB = checking_TCB;
			current_task_id = i;
			return;
		}				
	}
}


int create_task(void *function, void *arguements, stack_t *stack, int stack_size) {
    if (next_task_id > CONFIG_MAX_TASK_NUM) {
        return -1;
    }

    stack_t *stack_top = &stack[stack_size];
    // auto save by Cortex-M3
    *(--stack_top) = (stack_t)0x01000000u; // xPSR bit 24 = 1
    *(--stack_top) = (stack_t)function; // R15 PC function entry point
    *(--stack_top) = (stack_t)0x14141414u; // R14 LR
    *(--stack_top) = (stack_t)0x12121212u; // R12
    *(--stack_top) = (stack_t)0x03030303u; // R3
    *(--stack_top) = (stack_t)0x02020202u; // R2
    *(--stack_top) = (stack_t)0x01010101u; // R1
    *(--stack_top) = (stack_t)arguements; // R0
    // manual save by developer
    *(--stack_top) = (stack_t)0x11111111u; // R11
    *(--stack_top) = (stack_t)0x10101010u; // R10
    *(--stack_top) = (stack_t)0x09090909u; // R9
    *(--stack_top) = (stack_t)0x08080808u; // R8
    *(--stack_top) = (stack_t)0x07070707u; // R7
    *(--stack_top) = (stack_t)0x06060606u; // R6
    *(--stack_top) = (stack_t)0x05050505u; // R5
    *(--stack_top) = (stack_t)0x04040404u; // R4
    tcb_list[next_task_id].stack = stack_top;

    return next_task_id++;
}
/*
void switch_task() {
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}*/
void SysTick_Handler(void) {
	++now_tick;
	port_NVIC_INT_CTRL_REG = port_NVIC_PENDSVSET_BIT;
}


void delay(uint32_t us) {
    uint32_t delay_ticks = us / CONFIG_OS_TICK_TIME_US;
	uint32_t target_tick = now_tick + delay_ticks;
    os_data.current_TCB->delay_ticks = target_tick;
	switch_task();
}

uint32_t idle_ticks = 0;
void task_idle() {
	for (;;) {
		++idle_ticks;
	}
}
