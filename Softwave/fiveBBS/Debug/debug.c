#include "debug.h"
//ELOG_FMT_LVL：日志级别（如 [ASSERT]、[ERROR]、[DEBUG]）。
//ELOG_FMT_TAG：日志标签（通常是模块名，如 "UART"、"ADC"），用于区分日志来源。
//ELOG_FMT_TIME：日志输出时间戳（需提前配置系统时间或定时器计时）。
//ELOG_FMT_FUNC：输出日志所在的函数名（如 uart_init），便于定位代码位置。
//ELOG_FMT_T_INFO：线程信息（RTOS 环境下，如线程 ID、线程名）。
//ELOG_FMT_P_INFO：进程信息（多进程环境下，嵌入式场景较少用到）。
void Debug_Init(void)
{
	/* initialize EasyLogger */
	elog_init();
	/* set EasyLogger log format */
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
	/* start EasyLogger */
	elog_start();
}
