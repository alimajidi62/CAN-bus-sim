export module testIncpp;
#include "TestCalssInModule.h"
export int AddIncpp(int a, int b) { return a + b; }
export void TestClassInHeaderInModule()
{
	auto s= TestCalssInModule::getinstance();
	s.printStatus();

}