#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

struct FoodEntry {
    int rating;
    string name;
    bool operator<(const FoodEntry& other) const {
        if (rating != other.rating) return rating < other.rating; // max-heap by rating
        return name > other.name; // min-heap by name for ties
    }
};

class FoodRatings {
    unordered_map<string, string> foodToCuisine;
    unordered_map<string, int> foodToRating;
    unordered_map<string, priority_queue<FoodEntry>> cuisineHeap;

public:
    FoodRatings(vector<string>& foods, vector<string>& cuisines, vector<int>& ratings) {
        int n = foods.size();
        for (int i = 0; i < n; ++i) {
            foodToCuisine[foods[i]] = cuisines[i];
            foodToRating[foods[i]] = ratings[i];
            cuisineHeap[cuisines[i]].push({ ratings[i], foods[i] });
        }
    }

    void changeRating(string food, int newRating) {
        string cuisine = foodToCuisine[food];
        foodToRating[food] = newRating;
        cuisineHeap[cuisine].push({ newRating, food });
    }

    string highestRated(string cuisine) {
        auto& heap = cuisineHeap[cuisine];
        while (true) {
            FoodEntry top = heap.top();
            if (foodToRating[top.name] == top.rating) return top.name;
            heap.pop(); // Remove outdated entry
        }
    }
};