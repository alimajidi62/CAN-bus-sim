// ...existing code...
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

class FoodRatings {
    std::unordered_map<std::string, std::string> foodToCuisine;
    std::unordered_map<std::string, int> foodToRating;
    // For each cuisine, a max-heap of (rating, name)
    std::unordered_map<std::string, std::priority_queue<std::pair<int, std::string>>> cuisineHeap;

public:
    FoodRatings(std::vector<std::string>& foods, std::vector<std::string>& cuisines, std::vector<int>& ratings) {
        int n = foods.size();
        for (int i = 0; i < n; ++i) {
            foodToCuisine[foods[i]] = cuisines[i];
            foodToRating[foods[i]] = ratings[i];
            cuisineHeap[cuisines[i]].push({ratings[i], foods[i]});
        }
    }

    void changeRating(std::string food, int newRating) {
        std::string cuisine = foodToCuisine[food];
        foodToRating[food] = newRating;
        cuisineHeap[cuisine].push({newRating, food});
    }

    std::string highestRated(std::string cuisine) {
        auto& heap = cuisineHeap[cuisine];
        while (true) {
            auto [rating, name] = heap.top();
            if (foodToRating[name] == rating) return name;
            heap.pop(); // Remove outdated entry
        }
    }
};