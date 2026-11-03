#pragma once

class Math
{
public:
	//math helpers for negative numbers
	//we need floor division for division that always rounds down
	//floor division helps us find which chunk we are in
	//are static because theyre pure functions of inputs
	inline static int floorDiv(int a, int b)
	{
		int result = a / b; //normal a/b
		if (a < 0 && (a % b != 0)) //if a is negative and a%b != 0 
		{
			result -= 1; //subtract one
		}
		return result; //return result
	}

	//and positive modulo for figuring out where we are inside the chunk
	inline static int positiveModulo(int a, int b)
	{
		return ((a % b) + b) % b;
	}
};