/*
 * gpios.c
 *
 *  Created on: 07 Jul 2026
 *      Author: davejplater@gmail.com
 */

 #include "buffers.h" 

 #define OUTPUT ((1ULL << HPR5_FWD) | (1ULL << HPR1) | (1ULL << HPR2) | (1ULL<< HPR5_REV) | (1ULL << HPR5) | (1ULL << ENOTE))
 #define INPUT ((1ULL << R5sense) | (1ULL << R2sense) | (1ULL << R1sense) | (1ULL<< R1_in) | (1ULL << R10_in))

 nvs_handle_t sshandle;
 gpio_config_t output = {}; 
 gpio_config_t input  = {}; 
 uint8_t r10add; 
 uint8_t r1add; 
/* volatile uint64_t timer;
 volatile uint64_t tstore;*/
 gpio_pin_glitch_filter_config_t glitch = {};
 gpio_pin_glitch_filter_config_t glr1ch = {};
 gpio_glitch_filter_handle_t     R10_hand;
 gpio_glitch_filter_handle_t     R1_hand;

 void init_gpio(void) 
 { 
     /* 1. Configure Outputs */
     output.pin_bit_mask = OUTPUT; 
     output.mode = GPIO_MODE_OUTPUT; 
     ESP_ERROR_CHECK(gpio_config(&output)); 
     gpio_set_level(HPR1, 0); 
     gpio_set_level(HPR2, 0); 
     gpio_set_level(HPR5, 1); 

     /* 2. Configure Inputs with NEGEDGE from the start */
     input.pin_bit_mask = INPUT; 
     input.mode = GPIO_MODE_INPUT; 
     input.pull_up_en = true; 
     input.intr_type = GPIO_INTR_DISABLE; 
     ESP_ERROR_CHECK(gpio_config(&input));
	 glitch.clk_src = SOC_MOD_CLK_APB;
	 glr1ch.clk_src = SOC_MOD_CLK_APB;
	 glr1ch.gpio_num = R1_in;
	 glitch.gpio_num = R10_in;
	 gpio_new_pin_glitch_filter(&glitch, &R10_hand);
	 gpio_new_pin_glitch_filter(&glr1ch, &R1_hand);
	 gpio_set_level(HPR5_REV, 0);
	 gpio_set_level(ENOTE, 0);
}




