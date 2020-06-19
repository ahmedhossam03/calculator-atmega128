#include "calc.h"

int main(void)
{
	calc_init();
    while(1)
    {
		//lcd_send_itoa(key_wait_pressed());
        func_calc_bdmas();
    }
}