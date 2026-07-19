/*
 * test.c
 *
 *  Created on: 09 Jul 2026
 *      Author: davepl
 */

 #include "buffers.h"

 // I2C Bus Configuration
 #define I2C_PORT         I2C_NUM_0

 // Dual PCF8574 I2C Hardware Addresses
 #define PCF8574_GPIO_ADDR 0x20  // General I/O expander
 #define PCF8574_LCD_ADDR  0x27  // LCD Backpack expander

 // Device Descriptors
i2c_dev_t pcf_gpio_dev;  // Descriptor for general I/O
i2c_dev_t pcf_lcd_dev;   // Descriptor for the LCD backpack
hd44780_t lcd_dev;       // Descriptor for the HD44780 abstraction

 // Callback function used by the HD44780 engine to pipeline bits
 // It explicitly targets the LCD's expander descriptor (0x27)
 static esp_err_t write_lcd_data(const hd44780_t *lcd, uint8_t data) {
     return pcf8574_port_write(&pcf_lcd_dev, data);
 }

 void i2c_init(void)
 {
     // 1. Initialize the thread-safe i2cdev engine
     ESP_ERROR_CHECK(i2cdev_init());

     // 2. Initialize the General-Purpose PCF8574 (Address 0x20)
     memset(&pcf_gpio_dev, 0, sizeof(i2c_dev_t));
     ESP_ERROR_CHECK(pcf8574_init_desc(&pcf_gpio_dev, PCF8574_GPIO_ADDR, I2C_PORT,
		 SDA, SCL));
     
     // Set all pins on your general I/O expander to high-impedance inputs or low outputs safely
     pcf8574_port_write(&pcf_gpio_dev, 0xFF); 

     // 3. Initialize the LCD-Facing PCF8574 (Address 0x27)
     memset(&pcf_lcd_dev, 0, sizeof(i2c_dev_t));
     ESP_ERROR_CHECK(pcf8574_init_desc(&pcf_lcd_dev, PCF8574_LCD_ADDR, I2C_PORT,
		 SDA, SCL));

     // 4. Configure the HD44780 display parameters
     // Pin layout matches standard LCM1602 I2C sub-boards
     lcd_dev.write_cb = write_lcd_data; // Bind it to target the 0x27 expander
     lcd_dev.pins.rs = 0;
     lcd_dev.pins.e  = 2;
     lcd_dev.pins.d4 = 4;
     lcd_dev.pins.d5 = 5;
     lcd_dev.pins.d6 = 6;
     lcd_dev.pins.d7 = 7;
     lcd_dev.pins.bl = 3; // Backlight Pin
     
     lcd_dev.font = HD44780_FONT_5X8;
     lcd_dev.lines = 2;       // Change to 4 if using a 20x4 display
     lcd_dev.backlight = 1;   // Turn screen backlight ON

     // 5. Initialize the HD44780 layout engine
     esp_err_t res = hd44780_init(&lcd_dev);
     if (res != ESP_OK) {
         printf("Failed to sync and initialize HD44780 LCD: %s\n", esp_err_to_name(res));
         return;
     }

     // 6. Test output sequence
     hd44780_clear(&lcd_dev);
     hd44780_gotoxy(&lcd_dev, 0, 0);
     hd44780_puts(&lcd_dev, "LCD @ 0x        27 online");
     
     /* Toggle a pin on your general GPIO expander to verify separate execution
        Writes logical 0 to Pin 0 on the 0x20 chip, leaving others high */
//     pcf8574_write_bit(&pcf_gpio_dev, 0, 0); 
     
     hd44780_gotoxy(&lcd_dev, 0, 1);
     hd44780_puts(&lcd_dev, "GPIO @ 0x20 ready");
}

uint8_t get_io(void)
{
	uint8_t thedata;
	ESP_ERROR_CHECK(pcf8574_port_read(&pcf_gpio_dev, &thedata));
	return thedata;
}

bool get_pin_level(uint8_t pin)
{
	uint32_t x;
	pcf8574_get_level(&pcf_gpio_dev, pin, &x);
	return (x != 0);
}



 
 