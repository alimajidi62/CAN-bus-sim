// testcpp20.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <functional>
#include <iostream>
#include <string>
#include <type_traits>
#include <iostream>
#include <string>
#include <tuple>
#include <array>
#include <optional>
#include <thread>

#include "TestTuple.h"
using namespace std;
import childmodule;
import testIncpp;
import CoroutineTest;
import compare_3_way;
import ConstTest;
import testIncpp;
import OldTestAndTemplate;
import VariantTest;
import STDExecution;
import AtomicM;
import GreedyActivity;
int main()
{
	{
		cout << "\033[1;32m ****** OLD Test ****** \033[0m \n";
		int* s11 = new int();
		*s11 = 10;
		delete s11;
		s11 = nullptr;
		(s11) ? *s11 = 22 : NULL;
		for (int i = 0; i < 10; i++);
		{
			cout << "to_string()";
		}
		CNDCSimpleVal<int> s;
		s.GetDisplayValue();
		int a = 1; int b = 12; string op;
		auto my_lambda = [=]() -> double {
			if (op == "sum") {
				return a + b;
			}
			else {
				return (a + b) / 2.0;
			}
			};
		auto t = my_lambda();

		cout << testFunctionInFunction();

		/*int *testPointer;
		if (testPointer != NULL)
			*testPointer = 10;*/
		s1 Punk1;
		s1 Punk2;
		if (Punk1 == Punk2)
			cout << "equal";
		int* in1 = new int();
		int* in2 = new int();
		*in1 = 10;
		*in2 = 20;
		in1 = in2;
		cout << *in2;
		string I = "ss";
		auto* p = new tetsTemplate(I, 1.2);
		auto* p1 = new tetsTemplate(I, 1.3);;
		delete p1;
		p1 = NULL;
		if (*p == (double)1.1)
		{
			cout << endl << p->gcd(I, 1.23) << endl;
		}
		else
		{
			cout << endl << p1->gcd(I, 1.23) << endl;
		}

		MyNumber sM1(10);
		MyNumber sM3(10);
		auto Sm2 = sM1 / sM3;
		funcTemplate(sM1);
		funcTemplate(10.25);
		functinAddable("12.1");
		string t1 = "s";
		f4Template< 1, 2, 3>(1, 5, 6);              // NonTypePack are integers and args are integers
		f4Template<'a', 'b'>(std::string("hello"), std::string("world")); // NonTypePack are characters and args are strings
		cout << "\033[1;32m ****** OLD Test ****** \033[0m \n";
	}
	cout << "\033[1;32m ****** Using Tuple ****** \033[0m \n";
	auto* test_tuple = new TestTuple();
	cout << "\033[1;32m ********* Using Module *********\033[0m \n";
	moduleTestClass* localAdd = new moduleTestClass(1, 2, 3);
	cout << "result of function add=" << localAdd->addMudule(1, 2) << endl;
	cout << "Result module in cpp" << AddIncpp(2, 2222) << endl;
	cout << "\033[1;32m ****** Using Coroutines ****** \033[0m \n";
	foo1_coroutine();
	foo2_coroutine();

	while (!task_queue_coroutine.empty()) {
		auto task = task_queue_coroutine.front();
		if (!task()) {
			task_queue_coroutine.push(task);
		}
		task_queue_coroutine.pop();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	cout << "\033[1;32m ****** Using Three-way comparison ****** \033[0m \n";
	OperatorTest op1(1,2);
	OperatorTest op2(3,4);
	auto CompareResult = op2 <=> op1;
	if (CompareResult == strong_ordering::less)
		cout << "strong_ordering::less";
	else if (CompareResult == strong_ordering::equal)
		cout << "strong_ordering::equal";
	else if (CompareResult == strong_ordering::greater)
		cout << "strong_ordering::greater";
	else
		cout << "strong_ordering::equivalent";
	cout << "\033[1;32m ****** Using Const in different way ****** \033[0m \n";
	ConstInDifferentUsage constInDifferentUsage;
	constInDifferentUsage.NotConstFunction();
	int NonconstInt1 = 12;
	const int  ConstOut/*=11; ConstOut  can not assigned after initial const variable*/ = constInDifferentUsage.ConstInConstOut(NonconstInt1);
	//ConstOut++;
	cout << ConstOut << endl;
	cout << "\033[1;32m ****** Using header file in Module ****** \033[0m \n";
	TestClassInHeaderInModule();
	cout << "\033[1;32m ****** Test Variant ****** \033[0m \n";
	TestVarient();
	unordered_map</*function name*/string, vector<pair</*SM version*/string, pair<bool, std::optional<int>>>>> m_sMgapMatrix;
	m_sMgapMatrix.clear();
	vector<pair</*SM version*/string, pair<bool, std::optional<int>>>> temp;
	temp.push_back(make_pair("Classic", make_pair(true, 10)));
	temp.push_back(make_pair("SAD", make_pair(true, 11)));
	temp.push_back(make_pair("SMPP", make_pair(true, 111)));
	m_sMgapMatrix["f1"] = temp;
	temp.clear();
	temp.push_back(make_pair("S1", make_pair(true, 111)));
	temp.push_back(make_pair("S2", make_pair(true, 111)));
	m_sMgapMatrix["f2"] = temp;
	m_sMgapMatrix["f3"] = temp;
	auto tempMatrix = m_sMgapMatrix[""];
	cout << "\033[1;32m ****** Test std::execution ****** \033[0m \n";
	Execution_parallel();
	Execution_Seq();
	Execution_Unseq();
	Execution_Par_Unseq();
	cout << "\033[1;32m ****** Test std::atomic ****** \033[0m \n";
	AtomicM();


	cout << "\033[1;32m ****** Greedy Activity ****** \033[0m \n";
	int s[] = { 1, 3, 0, 5, 8, 5 };
	int f[] = { 2, 4, 6, 7, 9, 9 };
	int n = sizeof(s) / sizeof(s[0]);
	GreedyActivity(s, f, n);
	cout <<"\n\n\n\n"<<"\033[1;34m ****** END ****** \033[0m \n";
}
