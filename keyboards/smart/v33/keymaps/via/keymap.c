#include QMK_KEYBOARD_H


#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] =  { ENCODER_CCW_CW(KC_VOLU, KC_VOLD)  },
    [1] =  { ENCODER_CCW_CW(KC_VOLU, KC_VOLD)  },
	[2] =  { ENCODER_CCW_CW(KC_VOLU, KC_VOLD)  },
	[3] =  { ENCODER_CCW_CW(KC_VOLU, KC_VOLD)  }

};
#endif


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {


		[0] = LAYOUT(
		KC_MUTE,  MO(1),  KC_TAB,   KC_BSPC,   KC_PRINT_SCREEN,   KC_SCROLL_LOCK,   KC_PAUSE, 
		KC_NUM_LOCK,  KC_KP_SLASH,    KC_KP_ASTERISK,    KC_KP_MINUS,    KC_INSERT,    KC_HOME,    KC_PAGE_UP,    
		KC_KP_7, KC_KP_8,    KC_KP_9,    KC_KP_PLUS,    KC_DELETE,    KC_END,    KC_PAGE_DOWN,   
		KC_KP_4, KC_KP_5,    KC_KP_6,    KC_UP,    KC_LEFT,    KC_DOWN,    KC_RIGHT,   
		KC_KP_1, KC_KP_2, KC_KP_3,   KC_KP_ENTER,KC_KP_DOT,  KC_KP_0
	),
	[1] = LAYOUT(
	    KC_MUTE, _______,  _______,   _______,   _______,   _______,   _______, 
	    _______, RGB_SPD,  RGB_SPI,   RGB_SAD,   _______,   _______,   _______,   
		_______, _______,  _______,   RGB_SAI,   _______,   _______,   _______,    
	    _______, _______,  _______,   RGB_VAI,   RGB_HUD,   RGB_VAD,   RGB_HUI,   
		_______, _______,  _______,   RGB_MOD,   _______,   _______
	),
	[2] = LAYOUT(
	    KC_MUTE, _______,  _______,   _______,   _______,   _______,   _______, 
	    _______, _______,  _______,   _______,   _______,   _______,   _______,   
		_______, _______,  _______,   _______,   _______,   _______,   _______,    
	    _______, _______,  _______,   _______,   _______,   _______,   _______,   
		_______, _______,  _______,   _______,   _______,   _______
	),
	[3] = LAYOUT(
	    KC_MUTE, _______,  _______,   _______,   _______,   _______,   _______, 
	    _______, _______,  _______,   _______,   _______,   _______,   _______,   
		_______, _______,  _______,   _______,   _______,   _______,   _______,    
	    _______, _______,  _______,   _______,   _______,   _______,   _______,   
		_______, _______,  _______,   _______,   _______,   _______
	)

};
						

