/*
 * buffers.h
 *
 *  Created on: 07 Jul 2026
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
//#include "interleaved_pwm.h"
//#include "messages.h"

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

#define BUTR2   IOEXP0
#define BUTR5   IOEXP1
#define SERVICE IOEXP2

#define LEDON	ESP_ERROR_CHECK(gpio_set_level(HPR5_REV, 0))
#define LEDOFF	ESP_ERROR_CHECK(gpio_set_level(HPR5_REV, 1))
#define R1IN    ESP_LOGI("main", "R1sense = %d", x)
#define R2IN    ESP_LOGI("main", "R2sense = %d", x)
#define R5IN    ESP_LOGI("main", "R5sense = %d", x)

/* Memory allocate */
extern uint16_t credit;
extern volatile uint64_t timer;
extern volatile uint64_t tstore;
extern volatile uint8_t r10add;
extern volatile uint8_t r1add;
extern i2c_dev_t pcf_gpio_dev;
extern i2c_dev_t pcf_lcd_dev;
extern hd44780_t lcd_dev;
extern char msgbuf[128];
extern char msg16[17];
extern uint8_t scrollpos;
extern size_t msgsize;
extern int* parg;
extern gpio_isr_handle_t gpint;
char errormsg[32];
/* Function defines */
void init_gpio(void);
void i2c_init(void);
void dsp_init(void);
void lcd_write_string(const char *str);
void lcd_scroll_string(char *str);
uint16_t dispense(uint8_t numb, uint8_t value);
static void r10_isr_handler(void* arg);
static void r1_isr_handler(void* arg);
void tens_in(void);
void ones_in(void);
void error_msg(char* errtype);
uint16_t retrieve_credit();
void store_credit(uint16_t x);
void credisp(void);


#endif /* MAIN_BUFFERS_H_ */
