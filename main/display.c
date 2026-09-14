/*
 * display.c
 *
 *  Created on: 11 Jul 2026
 *      Author: davejplater@gmail.com
 */

 #include "buffers.h"
#include <stdlib.h>
 
 
 const char incoinmsg[] = {"- * Insert Coins or Notes press R5 or R2 button and collect change * - "};
 const char incoin2[] 	= {"- * Insert Coins or Notes press "};
 const char calljoe[]   = {"Error Call Joe. Machine empty."};
 
 char msgbuf[128];
 char msg16[17];
 size_t msgsize;
 uint8_t scrollpos;
 
 void loadincoin(void)
 {
	errorflg = retrieve_error();
	switch(errorflg)
	{
		case 0	:	strcpy(msgbuf, incoinmsg);
		 			break;
		case 1	:	sprintf(msgbuf, "%s R5 or R2 button for change, no R1 coins", incoin2);
					break;
		case 2	:	sprintf(msgbuf, "%s R5 button for change, no R2 coins", incoin2);
					break;
		case 3	:	sprintf(msgbuf, "%s R5 button for change, no R2 or R1 coins", incoin2);
					break;
		case 5	:	sprintf(msgbuf, "%s R2 button for change, no R5 coins", incoin2);
					break;
		case 6	:	sprintf(msgbuf, "%s R5 button for change, no R5 or R1 coins", incoin2);
					break;
		case 7	:	sprintf(msgbuf, "Call Joe, no R5 or R2 coins");
					break;
		case 8	:	sprintf(msgbuf, "%s ", calljoe);
					break;
	}
 }
 
 void call_joe(void)
 {
	errorflg = retrieve_error();
	credit = retrieve_credit();
	sprintf(msgbuf, " %s R%d left", calljoe, credit);
	while(errorflg == 8)
	{
		lcd_scroll_string(msgbuf);
		dly_msec(500);
		if(SERVICE)
		{
			errorflg = 0;
			store_error(0);
		}
	}
	while(SERVICE)
	{
		dly_msec(1);
	}
	esp_restart();
 }
void lcd_write_string(const char *str)
{
    char first_half[9] = {0};  // 8 chars + null terminator
    char second_half[9] = {0}; // 8 chars + null terminator
	hd44780_clear(&lcd_dev);
    
    size_t len = strlen(str);

    // Copy up to the first 8 characters
    strncpy(first_half, str, 8);
    
    // Copy the remaining characters if the string spans further
    if (len > 8) {
        strncpy(second_half, str + 8, 8);
    }

    // Write the first chunk to Row 0
    hd44780_gotoxy(&lcd_dev, 0, 0);
    hd44780_control(&lcd_dev, true, false, false); // clear visual cursors if needed
    hd44780_puts(&lcd_dev, first_half);

    // If there is data for the second half, hop to Row 1 (address 0x40 internally)
    if (len > 8) {
        hd44780_gotoxy(&lcd_dev, 0, 1);
        hd44780_puts(&lcd_dev, second_half);
    }
}

void error_msg(char* errtype)
{
	strcpy(msgbuf, calljoe);
	strcpy((msgbuf + sizeof(calljoe)), errtype);
}

void lcd_scroll_string(char *str)
{
	msgsize = strnlen(msgbuf, sizeof(msgbuf));
	strncpy(msg16, msgbuf + scrollpos, 16);
	msg16[16] = 0;
	lcd_write_string(msg16);
	++scrollpos;
	if(scrollpos >= (msgsize - 15))
	{
		scrollpos = 0;
	}
}

void credisp(void)
{
	credit = retrieve_credit();
	int num = sprintf(msg16, "Credit R%d", credit);
	lcd_write_string(msg16);
}

 
