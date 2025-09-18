#include <string>
#include <vector>
#include <unordered_map>
#include <set>
using namespace std;

class FoodRatings {
    // Map food name to its cuisine
    unordered_map<string, string> foodToCuisine;
    // Map food name to its rating
    unordered_map<string, int> foodToRating;
    // For each cuisine, store foods sorted by (rating desc, name asc)
    unordered_map<string, set<pair<int, string>>> cuisineFoods;

public:
    FoodRatings(vector<string>& foods, vector<string>& cuisines, vector<int>& ratings) {
        int n = foods.size();
        for (int i = 0; i < n; ++i) {
            foodToCuisine[foods[i]] = cuisines[i];
            foodToRating[foods[i]] = ratings[i];
            cuisineFoods[cuisines[i]].insert({-ratings[i], foods[i]});
        }
    }
    
    void changeRating(string food, int newRating) {
        string cuisine = foodToCuisine[food];
        int oldRating = foodToRating[food];
        // Remove old entry
        cuisineFoods[cuisine].erase({-oldRating, food});
        // Insert new entry
        cuisineFoods[cuisine].insert({-newRating, food});
        foodToRating[food] = newRating;
    }
    
    string highestRated(string cuisine) {
        // The first element in the set is the highest rated (rating desc, name asc)
        return cuisineFoods[cuisine].begin()->second;
    }
};