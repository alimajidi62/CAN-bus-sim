
export module childmodule;

import parentModule;
import <iostream>;
import <vector>;
import <algorithm>;
import <array>;
using namespace std;
export  class moduleTestClass:Interface
{
public:
	moduleTestClass(int a, int b, int c) :m_a(a),m_b(b),m_c(c)
	{
		
	}
	template <typename T, std::size_t... Indices>
	auto vectorToTupleHelper(const std::vector<T>& v, std::index_sequence<Indices...>) {
		return std::make_tuple(v[Indices]...);
	}

	template <std::size_t N, typename T>
	auto vectorToTuple(const std::vector<T>& v) {
		assert(v.size() >= N);
		return vectorToTupleHelper(v, std::make_index_sequence<N>());
	}
	int addMudule (int a, int b) override
	{
		cout << "\ninclude STD"<<endl;
		std::vector<int> v{ 7, 0, 4, 23, -1 };
		std::ranges::sort(v); // constrained algorithm
		for_each(v.cbegin(), v.cend(), [&](const int & element){
			cout << element << " ";
		});
		
		return a + b;
	}
	void Fun() override {}
private:
	int m_a;
	int m_b;
	int m_c;
};

