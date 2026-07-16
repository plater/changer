/*
 * pay.c
 *
 *  Created on: 13 Jul 2026
 *      Author: davejplater@gmail.com
 */
 
 #include "buffers.h"
#include "nvs.h"
 
 #define MIN_WIDTH (1000 * 10)//Minimum allowable pulse width 10mS
 #define MAX_WIDTH (1000 * 1000)//Maximum allowable pulse width one second

 uint16_t payed;
 nvs_handle_t crhandle;

uint16_t dispense(uint8_t numb, uint8_t value)
{
	
	return payed;
}

void tens_in(void)
{
	int64_t x;
	uint64_t elapsed;
	timer = esp_timer_get_time();
	elapsed = timer - tstore;
	r10add = (~(gpio_get_level(R10_in)) & 1);
	if(!r10add)
	{
		if(elapsed <= MIN_WIDTH)
		{
			goto endof;
		}
	}
	r10add = (~(gpio_get_level(R10_in)) & 1);
	while(r10add)
	{
		timer = esp_timer_get_time();
		elapsed = timer - tstore;
		if(elapsed > MAX_WIDTH)
		{
			strcpy(errormsg, "Note Reader error");
			error_msg(errormsg);
			elapsed = 0;
		}
		timer = esp_timer_get_time();
		elapsed = timer - tstore;
		x = elapsed % (1000 * 500);
		if(!x)
		{
			lcd_scroll_string(msgbuf);
		}
		r10add = (~(gpio_get_level(R10_in)) & 1);
	}
	credit = retrieve_credit();
	credit = credit + 10;
	store_credit(credit);
	endof:
	gpio_intr_enable(R10_in);
}

void ones_in(void)
{
	int64_t x;
	uint64_t elapsed;
	timer = esp_timer_get_time();
	elapsed = timer - tstore;
	r1add = (~(gpio_get_level(R1_in)) & 1);
	if(!r1add)
	{
		if(elapsed <= MIN_WIDTH)
		{
			goto endof2;
		}
	}
	r1add = (~(gpio_get_level(R1_in)) & 1);
	while(r1add)
	{
		timer = esp_timer_get_time();
		elapsed = timer - tstore;
		if(elapsed > MAX_WIDTH)
		{
			strcpy(errormsg, "Coin Mech error");
			error_msg(errormsg);
			elapsed = 0;
		}
		timer = esp_timer_get_time();
		elapsed = timer - tstore;
		x = elapsed % (1000 * 500);
		if(!x)
		{
			lcd_scroll_string(msgbuf);
		}
		r1add = (~(gpio_get_level(R1_in)) & 1);
	}
	credit = retrieve_credit();
	credit++;
	store_credit(credit);
	endof2:
	gpio_intr_enable(R1_in);
}

uint16_t retrieve_credit(void)
{
	uint16_t x;
	ESP_ERROR_CHECK(nvs_open("money", NVS_READONLY, &crhandle));
	ESP_ERROR_CHECK(nvs_get_u16(crhandle, "cash", &x));
	nvs_close(crhandle);	
	return x;
}

void store_credit(uint16_t x)
{
	ESP_ERROR_CHECK(nvs_open("money", NVS_READWRITE, &crhandle));
	ESP_ERROR_CHECK(nvs_set_u16(crhandle, "cash", x));
	ESP_ERROR_CHECK(nvs_commit(crhandle));
	nvs_close(crhandle);	
}

 
