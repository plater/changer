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
 float crdhod, crdhod0, crdhod1;
 uint32_t numbs; 
 uint32_t crumbs;
 
void dispense_r1(void)//crdhod, crdhod0, crdhod1;
{
	credit = retrieve_credit();
	numbs = credit;
	uint8_t y = pay_r1(numbs);
	switch(y)
	{
		case HOPPER_EMT :	call_joe(HOPPER_EMT, '1');
		break;
		case HOPPER_JAM :	call_joe(HOPPER_JAM, '1');
		break;
		case PAY_OK   	:
		break;
		default         :
	}
}

uint8_t pay_r1(uint8_t numb)
{
	nextcoin1:
		tstore = esp_timer_get_time();
		ESP_LOGI("payr1", "tstore = %llu", tstore);
		R1ON;
		while(!R1SEN)
		{
			vTaskDelay(1);
			if(get_elapsed() > (5000 * 1000))// Allow 5 seconds for the first coin
			{
				R1OFF;
				ESP_LOGI("payr1", "hopper timeout");
				return HOPPER_EMT;
			}
		}
		tstore = esp_timer_get_time();
		dly_msec(1);
		while(R1SEN)// Coin on it's way out
		{
			dly_msec(1);
			if(get_elapsed() > (500 * 1000))// Allow 500ms for the coin exit
			{
				R1OFF;
				ESP_LOGI("pay", "hopper jam");
				return HOPPER_JAM;
			}
		}
		tstore = esp_timer_get_time();
		--numb;
		if(numb <= 0)
		{
			credit = retrieve_credit();
			credit--;
			store_credit(credit);
			dly_msec(1);
			R1OFF;
		}
		else
		{
			credit = retrieve_credit();
			credit--;
			store_credit(credit);
			goto nextcoin1;
		}
	return PAY_OK;
}

void dispense_r2(void)//crdhod, crdhod0, crdhod1;
{
	credit = retrieve_credit();
	numbs = credit / 2;
	if(numbs > 5)
	{
		numbs = 5;
	}
	uint8_t y = pay_r2(numbs);
	switch(y)
	{
		case HOPPER_EMT :	call_joe(HOPPER_EMT, '2');
		goto error;
		case HOPPER_JAM :	call_joe(HOPPER_JAM, '2');
		goto error;
		case PAY_OK   	:
		break;
		default         :
	}
	credit = retrieve_credit();
	if(credit >= 5)
	{
		dispense_r5();
	}
	credit = retrieve_credit();
	if(credit > 0)
	{
		dispense_r1();
	}
	error:
}

uint8_t pay_r2(uint8_t numb)
{
	uint64_t elapsed;
	nextcoin2:
		tstore = esp_timer_get_time();
		ESP_LOGI("payr2", "tstore = %llu", tstore);
		R2ON;
		while(!R2SEN)
		{
			vTaskDelay(1);
			if(get_elapsed() > (5000 * 1000))// Allow 5 seconds for the first coin
			{
				R2OFF;
				ESP_LOGI("pay", "hopper timeout");
				return HOPPER_EMT;
			}
		}
		tstore = esp_timer_get_time();
		dly_msec(1);
		while(R2SEN)// Coin on it's way out
		{
			dly_msec(1);
			if(get_elapsed() > (500 * 1000))// Allow 500ms for the coin exit
			{
				R2OFF;
				ESP_LOGI("pay", "hopper jam");
				return HOPPER_JAM;
			}
		}
		tstore = esp_timer_get_time();
		--numb;
		credit = retrieve_credit();
//		ESP_LOGI("payr2", "numb = %d", numb);
		if(numb > 0)
		{
			credit = credit - 2;
			store_credit(credit);
			elapsed = get_elapsed();
			ESP_LOGI("payr2", "elapsed = %llu", elapsed);
			goto nextcoin2;
		}
		else
		{
			dly_msec(1);
			R2OFF;
			credit = credit - 2;
			store_credit(credit);
		}
	return PAY_OK;
}

void dispense_r5(void)//crdhod, crdhod0, crdhod1;
{
	credit = retrieve_credit();
	numbs = credit / 5;
	if(numbs > 8)
	{
		crumbs = numbs % 8;
		numbs  = numbs - crumbs;
	}
	uint8_t y = pay_r5(numbs);
	switch(y)
	{
		case HOPPER_EMT :	call_joe(HOPPER_EMT, '5');
		break;
		case HOPPER_JAM :	call_joe(HOPPER_JAM, '5');
		break;
		case PAY_OK   	:
		break;
		default         :
	}
}

uint8_t pay_r5(uint8_t numb)
{
		uint64_t elapsed;
		nextcoin5:
			tstore = esp_timer_get_time();
			ESP_LOGI("payr2", "tstore = %llu", tstore);
			R5ON;
			while(!R5SEN)
			{
				vTaskDelay(1);
				if(get_elapsed() > (5000 * 1000))// Allow 5 seconds for the first coin
				{
					R5OFF;
					ESP_LOGI("pay", "hopper timeout");
					return HOPPER_EMT;
				}
			}
			tstore = esp_timer_get_time();
			dly_msec(1);
			while(R5SEN)// Coin on it's way out
			{
				dly_msec(1);
				if(get_elapsed() > (500 * 1000))// Allow 500ms for the coin exit
				{
					R5OFF;
					ESP_LOGI("pay", "hopper jam");
					return HOPPER_JAM;
				}
			}
			tstore = esp_timer_get_time();
			--numb;
	//		ESP_LOGI("payr2", "numb = %d", numb);
			credit = retrieve_credit();
			if(numb > 0)
			{
				credit = credit - 5;
				store_credit(credit);
				elapsed = get_elapsed();
				ESP_LOGI("payr5", "elapsed = %llu", elapsed);
				goto nextcoin5;
			}
			else
			{
				dly_msec(1);
				R5OFF;
				credit = credit - 5;
				store_credit(credit);
			}
		return PAY_OK;
}

void tens_in(void)
{
	dly_msec(5);//Wait for 5mS debounce
	if(R10IN)
	{
		dly_msec(1);
		if(!R10IN)
		{
			goto endof; //Noise? pulse is less than 6mS
		}
	}
	dly_msec(10);
	
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
		dly_msec(10);
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
void deduct_credit(uint16_t minus)
{
	credit = retrieve_credit();
	credit = credit - minus;
	store_credit(credit);
}

void store_credit(uint16_t x)
{
	ESP_ERROR_CHECK(nvs_open("money", NVS_READWRITE, &crhandle));
	ESP_ERROR_CHECK(nvs_set_u16(crhandle, "cash", x));
	ESP_ERROR_CHECK(nvs_commit(crhandle));
	nvs_close(crhandle);	
}

void process_credit(void)
{
	if(BUTR2)
	{
		if(credit >= 2)
		{
			dispense_r2();
		}
		
	}
	if(credit >= 5)
	{
		if(BUTR5)
		{
			if(credit > 41)
			{
				dispense_r2();
			}
			if(credit >= 5)
			{
				dispense_r5();
			}
		}
	}
}



