/*
This is the c configuration file for the keymap

Copyright 2023 mentako-ya

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifdef RGBLIGHT_ENABLE
//#    define RGBLIGHT_EFFECT_BREATHING
//#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
//#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
//#    define RGBLIGHT_EFFECT_SNAKE
//#    define RGBLIGHT_EFFECT_KNIGHT
//#    define RGBLIGHT_EFFECT_CHRISTMAS
//#    define RGBLIGHT_EFFECT_STATIC_GRADIENT
//#    define RGBLIGHT_EFFECT_RGB_TEST
//#    define RGBLIGHT_EFFECT_ALTERNATING
//#    define RGBLIGHT_EFFECT_TWINKLE
#endif

#define TAP_CODE_DELAY 5
#define DYNAMIC_KEYMAP_LAYER_COUNT 4

// #### 自動マウスレイヤー切替有効化 ####
#define AUTO_MOUSE_LAYER_ENABLE

// #### エンコーダー設定 ####
// 左エンコーダー有効 左ボール無効（デフォルト）
#define ENCODER_LEFT_ENABLE
// 右エンコーダー有効 右ボール無効
//#define ENCODER_RIGHT_ENABEL

// #### 両側ボール設定 ####
//#undef ENCODER_ENABLE
// slave側トラックボールをスクロール操作に使用する場合（デフォルト）
//#define SLAVE_SCRL_DISABLE 0
// slave側トラックボールをポインター操作に使用する場合
//#define SLAVE_SCRL_DISABLE 1