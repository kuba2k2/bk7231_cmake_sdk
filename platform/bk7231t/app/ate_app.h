#ifndef _ATE_APP_H_
#define _ATE_APP_H_

#include "sys_config.h"

#define ATE_APP_FUN                 CFG_ENABLE_ATE_FEATURE

#if ATE_APP_FUN
#include "gpio_pub.h"

#include "uart_pub.h"
#define ATE_DEBUG
#ifdef ATE_DEBUG
#define ATE_PRT      os_printf
#define ATE_WARN     warning_prf
#define ATE_FATAL    fatal_prf
#else
#define ATE_PRT      null_prf
#define ATE_WARN     null_prf
#define ATE_FATAL    null_prf
#endif

#define ATE_GPIO_ID            GPIO11   /* uart1 tx pin*/

void ate_gpio_init(void);
uint32_t ate_mode_check(void);

void ate_app_init(void);
uint32_t get_ate_mode_state(void);
void ate_start(void);

#endif /*ATE_APP_FUN */
#endif // _ATE_APP_H_
// eof

