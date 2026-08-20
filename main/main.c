/*
 * main.c
 *
 *  Created on: 07 Jul 2026
 *      Author: davejplater@gmail.com
 */

			
#include "buffers.h"

uint64_t timer;
uint64_t tstore;
uint64_t elapsed;

void app_main(void)
{
	init_gpio();
	i2c_init() ;
	hd44780_clear(&lcd_dev);
	lcd_write_string("0123456789ABCDEF");
	ESP_ERROR_CHECK(nvs_flash_init());
 R1ON;
 delay_
	if(retrieve_error() == 1)
	{
		call_joe(HOPPER_EMT, 6);
	}
	LEDOFF;
	if(SERVICE)
	{
		LEDON;
		ESP_LOGI("ccred", "about to clear credit");
		store_credit(0);
		ESP_LOGI("ccred", "credit =%d", credit);
	}
	while(SERVICE){}
	LEDOFF;
	credit = retrieve_credit();
	if(credit)
	{
		credisp();
	}
	loadincoin();
	scrollpos = 0;
	int x = 0;
	uint16_t shcred = credit;
	
	uint64_t elapsed;
	uint64_t tstore2;
	uint64_t tstore4;
	uint8_t timeout = 0;
	tstore = esp_timer_get_time();
	tstore2 = tstore;
    while (true) 
	{
		vTaskDelay(1);
		x = R10IN | R01IN;
		while(x)
		{
			ESP_LOGI("cash_in", "cash input, R10 = %d and R1 = %d", R10IN, R01IN);
			if(R10IN)
			{
				tstore = esp_timer_get_time();
				tens_in();
				x = 10;
			}
			if(R01IN)
			{
				tstore = esp_timer_get_time(); 
				ones_in();
				x = 1;
			}
			x = R10IN | R01IN;
		}
		tstore = tstore2;
		if(credit > 10)
		{
			NOTEDS;
			COINDS;
		}
		elapsed = get_elapsed();
		if(elapsed > 500000) //500 mSec
		{
			credit = retrieve_credit();
			if(credit)
			{
				if(shcred != credit)
				{
					credisp();
					shcred = credit;
				}
				if(credit)
				{
					process_credit();
				}
			}
			else
			{
				NOTEEN;
				COINEN;
				lcd_scroll_string(msgbuf);
			}
			tstore = esp_timer_get_time();
			tstore2 = tstore;
		}
    }
}
