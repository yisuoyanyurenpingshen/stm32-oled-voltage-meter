#ifndef __OLED_H
#define __OLED_H

#include "stdint.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_SetCursor(uint8_t page, uint8_t x);
void OLED_ShowChar(uint8_t page, uint8_t x, char ch);
void OLED_ShowString(uint8_t page, uint8_t x, char *str);

#endif
