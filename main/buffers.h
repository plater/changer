/*
 * buffers.h
 *
 *  Created on: 07 July 2026
 *      Author: davejplater@gmail.com
 */
#pragma once

#ifndef MAIN_BUFFERS_H_
#define MAIN_BUFFERS_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "driver/gpio.h"
#include "driver/gpio_filter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "hal/gpio_types.h"
#include "esp_log.h"
#include <i2cdev.h>
#include "pcf8574.h"
#include "hd44780.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/idf_additions.h"

enum { 	PAY_OK = 0,
		HOPPER_EMT = 1,
		HOPPER_JAM  = 2,
	};

#define DLY1SEC()   		vTaskDelay(1000 / portTICK_PERIOD_MS)
#define DLYHSEC()   		vTaskDelay(500 / portTICK_PERIOD_MS)
#define DLYQSEC()   		vTaskDelay(250 / portTICK_PERIOD_MS)
#define DLYMSEC()			vTaskDelay(1 / portTICK_PERIOD_MS)
#define DLY5MSEC()			vTaskDelay(5 / portTICK_PERIOD_MS)
#define DLYTMSEC()			vTaskDelay(10 / portTICK_PERIOD_MS)

/* GPIO Defines */
#define R5sense		GPIO_NUM_0
#define R2sense		GPIO_NUM_1
#define HPR5_FWD	GPIO_NUM_2
#define R1sense		GPIO_NUM_3
#define SCL			GPIO_NUM_4
#define SDA			GPIO_NUM_5
#define HPR1		GPIO_NUM_6
#define HPR2		GPIO_NUM_7
#define HPR5_REV	GPIO_NUM_8
#define ENOTE		GPIO_NUM_9
#define HPR5		GPIO_NUM_10
#define R1_in		GPIO_NUM_20
#define R10_in		GPIO_NUM_21

#define BUTR2   	!(get_pin_level(3))
#define BUTR5   	!(get_pin_level(4))
#define SERVICE 	!(get_pin_level(5))
#define IOIN    	get_io()
#define IOPIN(pin) 	get_pin_level(pin)
#define COINEN		set_pin_level(6, 0)
#define COINDS		set_pin_level(6, 1)
#define NOTEEN		set_pin_level(7, 0)
#define NOTEDS		set_pin_level(7, 1)

#define LEDON	ESP_ERROR_CHECK(gpio_set_level(HPR5_REV, 0))
#define LEDOFF	ESP_ERROR_CHECK(gpio_set_level(HPR5_REV, 1))
#define R10IN   (~(gpio_get_level(R10_in)) & 1)
#define R01IN	(~(gpio_get_level(R1_in)) & 1)
#define R1SEN   (~(gpio_get_level(R1sense)) & 1)
#define R2SEN   (~(gpio_get_level(R2sense)) & 1)
#define R5SEN   (~(gpio_get_level(R5sense)) & 1)
#define R1IN    ESP_LOGI("main", "R1sense = %d", x)
#define R2IN    ESP_LOGI("main", "R2sense = %d", x)
#define R5IN    ESP_LOGI("main", "R5sense = %d", x)
#define R1OFF	gpio_set_level(HPR1, 0)
#define R1ON	gpio_set_level(HPR1, 1)
#define R2OFF	gpio_set_level(HPR2, 0)
#define R2ON	gpio_set_level(HPR2, 1)
#define R5OFF	gpio_set_level(HPR5, 0)
#define R5ON	gpio_set_level(HPR5, 1)

/* Memory allocate */
extern uint16_t credit;
extern uint8_t  errorflg;
extern uint64_t timer;
extern uint64_t tstore;
extern double tnow;
extern double tpsw;
extern double tstr;
extern uint8_t r10add;
extern uint8_t r1add;
extern i2c_dev_t pcf_gpio_dev;
extern i2c_dev_t pcf_lcd_dev;
extern hd44780_t lcd_dev;
extern char msgbuf[128];
extern char msg16[17];
extern uint8_t scrollpos;
extern size_t msgsize;
extern int* parg;
extern gpio_isr_handle_t gpint;
extern char errormsg[32];
/* Function defines */
void init_gpio(void);

void i2c_init(void);
void dsp_init(void);
void lcd_write_string(const char *str);
void lcd_scroll_string(char *str);
void error_msg(char* errtype);
uint16_t retrieve_credit();
void store_credit(uint16_t x);
void credisp(void);
void loadincoin(void);
void call_joe(int broken, int what);

void tens_in(void);
void ones_in(void);
void dly_msec(uint16_t msecs);
uint64_t get_elapsed(void);
uint8_t get_io(void);
bool get_pin_level(uint8_t pin);
void set_io(uint8_t thedata);
void set_pin_level(uint8_t pin, uint32_t value);
uint8_t pay_r1(uint8_t numb);
uint8_t pay_r2(uint8_t numb);
uint8_t pay_r5(uint8_t numb);
void process_credit(void);
void dispense_r1(void);
void dispense_r2(void);
void dispense_r5(void);
void  disable_cn(void);



#endif /* MAIN_BUFFERS_H_ */
