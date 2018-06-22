#include "Utilities.h"

int constrain(int num, int min, int max){
	if(num > max)
		num = max;
	else if(num < min)
		num = min;

	return num;
}
