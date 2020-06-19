Check "Calc_Schematic.jpg" to know the connections of the LCD and the Keypad.


The code was tested on Proteus as like in the picture and on a hardware kit with the same connections for all the components.


MAKE SURE TO DOWNLOAD ALL FOLDERS AND INCLUDE THEM IN DIRECTORIES TO AVOID ERRORS.


The calculator variables that carry the data are char (8bit) so keep in my mind the value to stay in range to avoid over flow, if you wish to change them make sure you change the 2's complement part as well at the end of "calc.c"


The function "func_calc_seq()" doesn't work with negative numbers and doesn't follow the BODMAS rule of basic mathematical operations.
The function "func_calc_bdmas()" can handle negative number and follows the BODMAS rule. KEEP IN MIND THE DATA TYPE RANGE AS MENTIONED BEFORE!
