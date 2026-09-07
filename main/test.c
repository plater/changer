/*
 *test.c
 *
 *  Created on: Sat 05 Sep 2026 11:38:24 SAST

 *      Author: davejplater@gmail.com
 */
 
#include "buffers.h"

uint8_t empty_r1(void)
{
	volatile int x;
	uint64_t elapsed;
	uint16_t timeout;
	uint8_t count = 0;
	lcd_write_string("Empty R1 hopper");
	while(!BUTR2)
	{
		vTaskDelay(1);
	}
	timeout = 0;
	R1ON;
	dly_msec(3);
	R1BEG:
	tstore = esp_timer_get_time();
	do
	{
		x = gpio_get_level(R1sense);
		vTaskDelay(1);
		timeout = get_elapsedm();
		if(timeout >= 5000)
		{
			R1OFF;
			goto R1FIN;
		}
	}while(x == 1);
		
		count++;
	ESP_LOGI("testR1", "count = %d time to start exit = %llu", count, elapsed);
	tstore = esp_timer_get_time();
	do
	{
		vTaskDelay(1);
		x = gpio_get_level(R1sense);
	}while(x == 0);
	elapsed = get_elapsed();
	ESP_LOGI("testR1", "count = %d time to finish exit = %llu", count, elapsed);
	sprintf(msgbuf, "dispensed %d coins", count);
	lcd_write_string(msgbuf);
	goto R1BEG;
	R1FIN:
	R1OFF;
	ESP_LOGI("R1", "End");
	return count;
}

uint8_t empty_r2(void)
{
	volatile int x;
	uint64_t elapsed;
	uint16_t timeout;
	uint8_t count = 0;
	lcd_write_string("Empty R2 hopper");
	while(!BUTR2)
	{
		vTaskDelay(1);
	}
	timeout = 0;
	R2ON;
	dly_msec(3);
	R2BEG:
	tstore = esp_timer_get_time();
	do
	{
		x = gpio_get_level(R2sense);
		vTaskDelay(1);
		timeout = get_elapsedm();
		if(timeout >= 5000)
		{
			R2OFF;
			goto R2FIN;
		}
	}while(x == 1);
		
		count++;
	ESP_LOGI("testR2", "count = %d time to start exit = %llu", count, elapsed);
	tstore = esp_timer_get_time();
	do
	{
		vTaskDelay(1);
		x = gpio_get_level(R2sense);
	}while(x == 0);
	elapsed = get_elapsed();
	ESP_LOGI("testR2", "count = %d time to finish exit = %llu", count, elapsed);
	sprintf(msgbuf, "dispensed %d coins", count);
	lcd_write_string(msgbuf);
	goto R2BEG;
	R2FIN:
	R2OFF;
	ESP_LOGI("R2", "End");
	return count;
}

uint8_t empty_r5(void)
{
	volatile int x;
	uint64_t elapsed;
	uint16_t timeout;
	uint8_t count = 0;
	lcd_write_string("Empty R5 hopper");
	while(!BUTR2)
	{
		vTaskDelay(1);
	}
	timeout = 0;
	R5ON;
	dly_msec(3);
	R5BEG:
	tstore = esp_timer_get_time();
	do
	{
		x = gpio_get_level(R5sense);
		vTaskDelay(1);
		timeout = get_elapsedm();
		if(timeout >= 5000)
		{
			R5OFF;
			goto R5FIN;
		}
	}while(x == 1);
		
		count++;
	ESP_LOGI("testR5", "count = %d time to start exit = %llu", count, elapsed);
	tstore = esp_timer_get_time();
	do
	{
		vTaskDelay(1);
		x = gpio_get_level(R5sense);
	}while(x == 0);
	elapsed = get_elapsed();
	ESP_LOGI("testR5", "count = %d time to finish exit = %llu", count, elapsed);
	sprintf(msgbuf, "dispensed %d coins", count);
	lcd_write_string(msgbuf);
	goto R5BEG;
	R5FIN:
	R5OFF;
	ESP_LOGI("R5", "End");
	return count;
}




