export module GreedyActivity;
import <iostream>;
import <queue>;
using namespace std;
export void GreedyActivity(int s[], int f[], int n)
{
    cout << "Following activities are selected" << endl;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> p;
	for (int l=0;l<n;l++)
    {
        p.push(make_pair(f[l], s[l]));
    }
    auto it=p.top();
    int start = it.second;
    int end = it.first;
    p.pop();
    cout << "{" << start <<","<< end << "},";
    while (!p.empty())
    {
        auto itr = p.top();
        p.pop();
        if (itr.second>=end)
        {
            start = itr.second;
            end = itr.first;
            cout << "{" << start << "," << end << "},";
        }
    }

}