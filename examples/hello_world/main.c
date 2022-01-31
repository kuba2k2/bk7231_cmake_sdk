#include "arm_arch.h"
#include "flash.h"
#include "include.h"
#include "rtos_pub.h"
#include "uart_pub.h"

// copied from flash.c
static UINT32 flash_get_id(void) {
	UINT32 value;

	value = (FLASH_OPCODE_RDID << OP_TYPE_SW_POSI) | OP_SW | WP_VALUE;
	REG_WRITE(REG_FLASH_OPERATE_SW, value);

	while (REG_READ(REG_FLASH_OPERATE_SW) & BUSY_SW)
		;

	UINT32 flash_id = REG_READ(REG_FLASH_RDID_DATA_FLASH);
	return flash_id;
}

void main_thread(beken_thread_arg_t arg) {
	bk_printf("main thread running\r\n");
	int num = 0;
	while (1) {
		bk_printf("Message %d\r\n", num++);
		rtos_delay_milliseconds(1000);
	}
}

void user_main(void) {
	bk_printf("Hello world!\r\n");
	UINT32 id = flash_get_id();
	bk_printf("Flash ID: 0x%x\r\n", id);
	int err = rtos_create_thread(
		NULL,
		BEKEN_APPLICATION_PRIORITY,
		"main",
		(beken_thread_function_t)main_thread,
		1024,
		(beken_thread_arg_t)0);
	bk_printf("rtos_create_thread err %d\r\n", err);
}
