#include "calc.h"

void calc_init()
{
	lcd_init();
	keypad_init();
}

void disp_calc(uint8 key) //display input data
{
	if(key == 'c') //clear screen
	{
		lcd_send_cmd(LCD_CLR);
		_delay_ms(2);
	}
	else if(key > 9 || key == '0') //display numbers from 0-9 
	{
		lcd_send_char(key);
	}
	else if(key <= 9) //display operations +-*/
	{
		lcd_send_char(key + '0');
	}
}

uint8 get_str(uint8 str[])	//put all inputs in array
{
	uint8 ind = 0;
	uint8 c_flag = 1;
	uint8 key = key_wait_pressed();
	while(key != '=' && ind < 16 && c_flag == 1)
	{
		disp_calc(key);		//display the input
		if(key == '0')		//convert '0' to 0
			key = 0;
		if(key == 'c')		//if c is pressed stop the loop and return 0
			c_flag = 0;
		str[ind] = key;		//put input in array
		ind++;
		key = key_wait_pressed();
	}
	if(key == '=')
	{
		str[ind] = key;		//put '=' in the last element of array
	}
	return c_flag;
}

uint8 str2data(uint8 str[], uint8 ind, sint8 *num)		//convert inputs to numbers
{
	*num = 0;
	uint8 x = 0;
	uint8 neg_flag = 0;
	
	if(str[ind] == '-')		//check the input if negative
	{
		ind = ind + 1;		//jump to the next element to start at number
		neg_flag = 1;		//raise negative flag
	}
	
	while(str[ind] <= 9 && ind < 16 && str[ind] != '=')		//convert input to one number
	{
		(x) = ( (x) * 10 ) + str[ind];
		ind++;
	}
	
	if(neg_flag == 1)		//check if negative flag is raised
		(*num) = (sint8)(x) * -1;		//if yes multiply output number by -1
	else
		(*num) = (sint8)x;
	
	return ind;			//return index of next operation
}

void func_calc_seq()   //sequential calculation no negative numbers or beginning with mult or div
{
	sint8 op1 = 0;
	sint8 op2 = 0;
	uint8 input_str[16] = {0};
	uint8 op_i = 0;
	
	lcd_goto_xy(0,0);
	uint8 clear_flag = get_str(input_str);		//get input array and get clear flag
	
	if(clear_flag == 1)		//if clear flag is high
		op_i = str2data(input_str, 0, &op1);		//get first operand
	while(input_str[op_i] != '=' && clear_flag == 1)		
	{
		//get the next operand, do operation and put result in op1 and restart loop until '='
		if(input_str[op_i] == '+')
		{
			op_i = str2data(input_str, op_i+1, &op2);		
			op1 = op1 + op2;	
		}
		else if(input_str[op_i] == '-')
		{
			op_i = str2data(input_str, op_i+1, &op2);
			op1 = op1 - op2;
		}
		else if(input_str[op_i] == '*')
		{
			op_i = str2data(input_str, op_i+1, &op2);
			op1 = op1 * op2;
		}
		else if(input_str[op_i] == '/')
		{
			op_i = str2data(input_str, op_i+1, &op2);
			op1 = op1 / op2;
		}
	}
	//print result
	lcd_goto_xy(0,1);
	lcd_send_itoa(op1);
}

