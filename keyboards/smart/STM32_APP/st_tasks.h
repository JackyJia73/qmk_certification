#ifndef __ST_TASKS_H_
#define __ST_TASKS_H_


typedef enum{
	QMK_TRANSMIT_MODE_USB,
	QMK_TRANSMIT_MODE_BLE,
	QMK_TRANSMIT_MODE_MAC
} QMK_TRAN_MODE_t;

void st_tasks(void);

#endif /* __ST_TASKS_H_ */
