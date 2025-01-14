export module AtomicM;
import <atomic>;
import <vector>;
import <thread>;
import <iostream>;
std::atomic<int> counter(0);
std::vector<std::atomic<int>> Avector;
void increment(int num)
{
	for (int i=0;i<num;i++)
	{
		++counter;
		///Avector.
	}
}
export void AtomicM()
{
	const int numThread = 10;
	const int numIncremnt = 1000;
	std::vector<std::thread> Threads;
	for (int i=0;i<numThread;i++)
	{
		Threads.emplace_back(increment, numIncremnt);
	}
	for (auto&t:Threads)
	{
		t.join();
	}
	std::cout << "final counter value:" << counter.load() << std::endl;

}