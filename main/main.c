/*
 * main.c
 *
 *  Created on: 07 Jul 2026
 *      Author: davejplater@gmail.com
 */

			
#include "buffers.h"
#include "freertos/idf_additions.h"

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
	LEDOFF;
	credit = retrieve_credit();
	if(credit)
	{
		credisp();
	}
	loadincoin();
	dispense(1, 1);
	scrollpos = 0;
	int x = 0;
	uint16_t shcred = credit;
	tstore = esp_timer_get_time();
	uint64_t elapsed;
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
			}
			if(R01IN)
			{
				tstore = esp_timer_get_time(); 
				ones_in();
			}
			x = R10IN | R01IN;
		}
		elapsed = get_elapsed();
		if(elapsed > 500000) //300 mSec
		{
			if(IOPIN(0))
			{
				LEDON;
			}
			else
			{
				LEDOFF;
			}
//			dispense(1, 1);
			if(credit)
			{
				if(shcred != credit)
				{
					credisp();
					shcred = credit;
				}
			}
			else
			{
				lcd_scroll_string(msgbuf);
			}
			tstore = esp_timer_get_time();
		}
    }
}
