/*
Copylight 2024 mentako_ya
SPDX-License-Identifier: GPL-2.0-or-later
*/

#include QMK_KEYBOARD_H
#ifdef CONSOLE_ENABLE
#include <print.h>
#endif


//////////////////////////////////////////////////////////////////////////////

// clang-format off
const matrix_row_t matrix_mask[MATRIX_ROWS] = {

    0b01111110,
    0b01111101,
    0b01111011,
    0b01110111,
    0b01101111,
    0b01011111,
    0b00111111,

    0b01111110,
    0b01111101,
    0b01111011,
    0b01110111,
    0b01101111,
    0b01011111,
    0b00111111,

};
// clang-format on

#ifdef POINTING_DEVICE_ENABLE
void pointing_device_init_kb(void) {

    pmw33xx_init(0);         // index 1 is the second device.
    pmw33xx_set_cpi(0, 1000); // applies to first sensor

}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {


    int8_t x_rev =  + mouse_report.x;
    int8_t y_rev =  + mouse_report.y;

    if (false) {
        // // rock scroll direction
        // if (abs(x_rev) > abs(y_rev)) {
        //     y_rev = 0;
        // } else {
        //     x_rev = 0;
        // }

        // // accumulate scroll
        // h_acm += x_rev * cocot_config.scrl_inv;
        // v_acm += y_rev * cocot_config.scrl_inv * -1;

        // int8_t h_rev = h_acm >> scrl_div_array[cocot_config.scrl_div];
        // int8_t v_rev = v_acm >> scrl_div_array[cocot_config.scrl_div];

        // // clear accumulated scroll on assignment

        // if (h_rev != 0) {
        //     if (mouse_report.h + h_rev > 127) {
        //         h_rev = 127 - mouse_report.h;
        //     } else if (mouse_report.h + h_rev < -127) {
        //         h_rev = -127 - mouse_report.h;
        //     }
        //     mouse_report.h += h_rev;
        //     h_acm -= h_rev << scrl_div_array[cocot_config.scrl_div];
        // }
        // if (v_rev != 0) {
        //     if (mouse_report.v + v_rev > 127) {
        //         v_rev = 127 - mouse_report.v;
        //     } else if (mouse_report.v + v_rev < -127) {
        //         v_rev = -127 - mouse_report.v;
        //     }
        //     mouse_report.v += v_rev;
        //     v_acm -= v_rev << scrl_div_array[cocot_config.scrl_div];
        // }

        mouse_report.x = 0;
        mouse_report.y = 0;
        mouse_report.h = y_rev * -1;
        mouse_report.v = x_rev * -1;
    } else {
        mouse_report.x = y_rev * -1;
        mouse_report.y = x_rev * -1;
        mouse_report.h = 0;
        mouse_report.v = 0;
    }

    return pointing_device_task_user(mouse_report);
}
#endif

#ifdef OLED_ENABLE
oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    if (is_keyboard_left()) {
        return OLED_ROTATION_180;
    }
    return rotation;
}

char keylog_str[24] = {};

const char code_to_name[60] = {
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\',
    '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

void set_keylog(uint16_t keycode, keyrecord_t *record) {
    char name = ' ';
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) ||
        (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX)) {
            keycode = keycode & 0xFF;
    }

    if (keycode < 60) {
        name = code_to_name[keycode];
    }

  // update keylog
  snprintf(keylog_str, sizeof(keylog_str), "c:%2d r:%2d k:%2d n:%c",
           record->event.key.row, record->event.key.col,
           keycode, name);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    set_keylog(keycode, record);
  }
  return true;
}

void oled_render_keylog(void) {
    oled_write_ln_P(keylog_str, false);
}

bool oled_task_kb(void) {

    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer:"), false);
    oled_write_ln_P(get_u8_str(get_highest_layer(layer_state), ' '), false);

    oled_render_keylog();

#   ifdef RGBLIGHT_ENABLE
        oled_write_P(PSTR("RGB Mode: "), false);
        oled_write_ln(get_u8_str(rgblight_get_mode(), ' '), false);
        oled_write_P(PSTR("h: "), false);
        oled_write(get_u8_str(rgblight_get_hue(), ' '), false);
        oled_write_P(PSTR("s: "), false);
        oled_write(get_u8_str(rgblight_get_sat(), ' '), false);
        oled_write_P(PSTR("v: "), false);
        oled_write_ln(get_u8_str(rgblight_get_val(), ' '), false);
#   endif
    return false;
}
#endif
