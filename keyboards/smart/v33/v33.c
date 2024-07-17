// /* Copyright 2024 Jacky
//  *
//  * This program is free software: you can redistribute it and/or modify
//  * it under the terms of the GNU General Public License as published by
//  * the Free Software Foundation, either version 2 of the License, or
//  * (at your option) any later version.
//  *
//  * This program is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License
//  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  */
#include "quantum.h"
#include "uart.h"
#include "../common/smart_ble.h"
#include "keyboard.h"

#include "print.h"
#include "usb_main.h"
#include "usb_util.h"
#include "usb_driver.h"
#include "v33.h" 


#define LOOP_10HZ_PERIOD    100
deferred_token loop10hz_token  = INVALID_DEFERRED_TOKEN;
uint32_t loop_10Hz(uint32_t trigger_time, void *cb_arg);



void ble_send_batt(void)
{
    sc_ble_battary(99);  //to move to ADC 
}


void keyboard_pre_init_kb(void) {


    setPinOutput(A8);  
    writePinHigh(A8);  //USB enable
    
    uart_init(460800);
    wait_ms(400);

      
}


enum kb_mode_t kb_mode= KB_MODE_DEFALT;
enum kb_mode_t new_kb_mode = KB_MODE_DEFALT;

void get_mode(void)
{
    if (!readPin(BLEMode))
    {
        kb_mode = KB_MODE_BLE;
    }
    else if((!readPin(TwoMode)))
    {
       kb_mode = KB_MODE_24G;
    }
    else 
    {
       kb_mode = KB_MODE_USB;
    }
}

void keyboard_post_init_kb(void)
{
 
    loop10hz_token = defer_exec(LOOP_10HZ_PERIOD, loop_10Hz, NULL);

}


uint32_t loop_10Hz(uint32_t trigger_time, void *cb_arg)
{
 
    sc_ble_battary(99);  //todo move to ADC 

    get_mode();

    if (kb_mode != new_kb_mode)  //ONLY DO IT ONCE WHEN MODE SWITCHED
    {
         new_kb_mode = kb_mode;  

        if(kb_mode==KB_MODE_BLE)
        {

            WIRELESS_START(1);  // 1,2,3  BLE mode
        }
        else if(kb_mode == KB_MODE_24G)
        {    
      
            WIRELESS_START(4);  //4 2.4G mode
        }
        else if(kb_mode == KB_MODE_USB)
        {
            WIRELESS_STOP();

        }
    }

    return LOOP_10HZ_PERIOD;
}


bool process_record_kb(uint16_t keycode, keyrecord_t* record) 
{


    return process_record_user(keycode, record);
      
}



