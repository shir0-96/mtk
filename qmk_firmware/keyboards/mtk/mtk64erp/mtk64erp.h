/*
Copylight 2024 mentako_ya
SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "quantum.h"

// clang-format off
#define LAYOUT( \
    L01, L02, L03, L04, L05, L06,              R06, R05, R04, R03, R02, R01, \
    L10, L12, L13, L14, L15, L16,              R16, R15, R14, R13, R12, R10, \
    L20, L21, L23, L24, L25, L26,              R26, R25, R24, R23, R21, R20, \
    L30, L31, L32, L34, L35, L36, L50,    R50, R36, R35, R34, R32, R31, R30, \
    L40, L41,                L51, L52,    R52, R51,                R41, R40, \
              L42,      L43,      L53,    R53,      R43,      R42,           \
			                      L54,    R54,                               \
         L60, L61, L62,                                  R62, R61, R60       \
	) \
    { \
        {   KC_NO, L01,   L02,   L03,   L04,   L05,   L06  }, \
        {   L10,   KC_NO, L12,   L13,   L14,   L15,   L16  }, \
        {   L20,   L21,   KC_NO, L23,   L24,   L25,   L26  }, \
        {   L30,   L31,   L32,   KC_NO, L34,   L35,   L36  }, \
        {   L40,   L41,   L42,   L43, KC_NO, KC_NO, KC_NO  }, \
        {   L50,   L51,   L52,   L53,   L54, KC_NO, KC_NO  }, \
        {   L60,   L61,   L62, KC_NO, KC_NO, KC_NO, KC_NO  }, \
        {   KC_NO, R01,   R02,   R03,   R04,   R05,   R06  }, \
        {   R10,   KC_NO, R12,   R13,   R14,   R15,   R16  }, \
        {   R20,   R21, KC_NO,   R23,   R24,   R25,   R26  }, \
        {   R30,   R31,   R32, KC_NO,   R34,   R35,   R36  }, \
        {   R40,   R41,   R42,   R43, KC_NO, KC_NO, KC_NO  }, \
        {   R50,   R51,   R52,   R53,   R54, KC_NO, KC_NO  }, \
        {   R60,   R61,   R62, KC_NO, KC_NO, KC_NO, KC_NO  } \
    }
// clang-format on

// enum cocot_keycodes {

//     COCOT_SAFE_RANGE = SAFE_RANGE,x
//     CPI_SW,
//     SCRL_SW,
//     ROT_R15,
//     ROT_L15,
//     SCRL_MO,
//     SCRL_TO,
//     SCRL_IN,
// };

// #define CPI_SW QK_KB_0
// #define SCRL_SW QK_KB_1
// #define ROT_R15 QK_KB_2
// #define ROT_L15 QK_KB_3
// #define SCRL_MO QK_KB_4
// #define SCRL_TO QK_KB_5
// #define SCRL_IN QK_KB_6

typedef struct {
    int16_t x;
    int16_t y;
} mtk_motion_t;

typedef union {
    uint32_t raw;
    struct {
        uint16_t cpi : 9;
        uint8_t  sdiv : 3;          // scroll divider
        bool     auto_mouse :true;   // auto mouse layer enable/disable
    };
} ee_config_t;

typedef struct {
        bool this_have_ball;
        bool that_enable;
        bool that_have_ball;

        uint16_t cpi_value;
        bool    cpi_changed;

        bool     scroll_mode;
        uint32_t scroll_mode_changed;
        uint8_t  scroll_div;

        bool     auto_mouse_mode;

        uint32_t scroll_snap_last;
        int8_t   scroll_snap_tension_h;

//        uint16_t       last_kc;
//        keypos_t       last_pos;
//        report_mouse_t last_mouse;
} mtk_config_t;

extern mtk_motion_t mtk_motion;
extern ee_config_t ee_config;
extern mtk_config_t mtk_config;

enum custom_keycodes {
    KBC_RST = SAFE_RANGE, // configuration: reset to default
    KBC_SAVE,             // configuration: save to EEPROM

    CPI_I100, // CPI +100 CPI
    CPI_D100, // CPI -100 CPI
    CPI_I1K,  // CPI +1000 CPI
    CPI_D1K,  // CPI -1000 CPI

    // In scroll mode, motion from primary trackball is treated as scroll
    // wheel.
    SCRL_TO,  // Toggle scroll mode
    SCRL_MO,  // Momentary scroll mode[

    SCRL_DVI, // Increment scroll divider
    SCRL_DVD, // Decrement scroll divider

    AM_TG_CL,       //Toggle auto mouse layer
    AM_TO_CL_INC,   //Increment AUTO_MOUSE_TIME
    AM_TO_CL_DEC,   //Decrement AUTO_MOUSE_TIME
    AM_TO_RST_INC,  //Increment AUTO_MOUSE_TIME
    AM_TO_RST_DEC,  //Decrement AUTO_MOUSE_TIME
};

// #define KBC_RST USER00
// #define KBC_SAVE USER01
// #define CPI_I100 USER02
// #define CPI_D100 USER03
// #define CPI_I1K USER04
// #define CPI_D1K USER05
// #define SCRL_TO USER06
// #define SCRL_MO USER07
// #define SCRL_DVI USER08
// #define SCRL_DVD USER09
// #define AM_TG_CL USER10
// #define AM_TO_CL_INC USER11
// #define AM_TO_CL_DEC USER12
// #define AM_TO_RST_INC USER13
// #define AM_TO_RST_DEC USER14

//////////////////////////////////////////////////////////////////////////////
// configration function

bool mtk_get_scroll_mode(void);

void mtk_set_scroll_mode(bool mode);

uint8_t mtk_get_scroll_div(void);

void mtk_set_scroll_div(uint8_t div);

uint16_t mtk_get_cpi(void);

void mtk_set_cpi(uint16_t cpi);

void set_keylog(uint16_t keycode, keyrecord_t *record);

void  mtk_set_auto_mouse_mode(bool mode);
