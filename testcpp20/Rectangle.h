#pragma once
class Rectangle
{
	int length;
	int breadth;
public:
	Rectangle(int l = 1, int b = 1)
	{
		length = l;
		breadth = b;
	}
	int area()
	{
		return length * breadth;
	}
};

