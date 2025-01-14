export module ConstTest;
import <iostream>;
using namespace std;
export class ConstInDifferentUsage
{
public:
	ConstInDifferentUsage() {};
	mutable int* m_var1;
	int m_var2;
	void NotConstFunction()
	{
		int x{ 10 };
		char y{ 'M' };

		const int* i = &x;
		const char* j = &y;

		// Value of x and y can be altered,
		// they are not constant variables
		x = 9;
		y = 'A';

		// Change of constant values because,
		// i and j are pointing to const-int
		// & const-char type value
		// *i = 6;
		// *j = 7;

		cout << *i << " " << *j<< endl;
		cout << *(++i) <<" "<< *(++j) << endl;
		// *i = 12; can not do this
		m_var1 = &x;
		//*m_var1 = 1;
	}
	const int ConstInConstOut(const int a)
	{
		return a + 1;
	}
	void ConstFunc() const
	{
		*m_var1 = 12;
		//m_var2 = 1; can not because this function is const and m_var2 not mutable
	}

};
