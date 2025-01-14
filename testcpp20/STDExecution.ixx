export module STDExecution;
import <algorithm>;
import <execution>;
import <vector>;
import <iostream>;
export void Execution_parallel()
{
	std::vector<int> vec = {1, 2, 3, 4, 5};
	std::cout << "std::execution::par:  ";
	std::for_each(std::execution::par, vec.begin(), vec.end(), [&](int n)
	{
		if (n == 2)
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << n*2 << " ";
	});
	std::cout << "\n";
	int x = 0;
	std::mutex m;
	int a[] = { 1,2 };
	std::vector<int> vs;
	std::for_each(std::execution::par, std::begin(a), std::end(a), [&](int i)
	{
			std::lock_guard<std::mutex> guard(m);
			vs.push_back(i * 2 + 1);
			++x;
			std::cout << x;
	});
	std::cout << "\n";
}
export void Execution_Seq()
{
	std::vector<int> vec = { 1, 2, 3, 4, 5 };
	std::cout << "std::execution::seq:  ";
	std::for_each(std::execution::seq, vec.begin(), vec.end(), [&](int n)
		{
			if (n == 2)
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			std::cout << n * 2 << " ";
		});
	std::cout << "\n";
}
export void Execution_Unseq()
{
	std::vector<int> vec = { 1, 2, 3, 4, 5 };
	std::cout << "std::execution::unseq:  ";
	std::for_each(std::execution::unseq, vec.begin(), vec.end(), [&](int n)
		{
			if (n == 2)
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			std::cout << n * 2 << " ";
		});
	std::cout << "\n";
}
export void Execution_Par_Unseq()
{
	int a[] = { 1,2 };
	std::mutex m;
	int x = 0;
	std::cout << "std::execution::Par_unseq:  ";
	std::for_each(std::execution::par_unseq, std::begin(a), std::end(a), [&](int n)
		{
			std::lock_guard<std::mutex> guard(m);
			x++;
		});
	std::cout << "\n";
}