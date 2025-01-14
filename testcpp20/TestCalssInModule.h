#pragma once
import <iostream>;

class TestCalssInModule
{
public:
	static TestCalssInModule getinstance()
	{
		return TestCalssInModule();
	}
	void printStatus()
	{
		std::cout << "instance created\n";
	}

private:
	TestCalssInModule() {};
};

