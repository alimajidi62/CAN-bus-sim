export module compare_3_way;

import <compare>;
using namespace std;
export class OperatorTest
{
public:
	OperatorTest(): x(0), y(0)
	{
	}

	int x;
	int y;
	strong_ordering operator<=>(const OperatorTest& other) const
	{
		if (auto cmp = x <=> other.x; cmp != 0)
			return cmp;
		return y <=> other.y;
	}

};
