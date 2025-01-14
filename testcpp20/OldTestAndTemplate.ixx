export module OldTestAndTemplate;
import <functional>;
import <iostream>;
import <string>;
import <type_traits>;
import <iostream>;
import <string>;
import <tuple>;
import <array>;
import <thread>;
using namespace std;
class  CNDCFormat
{
public:

	static string ToString(int n)
	{
		string str;
		printf("(int n)");
		return str;
	}
	static string ToString(unsigned long n)
	{
		string str;
		printf("(unsigned long n)");
		return str;
	}
	static string ToString(double d)
	{
		string str;
		printf("(double d)");
		return str;
	}
};
class INDCVal
{
public:

	virtual string GetDisplayValue() const = 0;
};

export template<class T>
 class CNDCSimpleVal :INDCVal
{
public:
	virtual string GetDisplayValue() const
	{
		return CNDCFormat::ToString(m_tData);
	}
private:
	T m_tData;
};

export class s1
{
public:
	s1()
	{

	}
	int s;
	char* m_layerName;
	bool operator==(const s1& other) const
	{
		return true;
	}
private:
	void tt()
	{

	}
private:
	void tt2()
	{

	}
};

class s2
{
public:
	s2()
	{

	}
	void creat(s1& l)
	{
		l.s++;
	}
	void test()
	{

	}
	const char* m_layerName;
	double	m_dDoneItems;
	void test1(s1 = s1())
	{
		this->test();
	}
};
void getnumber(int s)
{
	printf("test");
}
export int testFunctionInFunction()
{
	function f = [&]() -> int
		{
			return 12;
		};
	cout << endl << "Function in Function called  " << f() << endl;
	return  f();
}
template<typename T>
concept Integral = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_same_v<T, std::string>;

template<typename T>
concept FloatPoint = std::is_floating_point_v<T>;
template<typename T>
concept StringInput = std::is_same_v<T, std::string>;
export template <Integral T, FloatPoint U>
class tetsTemplate
{
public:
	T first;
	U Secound;
	tetsTemplate(T a, U b) : first(a), Secound(b) {};
	T gcd(T a, U b)
	{
		return a;
	}
	bool operator==(const tetsTemplate& other) const {
		return first == other.first && Secound == other.Secound;
	}
	bool operator==(const double other) const {
		return  Secound == other;
	}
};
// Concepts definition
template<typename T>
concept Addable = requires(T a, T b)
{
	a + b;
};

template<typename T>
concept Dividable = requires(T a, T b)
{
	a / b;
};

template<typename T>
concept DivAddable = Addable<T> && Dividable<T>;

export template<typename T>
void funcTemplate(T x)
{
	if constexpr (Addable<T>) {
		std::cout << "Type is Addable" << std::endl;
	}
	else if constexpr (requires(T a, T b) { a + b; }) {
		std::cout << "Type supports addition (checked with requires)" << std::endl;
	}

	if constexpr (Dividable<T>) {
		std::cout << "Type is Addable" << std::endl;
	}
	else if constexpr (requires(T a, T b) { a / b; }) {
		std::cout << "Type supports addition (checked with requires)" << std::endl;
	}
	else {
		std::cout << "Type is neither Addable nor supports addition" << std::endl;
	}

}

template<typename T> /*...*/
concept Ssss = requires (T x) // optional set of fictional parameter(s)
{
	// simple requirement: expression must be valid
	x++;    // expression must be valid

	// type requirement: `typename T`, T type must be a valid type
	typename T::value_type;
	typename funcTemplate<T>;

	// compound requirement: {expression}[noexcept][-> Concept];
	// {expression} -> Concept<A1, A2, ...> is equivalent to
	// requires Concept<decltype((expression)), A1, A2, ...>
	{*x};  // dereference must be valid
	{*x} noexcept;  // dereference must be noexcept
	// dereference must  return T::value_type
	{*x} noexcept -> std::same_as<typename T::value_type>;

	// nested requirement: requires ConceptName<...>;
		requires Addable<T>; // constraint Addable<T> must be satisfied
};
export struct MyNumber
{
	int Value;
	/*MyNumber operator+(const MyNumber Other) const
	{
		return MyNumber(Value + Other.Value);
	}*/
	MyNumber operator/(const MyNumber other) const
	{
		return  MyNumber(Value / other.Value);
	}
};
export void functinAddable(auto x)
{

}
template<Addable auto NonTypeParameter, Addable TypeParameter>
Addable auto f3(Addable auto x, auto y)
{
	// notice that nothing is allowed between constraint name and `auto`
	Addable auto z = 0;
	return 0;
}
#include <iostream>
#include <string>
#include <tuple>
#include <array>
#include <type_traits>

// Helper function to create a tuple from a parameter pack
template<typename... Args>
auto create_tuple(Args&&... args)
{
	return std::make_tuple(args ...);
}

// Concept to accept integers, floating points, and std::arrays for string literals
template<typename T>
concept AcceptEverything = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_same_v<T, std::string> || requires (T t) { t.size(); };

// Helper function to print std::array<char, N>
template<std::size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<char, N>& arr)
{
	for (const char& c : arr)
	{
		if (c == '\0') break;
		os << c;
	}
	return os;
}

// Define the f4 function template
export template<AcceptEverything auto... NonTypePack, AcceptEverything... TypePack>
void f4Template(TypePack... args)
{
	// Access and print elements in NonTypePack
	std::cout << "NonTypePack elements: ";
	((std::cout << NonTypePack << " "), ...);
	std::cout << std::endl;

	// Create a tuple from TypePack to access elements by index
	auto typePackTuple = create_tuple(args...);
	auto typePackTuple2 = create_tuple(NonTypePack...);

	// Access and print elements in TypePack
	std::cout << "TypePack elements: ";
	std::apply([](const auto&... elems) {
		((std::cout << elems << " "), ...);
		}, typePackTuple);
	std::cout << std::endl;
}