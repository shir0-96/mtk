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

ee_config_t ee_config = {
 .cpi = 0,
 .sdiv = 0,
};

mtk_config_t mtk_config = {
    .this_have_ball = false,
    .that_enable    = false,
    .that_have_ball = false,

    .cpi_value   = 0,
    .cpi_changed = false,

    .scroll_mode = false,
    .scroll_div  = 0,
};

 static void add_cpi(int16_t delta) {
     int16_t v = mtk_get_cpi() + delta;
     mtk_set_cpi(v < 1 ? 1 : v);
 }

 static void add_scroll_div(int16_t delta) {
     int8_t v = mtk_get_scroll_div() + delta;
     mtk_set_scroll_div(v < 1 ? 1 : v);
 }


#ifdef POINTING_DEVICE_ENABLE
#    if defined(POINTING_DEVICE_LEFT)
#        define POINTING_DEVICE_THIS_SIDE is_keyboard_left()
#    elif defined(POINTING_DEVICE_RIGHT)
#        define POINTING_DEVICE_THIS_SIDE !is_keyboard_left()
#    elif defined(POINTING_DEVICE_COMBINED)
#        define POINTING_DEVICE_THIS_SIDE true
#    endif

//////////////////////////////////////////////////////////////////////////////
// Configurations

#ifndef MTK_CPI_DEFAULT
#    define MTK_CPI_DEFAULT 1000
#endif

#ifndef MTK_SCROLL_DIV_MIN
#    define MTK_SCROLL_DIV_MIN 1
#endif

#ifndef MTK_SCROLL_DIV_DEFAULT
#    define MTK_SCROLL_DIV_DEFAULT 8
#endif

#ifndef MTK_SCROLL_DIV_MAX
#    define MTK_SCROLL_DIV_MAX 16
#endif

#ifndef MTK_SCROLLBALL_INHIVITOR
#    define MTK_SCROLLBALL_INHIVITOR 5
#endif

#ifndef MTK_SCROLLSNAP_ENABLE
#    define MTK_SCROLLSNAP_ENABLE 1
#endif

#ifndef MTK_SCROLLSNAP_RESET_TIMER
#    define MTK_SCROLLSNAP_RESET_TIMER 100
#endif

#ifndef MTK_SCROLLSNAP_TENSION_THRESHOLD
#    define MTK_SCROLLSNAP_TENSION_THRESHOLD 12
#endif

//////////////////////////////////////////////////////////////////////////////
// Constants

#define MTK_TX_GETINFO_INTERVAL 500
#define MTK_TX_GETINFO_MAXTRY 10
#define MTK_TX_GETMOTION_INTERVAL 4

//////////////////////////////////////////////////////////////////////////////
// Types

void eeconfig_init_kb(void) {
    mtk_config.cpi_value = MTK_CPI_DEFAULT;
    mtk_config.cpi_changed = false;
    mtk_config.scroll_mode = false;
    mtk_config.scroll_div = MTK_SCROLL_DIV_DEFAULT;
    mtk_config.auto_mouse_mode = true;

    ee_config_t c = {
        .cpi  = mtk_config.cpi_value / PMW33XX_CPI_STEP,
        .sdiv = mtk_config.scroll_div,
        .auto_mouse = mtk_config.auto_mouse_mode,
    };
    eeconfig_update_kb(c.raw);
    eeconfig_init_user();
}

void matrix_init_kb(void) {
    // is safe to just read CPI setting since matrix init
    // comes before pointing device init.
    ee_config.raw = eeconfig_read_kb();
    mtk_set_cpi(ee_config.cpi * PMW33XX_CPI_STEP);
    mtk_set_scroll_div(ee_config.sdiv);
    mtk_set_auto_mouse_mode(ee_config.auto_mouse);

    if (mtk_config.cpi_value > PMW33XX_CPI_MAX){
        eeconfig_init_kb();
    }
    matrix_init_user();
}

void pointing_device_init_kb(void) {
        pmw33xx_init(0);                                    // index 1 is the second device.
        pmw33xx_set_cpi(0, mtk_config.cpi_value);       // applies to first sensor
        set_auto_mouse_enable(mtk_config.auto_mouse_mode);
}

