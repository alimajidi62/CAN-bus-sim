#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
using namespace std;

class FoodRatings {
    unordered_map<string, string> foodToCuisine;
    unordered_map<string, int> foodToRating;
    unordered_map<string, map<int, set<string>>> cuisineRatings;

public:
    FoodRatings(vector<string>& foods, vector<string>& cuisines, vector<int>& ratings) {
        int n = foods.size();
        for (int i = 0; i < n; ++i) {
            foodToCuisine[foods[i]] = cuisines[i];
            foodToRating[foods[i]] = ratings[i];
            cuisineRatings[cuisines[i]][ratings[i]].insert(foods[i]);
        }
    }
    
    void changeRating(string food, int newRating) {
        string cuisine = foodToCuisine[food];
        int oldRating = foodToRating[food];
        // Remove from old rating set
        auto& ratingSet = cuisineRatings[cuisine][oldRating];
        ratingSet.erase(food);
        if (ratingSet.empty()) cuisineRatings[cuisine].erase(oldRating);
        // Add to new rating set
        cuisineRatings[cuisine][newRating].insert(food);
        foodToRating[food] = newRating;
    }
    
    string highestRated(string cuisine) {
        auto& ratingsMap = cuisineRatings[cuisine];
        auto it = ratingsMap.rbegin(); 
        return *(it->second.begin()); // lex smallest food name
    }
};