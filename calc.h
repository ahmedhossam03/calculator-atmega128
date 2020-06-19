#ifndef CALC_H_
#define CALC_H_

#include "lcd/lcd.h"
#include "keypad/keypad.h"

void calc_init();
void disp_calc(uint8);
void func_calc_seq();
void func_calc_bdmas();

#endif /* CALC_H_ */