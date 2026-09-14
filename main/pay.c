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
 volatile int x;
 uint16_t payed;
 nvs_handle_t crhandle;
 volatile uint16_t credit;
 volatile uint8_t  errorflg;
 float crdhod, crdhod0, crdhod1;
 uint32_t numbs; 
 uint32_t crumbs;
 
 void register_error(uint8_t what)
 {
	errorflg = retrieve_error();
	errorflg = errorflg + what;
	store_error(errorflg);
 }
 
void dispense_r1(void)//crdhod, crdhod0, crdhod1;
{
	credit = retrieve_credit();
	errorflg = retrieve_error();
	if((errorflg == 1) || (errorflg == 3) || (errorflg == 6) || (errorflg == 8))// R1 hopper not functional
	{
		hopper_emt(1);
		goto error;
	}
	numbs = credit;
	ESP_LOGI("disR1", "credit = %d numbs = %d ", credit, numbs);
	sprintf(msgbuf, "Dispense R1 x %d", (uint16_t)numbs);
	lcd_write_string(msgbuf);
	uint8_t y = pay_r1(numbs);
	switch(y)
	{
		case HOPPER_EMT :	hopper_emt(1);
							break;
		case HOPPER_JAM :	hopper_emt(1);
							break;
		case PAY_OK   	:	ESP_LOGI("R1PAY_OK", "Returning");
							goto error;
	}
	error:
}

uint8_t pay_r1(uint8_t numb)
{
		R1ON;
		dly_msec(3);//Wait for valid signal after PU
		nextcoin1:
 		tstore = esp_timer_get_time();
		do
		{
			vTaskDelay(1);
			if(get_elapsed() > (5000 * 1000))// Allow 5 seconds for the first coin
			{
				R1OFF;
				ESP_LOGI("payr1", "hopper timeout");
				register_error(1);
				return HOPPER_EMT;
			}
			x = gpio_get_level(R1sense);
		}while(x == 1);
		uint64_t gone = get_elapsed();
		ESP_LOGI("payr1", "took %llu micro seconds to detect coin", gone);
		tstore = esp_timer_get_time();
		do
		{
			if(get_elapsed() > (500 * 1000))// Allow 500ms for the coin exit
			{
				R1OFF;
				ESP_LOGI("payr1", "hopper jam");
				register_error(1);
				return HOPPER_JAM;
			}
			x = gpio_get_level(R1sense);
		}while(x == 0);// Coin on it's way out
		gone = get_elapsed();
		ESP_LOGI("payr1", "coin took %llu micro seconds to pass exit", gone);
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
	ESP_LOGI("payR1 Ok", "credit = %d numbs = %d", credit, numbs);
	return PAY_OK;
}

void dispense_r2(void)//crdhod, crdhod0, crdhod1;
{
	credit = retrieve_credit();
	errorflg = retrieve_error();
	if((errorflg == 2) || (errorflg == 3) || (errorflg == 7))//R2 hopper disabled
	{
		hopper_emt(2);
		goto error;
	}
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
		case HOPPER_EMT :	hopper_emt(2);
							break;
		case HOPPER_JAM :	hopper_emt(2);
							break;
		case PAY_OK   	:	ESP_LOGI("R2PAY_OK", "Returning");
							goto error;
	}
	error:
}

uint8_t pay_r2(uint8_t numb)
{
	uint64_t elapsed;
	R2ON;
	dly_msec(3);
	nextcoin2:
	tstore = esp_timer_get_time();
	ESP_LOGI("1payr2", "tstore = %llu numbs = %d", tstore, numbs);
	do
	{
		vTaskDelay(1);
		if(get_elapsed() > (5000 * 1000))// Allow 5 seconds for the first coin
		{
			R2OFF;
			ESP_LOGI("payr2", "hopper timeout");
			register_error(2);
			return HOPPER_EMT;
		}
		x = gpio_get_level(R2sense);
	}while(x == 1);
	tstore = esp_timer_get_time();
	do// Coin on it's way out
	{
		vTaskDelay(1);
		if(get_elapsed() > (500 * 1000))// Allow 500ms for the coin exit
		{
			R2OFF;
			ESP_LOGI("payr2", "hopper jam");
			register_error(2);
			return HOPPER_JAM;
		}
		x = gpio_get_level(R2sense);
	}while(x == 0);
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
		goto nextcoin2;
	}
	else
	{
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
}

void dispense_r5(void)//crdhod, crdhod0, crdhod1;
{
	credit = retrieve_credit();
	errorflg = retrieve_error();
	if(errorflg >= 5)
	{
		hopper_emt(5);
		goto error;
	}
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
		case HOPPER_EMT :	hopper_emt(5);
							break;
		case HOPPER_JAM :	hopper_emt(5);
							break;
		case PAY_OK   	:	ESP_LOGI("R5PAY_OK", "Returning");
							goto error;
	}
	error:
}

