#include "TestTuple.h"
#include <iostream>
#include <tuple>
#include <vector>
using namespace std;
int add(int first, int second) { return first + second; }
template<typename T>
void CreatTuple(T value)
{
	int a[std::tuple_size_v<T>];
	std::cout << std::tuple_size<T>{} << ' ' // or at run time
		<< sizeof a << ' '
		<< sizeof value << '\n';
	std::apply([](const auto&... elems) {
		((std::cout << elems << " "), ...);
		}, value);
	vector<pair<int, int>> t;
	t.push_back(make_pair(1, 2));
	t.push_back(make_pair(3, 4));
	t.push_back(make_pair(5, 6));
	std::cout << std::apply(add, make_pair(1, 2)) << '\n';
}
TestTuple::TestTuple()
{
	tuple<string, string, string> geek;
	tuple<string, string, string> geek2;
	geek = std::make_tuple("1", "20", "112");
	geek2 = std::make_tuple("1", "3", "12");
	if(geek>geek2)
	{
		geek2 = geek;
		std::cout << get<0>(geek)<<get<1>(geek)<<get<2>(geek)<<endl;
	}
	if(geek < geek2)
	{
		geek2 = geek;
		std::cout << get<0>(geek) << get<1>(geek) << get<2>(geek) << endl;
	}
	if(geek==geek2)
	{
		geek2 = geek;
		std::cout << get<0>(geek) << get<1>(geek) << get<2>(geek) << endl;
	}
	cout << "****************************"<<endl;
	CreatTuple(make_tuple(1, 2, "s", 12, 1.2222));

}