void func_calc_bdmas()  //normal calculation, handles negative numbers and beginning with mult or div
{
	sint8 ops[10] = {0};
	uint8 input_str[16] = {0};
	uint8 opers[10] = {0};
	uint8 ops_num = 0;
	uint8 i = 0;
	uint8 j = 0;
	uint8 err = 0;
	
	lcd_goto_xy(0,0);
	for(uint8 loopCounter = 0; loopCounter < 16; loopCounter++)
	{
		lcd_send_char(' ');
	}
	lcd_goto_xy(0,0);
	uint8 clear_flag = get_str(input_str);		//get input string and get clear flag
	
	while(input_str[i] != '=' && clear_flag != 0)		//if clear flag is high
	{
		if(i == 0)    //initially
		{
			//get number put it in operands array
			i = str2data(input_str, i, &ops[j]);
			//get operation and put it in operation array
			opers[j] = input_str[i];
			j++;
		}
		else
		{
			//same as above but i+1 to start after the operation index
			i = str2data(input_str, i+1, &ops[j]);
			opers[j] = input_str[i];
			j++;
		}
		ops_num = j;		//number of operations and operands 4 operands have 3 operations and an '='
	}
	
	j = 0;		//look for multiplication and divisions first
	while(opers[j] != '=' && err == 0 && clear_flag != 0)
	{
		if(opers[j] == '*')//if multiplication
		{
			ops[j] = ops[j] * ops[j+1]; //multiply the operand at the same address as the '*' in the operations array with address after
			ops[j+1] = 0; //remove one of the operands after putting result
			opers[j] = 0; //remove operator
			
			for(i = j+1; i < ops_num - 1; i++) //swap until removed operand is at the end of operand array
			{
				sint8 temp = ops[i];
				ops[i] = ops[i+1];
				ops[i+1] = temp;
			}
			
			for(i = j; i < ops_num - 1; i++) //swap until removed operation is at the end of operation array
			{
				uint8 temp = opers[i];
				opers[i] = opers[i+1];
				opers[i+1] = temp;
			}
			
			ops_num = ops_num - 1; //decrease array size by one to get rid of removed operands and operations
		}
		else if(opers[j] == '/')  //same as multiplication
		{
			if(ops[j+1] == 0)  //raise error flag if division by zero
			{
				err = 1;
				lcd_send_cmd(LCD_CLR);
				_delay_ms(2);
				uint8 err_str[] = "ERROR!";
				lcd_send_str(err_str, sizeof(err_str)-1);
				_delay_ms(2000);
			}
			else
			{
				ops[j] = ops[j] / ops[j+1];
				ops[j+1] = 0;
				opers[j] = 0;
				
				for(i = j+1; i < ops_num - 1; i++)
				{
					sint8 temp = ops[i];
					ops[i] = ops[i+1];
					ops[i+1] = temp;
				}
				
				for(i = j; i < ops_num - 1; i++)
				{
					uint8 temp = opers[i];
					opers[i] = opers[i+1];
					opers[i+1] = temp;
				}
				
				ops_num = ops_num - 1;
			}
		}
		else //if no mult or div found move to next operation in operation array and restart the loop
		{
			j++;
		}
		
	}
	
	//after all mult and div operations are done look for addition and subtraction
	//same technique as mult and div done
	j = 0;
	while(opers[j] != '=' && err == 0 && clear_flag != 0)
	{
		if(opers[j] == '+')
		{
			ops[j] = ops[j] + ops[j+1];
			ops[j+1] = 0;
			opers[j] = 0;
			
			for(i = j+1; i < ops_num - 1; i++)
			{
				sint8 temp = ops[i];
				ops[i] = ops[i+1];
				ops[i+1] = temp;
			}
			
			for(i = j; i < ops_num - 1; i++)
			{
				uint8 temp = opers[i];
				opers[i] = opers[i+1];
				opers[i+1] = temp;
			}
			
			ops_num = ops_num - 1;
		}
		else if(opers[j] == '-')
		{
			ops[j] = ops[j] - ops[j+1];
			ops[j+1] = 0;
			opers[j] = 0;
			
			for(i = j+1; i < ops_num - 1; i++)
			{
				sint8 temp = ops[i];
				ops[i] = ops[i+1];
				ops[i+1] = temp;
			}
			
			for(i = j; i < ops_num - 1; i++)
			{
				uint8 temp = opers[i];
				opers[i] = opers[i+1];
				opers[i+1] = temp;
			}
			
			ops_num = ops_num - 1;
		}
		else
		{
			j++;
		}
	}
	
	//if there are no error or clear display result
	if(err == 0 && clear_flag != 0)
	{
		if(ops[0] >= 128) //if result was negative, 128 is 2's comp of the -128 and 255 is the 2's comp of 0
		{
			//remove the 2's comp to get the real value
			ops[0] = ops[0] - 1;
			ops[0] = ~ops[0];
			lcd_goto_xy(0,1);
			lcd_send_char('-');  //add negative sign first
			lcd_send_itoa((uint8)ops[0]);  //print real value
		}
		else  //if output is positive print result
		{
			uint8 x = 0;
			x = (uint8)ops[0];
			lcd_goto_xy(1,1);
			lcd_send_itoa(x);
		}
	}
}