uint8_t pay_r5(uint8_t numb)
{
	uint64_t elapsed;
	R5ON;
	dly_msec(3);
	nextcoin5:
	tstore = esp_timer_get_time();
	do
	{
		vTaskDelay(1);
		if(get_elapsed() > (5000 * 1000))// Allow 5 seconds for the first coin
		{
			R5OFF;
			ESP_LOGI("payr5", "hopper timeout");
			register_error(5);
			return HOPPER_EMT;
		}
	x = gpio_get_level(R5sense);
	}while(x == 1);
	tstore = esp_timer_get_time();
	(numb == 1) ? R5OFF : R5ON; //Turn off on last coin exit
	do// Coin on it's way out
	{
		vTaskDelay(1);
		if(get_elapsed() > (500 * 1000))// Allow 500ms for the coin exit
		{
			R5OFF;
			ESP_LOGI("payr5", "hopper jam");
			register_error(5);
			return HOPPER_JAM;
		}
	x = gpio_get_level(R5sense);
	}while(x == 0);
	tstore = esp_timer_get_time();
	--numb;
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
	while(elapsed <= (300 * 1000))//wait for next pulse
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

uint16_t get_elapsedm(void)
{
	uint64_t lapsed;
	uint16_t lapsedm;
	timer = esp_timer_get_time();
	lapsed = timer - tstore;
	lapsedm = lapsed / 1024;
	return lapsedm;
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
	else if(err)
	{
		ESP_ERROR_CHECK(err);
	} 
	err = nvs_get_u16(crhandle, "error", &strclr);
	if(err)
	{
		if(err == ESP_ERR_NVS_NOT_FOUND)
		{
			nvs_close(crhandle);
			store_error(0);
			return 0;
		}
		ESP_ERROR_CHECK(err);
	}
	
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
	err = nvs_get_u16(crhandle, "cash", &x);
	if(err)
	{
		if(err == ESP_ERR_NVS_NOT_FOUND)
		{
			nvs_close(crhandle);
			store_credit(0);
			return 0;
		}
		ESP_ERROR_CHECK(err);
	}
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
	s = 0;
	errorflg = retrieve_error();
	if(BUTR2)
	{
		if((errorflg == 2) || (errorflg == 3) || (errorflg == 7))
		{
			lcd_write_string("R2 hopper empty");
			dly_msec(3000);
			shcred = 0;
			goto error;
		}
		NOTEDS;
		lcd_write_string("R2 button press");
		dly_msec(100);
		ESP_LOGI("BUTR2", "Two rand button pressed credit = %d", credit);
		if(credit >= 2)
		{
			dispense_r2();
		}
		error:
	}
	if(credit >= 5)
	{
		NOTEDS;
		if(BUTR5)
		{
			if(errorflg >= 5)
			{
				lcd_write_string("R5 hopper empty");
				dly_msec(3000);
				shcred = 0;
				goto error5;
			}
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
	if(credit < 1)
	{
		set_io(0x3F);
	}
	error5:
}

void hopper_emt(int what)
{
	volatile int y = 0;
	int is;
	credit = retrieve_credit();
	errorflg = retrieve_error();
	if(errorflg == 8)
	{
		call_joe();
	}
	switch(what)
	{
		case 1 :	if((credit >= 5) && (errorflg < 5))
					{
						is = 5;
						numbs = credit / 5;
						y = pay_r5(numbs);
					}
					if((credit >= 2) && (errorflg != 2) && (errorflg != 3) && (errorflg != 7))
					{
						is = 2;
						numbs = credit / 2;
						y = pay_r2(numbs);
					}
					break;
		case 2 :	if((credit >= 5) && (errorflg < 5))
					{
						is = 5;
						numbs = credit / 5;
						y = pay_r5(numbs);
					}
					if((credit >= 1) && (errorflg != 1) && (errorflg != 3) && (errorflg != 6))
					{
						is = 1;
						numbs = credit;
						y = pay_r1(numbs);
					}
					break;
		case 5 :	if((credit >= 2) && (errorflg != 2) && (errorflg != 3) && (errorflg != 7))
					{
						is = 2;
						numbs = credit / 2;
						y = pay_r2(numbs);
					}
					if((credit >= 1) && (errorflg != 1) && (errorflg != 3) && (errorflg != 6))
					{
						is = 1;
						numbs = credit;
						y = pay_r1(numbs);
					}
					break;
	}
	
}


