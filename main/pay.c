/*
 * pay.c
 *
 *  Created on: 13 Jul 2026
 *      Author: davejplater@gmail.com
 */
 
 #include "buffers.h"
#include "nvs.h"
#include <stdlib.h>
 
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
	ESP_LOGI("disR1", "credit = %d numbs = %d ", credit, numbs);
	sprintf(msgbuf, "Dispense R1 x %d", (uint16_t)numbs);
	lcd_write_string(msgbuf);
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
	ESP_LOGI("R1PAY_OK", "Returning");
}

uint8_t pay_r1(uint8_t numb)
{
#ifdef COMHOP
		R1ON;
		dly_msec(2);//Wait for valid signal after PU
		nextcoin1:
  R1ON;
		tstore = esp_timer_get_time();
	
		dly_msec(2);
		while(!R1cSEN)
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
  R1OFF;
		dly_msec(2);
		while(R1cSEN)// Coin on it's way out
		{
			dly_msec(3);
			if(get_elapsed() > (500 * 1000))// Allow 500ms for the coin exit
			{
				R1OFF;
				ESP_LOGI("pay", "hopper jam");
				return HOPPER_JAM;
			}
		}
		tstore = esp_timer_get_time();
		--numb;
		ESP_LOGI("payr1", "numb = %d credit - 1 = %d", numb, (credit--));
		if(numb == 0)
		{
			R1OFF;
			credit = retrieve_credit();
			credit--;
			store_credit(credit);
			dly_msec(20);
		}
		else
		{
			credit = retrieve_credit();
			credit--;
			store_credit(credit);
			goto nextcoin1;
		}
#else
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
dly_msec(10);
while(R1SEN)// Coin on it's way out
{
	dly_msec(10);
	if(get_elapsed() > (500 * 1000))// Allow 500ms for the coin exit
	{
		R1OFF;
		ESP_LOGI("pay", "hopper jam");
		return HOPPER_JAM;
	}
}
tstore = esp_timer_get_time();
--numb;
ESP_LOGI("payr1", "numb = %d credit - 1 = %d", numb, (credit--));
if(numb == 0)
{
	credit = retrieve_credit();
	credit--;
	store_credit(credit);
	dly_msec(20);
	R1OFF;
}
else
{
	credit = retrieve_credit();
	credit--;
	store_credit(credit);
	goto nextcoin1;
}
#endif
	ESP_LOGI("payR1 Ok", "credit = %d numbs = %d", credit, numbs);
	return PAY_OK;
}

void dispense_r2(void)//crdhod, crdhod0, crdhod1;
{
	credit = retrieve_credit();
	numbs = credit / 2;
	payed = credit % 2;
	ESP_LOGI("disR2", "credit = %d numbs = %d remainder = %d", credit, numbs, payed);
	if(numbs > 5)
	{
		numbs = 5;
	}
	sprintf(msgbuf, "Dispense R2 x %d", (uint16_t)numbs);
	lcd_write_string(msgbuf);
	uint8_t y = pay_r2(numbs);
	switch(y)
	{
		case HOPPER_EMT :	call_joe(HOPPER_EMT, '2');
		goto error;
		case HOPPER_JAM :	call_joe(HOPPER_JAM, '2');
		goto error;
		case PAY_OK   	:
		default         :
	}
	credit = retrieve_credit();
	if(credit >= 10)
	{
		dispense_r5();
	}
	credit = retrieve_credit();
	if(credit > 0)
	{
		dispense_r1();
	}
	ESP_LOGI("R2PAY_OK", "Returning");
		error:
}

uint8_t pay_r2(uint8_t numb)
{
	uint64_t elapsed;
	nextcoin2:
		tstore = esp_timer_get_time();
		ESP_LOGI("1payr2", "tstore = %llu numbs = %d", tstore, numbs);
		R2ON;
		while(!R2SEN)
		{
			vTaskDelay(1);
			if(get_elapsed() > (5000 * 1000))// Allow 5 seconds for the first coin
			{
				R2OFF;
				ESP_LOGI("pay", "hopper timeout");
				goto empty;
				return HOPPER_EMT;
			}
		}
		R2OFF;
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
		ESP_LOGI("2payr2", "numb = %d credit - 2 = %d", numb, (credit - 2));
		if(numb > 0)
		{
			credit = credit - 2;
			store_credit(credit);
			elapsed = get_elapsed();
			ESP_LOGI("payr2", "elapsed = %llu", elapsed);
			dly_msec(150);
			goto nextcoin2;
		}
		else
		{
			dly_msec(1);
			R2OFF;
			credit = credit - 2;
			store_credit(credit);
			if(credit == 2)
			{
				goto nextcoin2;
			}
			ESP_LOGI("3payr2", "numb = %d credit - 2 = %d", numb, credit);
			
		}
	ESP_LOGI("payR2 Ok", "credit = %d numbs = %d", credit, numbs);
	return PAY_OK;
empty:
credit = retrieve_credit();
if(credit > 0)
{
	numbs = credit;
	int x = pay_r1(numbs);
	if(x != PAY_OK)
	{
		call_joe(HOPPER_EMT, '6');
	}
}
return PAY_OK;
}

