/*
 * display.c
 *
 *  Created on: 11 Jul 2026
 *      Author: davejplater@gmail.com
 */

 #include "buffers.h"
 #include "messages.h"
 
 
 
 char msgbuf[128];
 char msg16[17];
 uint8_t scrollpos;
 size_t msgsize;

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
	msgsize = strlen(msgbuf);
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
	int num = sprintf(msg16, "Credit R%d", credit);
	lcd_write_string(msg16);
}

 
