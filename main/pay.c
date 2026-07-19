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
 #define R5speed   (1000 / 3)

 char errormsg[32];
 uint16_t payed;
 nvs_handle_t crhandle;
 uint16_t credit;
 uint8_t  errorflg;
 
uint16_t dispense(uint8_t numb, uint8_t value)
{
	switch(value)
	{
		case 1 : pay_r1(numb);
		break;
		case 2 : pay_r2(numb);
		break;
		case 5 : pay_r5(numb);
		break;
	}
	
	return payed;
}

void pay_r1(uint8_t numb)
{
	tstore = esp_timer_get_time();
	gpio_set_level(HPR1, 1);
	while(!R1SEN)
	{
		vTaskDelay(1);
		if(get_elapsed() > (3000 * 1000))// Allow 3 seconds for the first coin
		{
			errorflg = 1;
			gpio_set_level(HPR1, 0);
		}
	}
	tstore = esp_timer_get_time();
	dly_msec(5);
	while(R1SEN)// Coin on it's way out
	{
		if(get_elapsed() > (500 * 1000))// Allow 3 seconds for the first coin
		{
			errorflg = 1;
			gpio_set_level(HPR1, 0);
		}
	}
}

void pay_r2(uint8_t numb)
{
	gpio_set_level(HPR2, 1);
	while(!R2SEN)
	{
		
	}
}

void pay_r5(uint8_t numb)
{
	gpio_set_level(HPR5, 1);
	while(!R5SEN)
	{
		
	}
}

void tens_in(void)
{
	dly_msec(5);//Wait for 5mS debounce
	if(!R10IN)
	{
		dly_msec(1);
		if(!R10IN)
		{
			goto endof; //Noise? pulse is less than 6mS
		}
	}
	while(R10IN)
	{
		dly_msec(1);
	}
	credit = retrieve_credit();
	credit = credit + 10;
	store_credit(credit);
	endof:
}

void ones_in(void)
{
		dly_msec(5);//Wait for 5mS debounce
		if(!R01IN)
		{
			dly_msec(1);
			if(!R01IN)
			{
				goto endof; //Noise? pulse is less than 6mS
			}
		}
		while(R01IN)
		{
			dly_msec(1);
		}
		credit = retrieve_credit();
		credit++;
		store_credit(credit);
	endof:
}

uint64_t get_elapsed(void)
{
	uint64_t lapsed;
	timer = esp_timer_get_time();
	lapsed = timer - tstore;
	return lapsed;
}

void dly_msec(uint16_t msecs)
{
	vTaskDelay(pdMS_TO_TICKS(msecs));
}

uint16_t retrieve_credit(void)
{
	esp_err_t err;
	uint16_t x;
	err = nvs_open("money", NVS_READONLY, &crhandle);
	if(err == ESP_ERR_NVS_NOT_FOUND)
	{
		store_credit(0);
		return 0;
	}
	else
	{
		ESP_ERROR_CHECK(err);
	} 
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

 