void dispense_r5(void)//crdhod, crdhod0, crdhod1;
{
	credit = retrieve_credit();
	numbs = credit / 5;
	payed = credit % 5;
	ESP_LOGI("disR5", "credit = %d numbs = %d remainder = %d", credit, numbs, payed);
	if(numbs > 8)
	{
		crumbs = numbs % 8;
		numbs  = numbs - crumbs;
	}
	ESP_LOGI("disR5", "credit = %d numbs = %d crumbs = %d", credit, numbs, crumbs);
	sprintf(msgbuf, "Dispense R5 x %d", (uint16_t)numbs);
	lcd_write_string(msgbuf);
	uint8_t y = pay_r5(numbs);
	switch(y)
	{
		case HOPPER_EMT :	call_joe(HOPPER_EMT, '5');
		break;
		case HOPPER_JAM :	call_joe(HOPPER_JAM, '5');
		break;
		case PAY_OK   	:
		default         :
	}
	ESP_LOGI("R5PAY_OK", "Returning");
}

uint8_t pay_r5(uint8_t numb)
{
		uint64_t elapsed;
		nextcoin5:
			tstore = esp_timer_get_time();
			ESP_LOGI("payr2", "tstore = %llu", tstore);
			R5ON;
			dly_msec(10);
			while(!R5SEN)
			{
				vTaskDelay(1);
				if(get_elapsed() > (5000 * 1000))// Allow 5 seconds for the first coin
				{
					R5OFF;
					ESP_LOGI("pay", "hopper timeout");
					goto empty;
					return HOPPER_EMT;
				}
			}
			R5OFF;
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
				dly_msec(100);
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
				R5OFF;
				credit = credit - 5;
				store_credit(credit);
			}
		ESP_LOGI("payR5 Ok", "credit = %d numbs = %d", credit, numbs);
		return PAY_OK;
		empty:
		credit = retrieve_credit();
		if(credit > 1)
		{
			numbs = credit / 2;
			pay_r2(numbs);
		}
		else
		{
			numbs = credit;
			int x = pay_r1(numbs);
			if(x != PAY_OK)
			{
			call_joe(HOPPER_EMT, '6');
			}
		}
		return PAY_OK;
}
		

void tens_in(void)
{
	uint64_t elapsed = 0;
	startof:
	dly_msec(5);//Wait for 5mS debounce
	if(R10IN)
	{
		dly_msec(1);
		if(!R10IN)
		{
			goto endof; //Noise? pulse is less than 1mS
		}
	}
	dly_msec(10);
	
	credit = retrieve_credit();
	credit = credit + 10;
	store_credit(credit);
	tstore = esp_timer_get_time();
	while(R10IN)
	{
		elapsed = get_elapsed();
		
	}
	endof:
	tstore = esp_timer_get_time();
	while(elapsed <= (300 * 1000))//wait for next pulse
	{
		elapsed = get_elapsed();
		if(R10IN)
		{
			goto startof;
		}
	}
}

void ones_in(void)
{
		uint64_t elapsed =  0;
		startof:
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
		tstore = esp_timer_get_time();
		while(R01IN)
		{
			elapsed = get_elapsed();
		}
	endof:
	elapsed = get_elapsed();
	while(elapsed <= (100 * 1000))//wait for next pulse
	{
		if(R01IN)
		{
			goto startof;
		}
		elapsed = get_elapsed();
	}
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

void store_error(int strclr)
{
ESP_ERROR_CHECK(nvs_open("money", NVS_READWRITE, &crhandle));
ESP_ERROR_CHECK(nvs_set_u16(crhandle, "error", strclr));
ESP_ERROR_CHECK(nvs_commit(crhandle));
nvs_close(crhandle);
}

uint16_t retrieve_error(void)
{	
	esp_err_t err;
	uint16_t strclr;
	err = nvs_open("money", NVS_READONLY, &crhandle);
	if(err == ESP_ERR_NVS_NOT_FOUND)
	{
		store_error(0);
		return 0;
	}
	else
	{
		ESP_ERROR_CHECK(err);
	} 
	ESP_ERROR_CHECK(nvs_get_u16(crhandle, "error", &strclr));
	nvs_close(crhandle);
	return strclr;
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
		NOTEDS;
		lcd_write_string("R2 button press");
		dly_msec(100);
		ESP_LOGI("BUTR2", "Two rand button pressed credit = %d", credit);
		if(credit >= 2)
		{
			dispense_r2();
		}
		
	}
	if(credit >= 5)
	{
//		NOTEDS;
//		NOTEDS;
		if(BUTR5)
		{
			NOTEDS;
			lcd_write_string("R5 button press");
			ESP_LOGI("BUTR5", "R5 pressed, credit = %d", credit);
			if(credit > 40)
			{
				dispense_r2();
			}
			if(credit >= 5)
			{
				dispense_r5();
			}
		}
	}
	credit = retrieve_credit();
	if(credit == 0)
	{
		abort();
	}
}