// Scroll Accumulation
static int16_t scroll_h;
static int16_t scroll_v;
//static int16_t h_acm = 0;
//ßstatic int16_t v_acm = 0;
static int16_t x_rev_max = 0;
static int16_t y_rev_max = 0;
report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {


    int16_t x_rev =  mouse_report.y * -1;
    int16_t y_rev =  mouse_report.x * -1;
    x_rev_max = (x_rev_max < x_rev ?  x_rev : x_rev_max);
    y_rev_max = (y_rev_max < y_rev ?  y_rev : y_rev_max);

    if (mtk_get_scroll_mode()) {
        if (abs(x_rev) > abs(y_rev)) {
            y_rev = 0;
        } else {
            x_rev = 0;
        }

        // oddball accumulate scroll
        scroll_h += x_rev;
        scroll_v += y_rev;

        int8_t scaled_scroll_h = scroll_h / (mtk_get_scroll_div() * 8);
        int8_t scaled_scroll_v = scroll_v / (mtk_get_scroll_div() * 8);

        // clear accumulated scroll on assignment

        if (scaled_scroll_h != 0) {
            mouse_report.h = -scaled_scroll_h;
            scroll_h       = 0;
        }

        if (scaled_scroll_v != 0) {
            mouse_report.v = -scaled_scroll_v;
            scroll_v       = 0;
        }

        // accumulate scroll
        // h_acm += x_rev ;
        // v_acm += y_rev * -1;

        // int8_t h_rev = h_acm >> mtk_get_scroll_div();
        // int8_t v_rev = v_acm >> mtk_get_scroll_div();

        // // clear accumulated scroll on assignment

        // if (h_rev != 0) {
        //     if (mouse_report.h + h_rev > MTK_SCROLLBALL_INHIVITOR) {
        //         h_rev = - mouse_report.h;
        //     } else if (mouse_report.h + h_rev < MTK_SCROLLBALL_INHIVITOR * -1) {
        //         h_rev = - mouse_report.h;
        //     }
        //     mouse_report.h += h_rev + (h_rev > 0 ? MTK_SCROLLBALL_INHIVITOR * -1 : MTK_SCROLLBALL_INHIVITOR);
        //     h_acm -= h_rev << mtk_get_scroll_div();
        // }
        // if (v_rev != 0) {
        //     if (mouse_report.v + v_rev > MTK_SCROLLBALL_INHIVITOR) {
        //         v_rev = - mouse_report.v;
        //     } else if (mouse_report.v + v_rev < MTK_SCROLLBALL_INHIVITOR * -1) {
        //         v_rev = - mouse_report.v;
        //     }
        //     mouse_report.v += v_rev + (v_rev > 0 ? MTK_SCROLLBALL_INHIVITOR * -1 : MTK_SCROLLBALL_INHIVITOR) ;
        //     v_acm -= v_rev << mtk_get_scroll_div();
        // }

        mouse_report.x = 0;
        mouse_report.y = 0;

    } else {
        mouse_report.x = x_rev;
        mouse_report.y = y_rev;
        mouse_report.h = 0;
        mouse_report.v = 0;
    }

    return pointing_device_task_user(mouse_report);
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    // Auto enable scroll mode when the highest layer is 3
    mtk_set_scroll_mode(get_highest_layer(state) == 3);

    #ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    switch(get_highest_layer(remove_auto_mouse_layer(state, true))) {
        case 1 ... 3:
            state = remove_auto_mouse_layer(state, false);
            set_auto_mouse_enable(false);
            break;
        default:
            if(!mtk_get_auto_mouse_mode()){
                state = remove_auto_mouse_layer(state, false);
            }
            set_auto_mouse_enable(mtk_get_auto_mouse_mode());
            break;
    }
    #endif

    return layer_state_set_user(state);
}
#endif

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {

    if (!process_record_user(keycode, record)) {
        return false;
    }

    // strip QK_MODS part.
    if (keycode >= QK_MODS && keycode <= QK_MODS_MAX) {
        keycode &= 0xff;
    }

    switch (keycode) {
#ifndef MOUSEKEY_ENABLE
        // process KC_MS_BTN1~8 by myself
        // See process_action() in quantum/action.c for details.
        case KC_MS_BTN1 ... KC_MS_BTN8: {
            extern void register_mouse(uint8_t mouse_keycode, bool pressed);
            register_mouse(keycode, record->event.pressed);
            // to apply QK_MODS actions, allow to process others.
            return true;
        }
#endif

        case SCRL_MO:
            mtk_set_scroll_mode(record->event.pressed);
            return false;
    }

    // process events which works on pressed only.
    if (record->event.pressed) {

        set_keylog(keycode, record);

        switch (keycode) {
            case KBC_RST:
                ee_config.raw = eeconfig_read_kb();
                mtk_set_cpi(ee_config.cpi * PMW33XX_CPI_STEP);
                mtk_set_scroll_div(ee_config.sdiv);
                mtk_set_auto_mouse_mode(ee_config.auto_mouse);
                break;
            case KBC_SAVE:
                ee_config.cpi  = mtk_config.cpi_value / PMW33XX_CPI_STEP;
                ee_config.sdiv = mtk_config.scroll_div;
                ee_config.auto_mouse = mtk_config.auto_mouse_mode;
                eeconfig_update_kb(ee_config.raw);
                break;
            case CPI_I100:
                add_cpi(100);
                break;
            case CPI_D100:
                add_cpi(-100);
                break;
            case CPI_I1K:
                add_cpi(1000);
                break;
            case CPI_D1K:
                add_cpi(-1000);
                break;

            case SCRL_TO:
                mtk_set_scroll_mode(!mtk_config.scroll_mode);
                break;
            case SCRL_DVI:
                add_scroll_div(1);
                break;
            case SCRL_DVD:
                add_scroll_div(-1);
                break;

            case AM_TG_CL:
                mtk_set_auto_mouse_mode(!mtk_config.auto_mouse_mode);
                break;

            default:
                return true;
        }
        return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// configration function

bool mtk_get_scroll_mode(void) {
    return mtk_config.scroll_mode;
}

void mtk_set_scroll_mode(bool mode) {
    if (mode != mtk_config.scroll_mode) {
        mtk_config.scroll_mode_changed = timer_read32();
    }
    mtk_config.scroll_mode = mode;
}

uint8_t mtk_get_scroll_div(void) {
    return mtk_config.scroll_div == 0 ? MTK_SCROLL_DIV_DEFAULT : mtk_config.scroll_div;
}

void mtk_set_scroll_div(uint8_t div) {
    mtk_config.scroll_div = div > MTK_SCROLL_DIV_MAX ? MTK_SCROLL_DIV_MAX : div;
}

uint16_t mtk_get_cpi(void) {
    return mtk_config.cpi_value == 0 ? MTK_CPI_DEFAULT : mtk_config.cpi_value;
}

void mtk_set_cpi(uint16_t cpi) {
    if (cpi > PMW33XX_CPI_MAX) {
       cpi = PMW33XX_CPI_MAX;
    }else if (cpi < PMW33XX_CPI_MIN * 2) {
       cpi = PMW33XX_CPI_MIN * 2;
    }
    mtk_config.cpi_value   = cpi;
    mtk_config.cpi_changed = true;
    pointing_device_set_cpi(cpi == 0 ? MTK_CPI_DEFAULT - 1 : cpi - 1);
}

bool mtk_get_auto_mouse_mode(void) {
    return mtk_config.auto_mouse_mode;
}

void mtk_set_auto_mouse_mode(bool mode) {
    mtk_config.auto_mouse_mode = mode;
}


////////////////////////////////////////////////////////////////////////////
// oled function

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
  snprintf(keylog_str, sizeof(keylog_str), "c:%-2d r:%-2d k:%04x n:%-c",
           record->event.key.row, record->event.key.col,
           keycode, name);
}

void oled_render_keylog(void) {
    oled_write_ln_P(keylog_str, false);
}

char pointing_str[23] = {};
void oled_render_pointing(void) {
    snprintf(pointing_str, sizeof(pointing_str), "cpi:%-5d div:%-d sc:%-d",
    mtk_config.cpi_value, mtk_config.scroll_div, mtk_config.scroll_mode);
    oled_write_ln_P(pointing_str, false);
}

char rgb_str[25] = {};
void oled_render_rgb(void) {
    snprintf(rgb_str, sizeof(rgb_str), "R:%-2d h:%-3d s:%-3d v:%-3d",
    rgblight_get_mode(), rgblight_get_hue(), rgblight_get_sat(),rgblight_get_val());
    oled_write_ln_P(rgb_str, false);
}

bool oled_task_kb(void) {

    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer:"), false);
    oled_write_ln_P(get_u8_str(get_highest_layer(layer_state), ' '), false);

    oled_render_keylog();
    oled_render_pointing();
    oled_render_rgb();

    return false;
}
#endif
