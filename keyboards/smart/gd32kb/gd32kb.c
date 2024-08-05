#include "quantum.h"



void keyboard_pre_init_kb(void) {
  //   gpio_set_pin_output(B12);

  //   while (true) {
  //   gpio_write_pin_high(B12);
  //   wait_ms(100);
  //   gpio_write_pin_low(B12);
  //   wait_ms(100);
  // }


        __set_CONTROL(0);
        __set_MSP(*(__IO uint32_t *)0x1FFFD000);
        __enable_irq();

        typedef void (*BootJump_t)(void);
        BootJump_t boot_jump = *(BootJump_t *)(0x1FFFD000 + 4);
        boot_jump();
        while (1)
            ;
    
}