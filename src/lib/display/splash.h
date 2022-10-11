/*
 * splash.h
 *
 *  Created on: Oct 9, 2020 - Modified May 13, 2021
 *      Author: Zac Adam-MacEwen (Arcana Labs)
 */

#ifndef LIB_DISPLAY_SPLASH_H_
#define LIB_DISPLAY_SPLASH_H_

#pragma PERSISTENT(splash_bitmap)
unsigned char splash_bitmap[] = {
     // 'ARCANA_LABS', 128x128px
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0x87, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xe1, 0x9f, 0xc8, 0x0f, 0xff, 0xff, 0xf9, 0x87, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xc3, 0x9f, 0xcf, 0x80, 0xff, 0xff, 0xf9, 0xe3, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0x0f, 0x9f, 0xcf, 0x80, 0x0f, 0xff, 0xf9, 0xf0, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xfc, 0x3f, 0x9f, 0x9f, 0x18, 0x00, 0xff, 0xf9, 0xfc, 0x3f, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xf0, 0xff, 0x9f, 0x9f, 0x3c, 0x30, 0x1f, 0xf9, 0xff, 0x0f, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xc3, 0xff, 0x9f, 0x9e, 0x3c, 0x0f, 0x01, 0xf9, 0xff, 0xc3, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0x87, 0xff, 0x9f, 0x9e, 0x79, 0x83, 0xe0, 0x19, 0xff, 0xe1, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xfe, 0x1f, 0xff, 0x9f, 0x3c, 0xf3, 0xc1, 0xfe, 0x01, 0xff, 0xf8, 0x7f, 0xff, 0xff,
 0xff, 0xff, 0xf8, 0x7f, 0xff, 0x9f, 0x3c, 0xe7, 0x80, 0x7f, 0xe1, 0xff, 0xfe, 0x1f, 0xff, 0xff,
 0xff, 0xff, 0xe1, 0xff, 0xff, 0x9f, 0x39, 0xce, 0x02, 0x1f, 0xf1, 0xff, 0xff, 0x87, 0xff, 0xff,
 0xff, 0xff, 0x87, 0xff, 0xff, 0x9f, 0x31, 0x9c, 0x21, 0x87, 0xf1, 0xff, 0xff, 0xe1, 0xff, 0xff,
 0xff, 0xff, 0x0f, 0xff, 0xff, 0x9e, 0x73, 0x30, 0xe0, 0xe3, 0xf1, 0xff, 0xff, 0xf0, 0xff, 0xff,
 0xff, 0xfc, 0x3f, 0xff, 0xff, 0x9e, 0x62, 0x42, 0x00, 0x70, 0xe1, 0xff, 0xff, 0xfc, 0x3f, 0xff,
 0xff, 0xf0, 0xff, 0xff, 0xff, 0x9e, 0x64, 0x00, 0x10, 0x3c, 0x21, 0xff, 0xff, 0xff, 0x0f, 0xff,
 0xff, 0xc3, 0xff, 0xff, 0xff, 0x9e, 0x48, 0x00, 0x00, 0x9f, 0x01, 0xff, 0xff, 0xff, 0xc3, 0xff,
 0xff, 0x0f, 0xff, 0xff, 0xff, 0x9c, 0xc0, 0x00, 0x00, 0xcf, 0x89, 0xff, 0xff, 0xff, 0xf0, 0xff,
 0xfe, 0x1f, 0xff, 0xff, 0xff, 0x9c, 0x80, 0x1f, 0xf8, 0x67, 0xc9, 0xff, 0xff, 0xff, 0xf8, 0x7f,
 0xf8, 0x7f, 0xff, 0xff, 0xff, 0x9c, 0x06, 0x1f, 0xf8, 0x33, 0x89, 0xff, 0xff, 0xff, 0xfe, 0x1f,
 0xe1, 0xff, 0xff, 0xff, 0xff, 0x99, 0x02, 0x1f, 0xf8, 0x39, 0x09, 0xff, 0xff, 0xff, 0xff, 0x87,
 0x87, 0xff, 0xff, 0xff, 0xff, 0x98, 0x32, 0x1f, 0xf8, 0x9c, 0x19, 0xff, 0xff, 0xff, 0xff, 0xe1,
 0x1f, 0xff, 0xff, 0xff, 0xff, 0x98, 0x39, 0x1f, 0xf8, 0x9c, 0x19, 0xff, 0xff, 0xff, 0xff, 0xf8,
 0x87, 0xff, 0xff, 0xff, 0xff, 0x98, 0x39, 0x1f, 0xf8, 0x4c, 0x19, 0xff, 0xff, 0xff, 0xff, 0xe1,
 0xe1, 0xff, 0xff, 0xff, 0xff, 0x90, 0x9c, 0x1f, 0xf8, 0x40, 0x99, 0xff, 0xff, 0xff, 0xff, 0xc7,
 0xf0, 0xff, 0xff, 0xff, 0xff, 0x91, 0xcc, 0x1f, 0xf8, 0x60, 0x39, 0xff, 0xff, 0xff, 0xff, 0x0f,
 0xfc, 0x3f, 0xff, 0xff, 0xff, 0x91, 0xe6, 0x1f, 0xf8, 0x01, 0x39, 0xff, 0xff, 0xff, 0xfc, 0x3f,
 0xff, 0x0f, 0xff, 0xff, 0xff, 0x93, 0xf2, 0x00, 0x00, 0x01, 0x39, 0xff, 0xff, 0xff, 0xf0, 0xff,
 0xff, 0xc3, 0xff, 0xff, 0xff, 0x80, 0xf9, 0x00, 0x00, 0x13, 0x39, 0xff, 0xff, 0xff, 0xc3, 0xff,
 0xff, 0xf0, 0xff, 0xff, 0xff, 0x84, 0x3c, 0x8c, 0x00, 0x26, 0x79, 0xff, 0xff, 0xff, 0x8f, 0xff,
 0xff, 0xf8, 0x7f, 0xff, 0xff, 0x87, 0x1e, 0x00, 0x02, 0x66, 0x79, 0xff, 0xff, 0xfe, 0x1f, 0xff,
 0xff, 0xfe, 0x1f, 0xff, 0xff, 0x87, 0x87, 0x03, 0x8c, 0xce, 0x79, 0xff, 0xff, 0xf8, 0x7f, 0xff,
 0xff, 0xff, 0x87, 0xff, 0xff, 0x8f, 0xe1, 0x8e, 0x19, 0xcc, 0xf9, 0xff, 0xff, 0xe1, 0xff, 0xff,
 0xff, 0xff, 0xe1, 0xff, 0xff, 0x8f, 0xf8, 0x40, 0x73, 0x9c, 0xf9, 0xff, 0xff, 0x87, 0xff, 0xff,
 0xff, 0xff, 0xf8, 0xff, 0xff, 0x8f, 0xfc, 0x01, 0xe7, 0x1c, 0xf9, 0xff, 0xff, 0x1f, 0xff, 0xff,
 0xff, 0xff, 0xfc, 0x3f, 0xff, 0x80, 0xff, 0x03, 0xcf, 0x3c, 0xf9, 0xff, 0xfc, 0x3f, 0xff, 0xff,
 0xff, 0xff, 0xff, 0x0f, 0xff, 0x90, 0x0f, 0xc1, 0x8e, 0x79, 0xf9, 0xff, 0xf0, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xc3, 0xff, 0x9f, 0x00, 0xf0, 0x1e, 0x79, 0xf9, 0xff, 0xc3, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xf0, 0xff, 0x9f, 0xf0, 0x18, 0x3c, 0xf9, 0xf9, 0xff, 0x0f, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xfc, 0x7f, 0x9f, 0xff, 0x00, 0x18, 0xf9, 0xf9, 0xfe, 0x1f, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xfe, 0x1f, 0x9f, 0xff, 0xe0, 0x01, 0xf3, 0xf9, 0xf8, 0x7f, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0x87, 0x9f, 0xff, 0xfe, 0x01, 0xf3, 0xf9, 0xe1, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xe1, 0x9f, 0xff, 0xff, 0xe0, 0x33, 0xf9, 0x87, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xfe, 0x03, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x87, 0xff, 0xff, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1, 0xff, 0xff, 0x87, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xfc, 0x7f, 0xe3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff,
 0xff, 0xfe, 0xf8, 0x0f, 0x80, 0xf9, 0xe1, 0xe7, 0xef, 0xf1, 0xff, 0x9f, 0x01, 0xe0, 0xff, 0xff,
 0xff, 0xfc, 0x7c, 0xe7, 0x1e, 0xf9, 0xf8, 0xe7, 0xc7, 0xf9, 0xff, 0x9f, 0x9c, 0xce, 0xff, 0xff,
 0xff, 0xfc, 0x7c, 0xe7, 0x3e, 0xf0, 0xf8, 0xe7, 0xc7, 0xf9, 0xff, 0x0f, 0x9c, 0xce, 0xff, 0xff,
 0xff, 0xf8, 0x3c, 0xc6, 0x3f, 0xf4, 0xfa, 0x67, 0x83, 0xf9, 0xff, 0x4f, 0x9c, 0xc3, 0xff, 0xff,
 0xff, 0xf9, 0x3c, 0x0e, 0x3f, 0xe4, 0x7a, 0x27, 0xb3, 0xf9, 0xfe, 0x47, 0x81, 0xe1, 0xff, 0xff,
 0xff, 0xf8, 0x1c, 0x1e, 0x3f, 0xe0, 0x7b, 0x07, 0x03, 0xf9, 0xfe, 0x07, 0x98, 0xf0, 0xff, 0xff,
 0xff, 0xf3, 0x9c, 0xce, 0x3f, 0xe6, 0x7b, 0x87, 0x39, 0xf9, 0xfe, 0x67, 0x9c, 0x5c, 0x7f, 0xff,
 0xff, 0xf7, 0x9c, 0xc7, 0x3e, 0xcf, 0x3b, 0xc7, 0x79, 0xf9, 0xfc, 0xf3, 0x9c, 0x5e, 0x7f, 0xff,
 0xff, 0xe7, 0xcc, 0xe3, 0x1c, 0xdf, 0x3b, 0xe6, 0x78, 0xf9, 0xfd, 0xf3, 0x9c, 0xce, 0x7f, 0xff,
 0xff, 0xef, 0xc4, 0xf1, 0x81, 0x9f, 0x9b, 0xe6, 0xfc, 0x78, 0x09, 0xf9, 0x81, 0xe0, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

#endif /* LIB_DISPLAY_SPLASH_H_ */