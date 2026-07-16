			
#include "buffers.h"
#include "messages.h"

void app_main(void)
{
	init_gpio();
	i2c_init() ;
	hd44780_clear(&lcd_dev);
	lcd_write_string("0123456789ABCDEF");
	ESP_ERROR_CHECK(nvs_flash_init());
	credit = retrieve_credit();
	if(credit)
	{
		credisp();
	}
	strcpy(msgbuf, incoinmsg);
	scrollpos = 0;
	gpio_set_level(HPR1, 0);
	int x;
	int count = 0;
	r10add = 0;
	r1add = 0;
	/* 5. Enable the interrupts */
	ESP_ERROR_CHECK(gpio_intr_enable(R1_in)); 
	ESP_ERROR_CHECK(gpio_intr_enable(R10_in)); 
    while (true) 
	{
		if(!credit || count)
		{
			LEDON;
			lcd_scroll_string(msgbuf);
        	
		}	
		x = r10add + r1add;
		while(x)
		{
			ESP_LOGI("cash_in", "cash input, R10 = %d and R1 = %d", r10add, r1add);
			if(r10add)
			{
				tens_in();
			}
			if(r1add)
			{
				ones_in();
			}
			r10add = 0;
			r1add  = 0;
			x = r10add + r1add;
		}
		DLY5MSEC();
		count++;
		if(count >+ 100)
		{
			count = 0;
		}
//	gpio_set_intr_type(R10_in, GPIO_INTR_NEGEDGE);
//	gpio_set_intr_type(R1_in, GPIO_INTR_NEGEDGE);
    }
}
