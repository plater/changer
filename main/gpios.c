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
 volatile uint8_t r10add; 
 volatile uint8_t r1add; 
 volatile uint64_t timer;
 volatile uint64_t tstore;
 gpio_pin_glitch_filter_config_t glitch = {};
 gpio_pin_glitch_filter_config_t glr1ch = {};
 gpio_glitch_filter_handle_t     R10_hand;
 gpio_glitch_filter_handle_t     R1_hand;

 static void IRAM_ATTR r10_isr_handler(void* arg) 
 { 
     r10add = (~(gpio_get_level(R10_in)) & 1); 
     // Cleanly disable the interrupt so it only triggers once
     gpio_intr_disable(R10_in); 
	 tstore = esp_timer_get_time();
 } 

 static void IRAM_ATTR r1_isr_handler(void* arg) 
 { 
     r1add = (~(gpio_get_level(R1_in)) & 1); 
     // Cleanly disable the interrupt so it only triggers once
     gpio_intr_disable(R1_in);
	 tstore = esp_timer_get_time(); 
 } 

 void init_gpio(void) 
 { 
     /* 1. Configure Outputs */
     output.pin_bit_mask = OUTPUT; 
     output.mode = GPIO_MODE_OUTPUT; 
     ESP_ERROR_CHECK(gpio_config(&output)); 
     gpio_set_level(HPR1, 0); 
     gpio_set_level(HPR2, 0); 
     gpio_set_level(HPR5, 0); 

     /* 2. Configure Inputs with NEGEDGE from the start */
     input.pin_bit_mask = INPUT; 
     input.mode = GPIO_MODE_INPUT; 
     input.pull_up_en = true; 
     // Make sure to use EDGE triggering from the start!
     input.intr_type = GPIO_INTR_NEGEDGE; 
     ESP_ERROR_CHECK(gpio_config(&input));
	 glitch.clk_src = SOC_MOD_CLK_APB;
	 glr1ch.clk_src = SOC_MOD_CLK_APB;
	 glr1ch.gpio_num = R1_in;
	 glitch.gpio_num = R10_in;
	 gpio_new_pin_glitch_filter(&glitch, &R10_hand);
	 gpio_new_pin_glitch_filter(&glr1ch, &R1_hand);

     /* 3. Install ISR Service (safely ignoring "already installed" errors) */
     // Note: We use ESP_INTR_FLAG_IRAM because handlers are marked IRAM_ATTR
     esp_err_t err = gpio_install_isr_service(ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_HIGH);
     if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
         ESP_ERROR_CHECK(err); // Fatal error other than "already installed"
     }

     /* 4. Add handlers */
     ESP_ERROR_CHECK(gpio_isr_handler_add(R1_in, r1_isr_handler, NULL)); 
     ESP_ERROR_CHECK(gpio_isr_handler_add(R10_in, r10_isr_handler, NULL)); 

 }




