from typing import List
import bisect

class Solution:
    def avoidFlood(self, rains: List[int]) -> List[int]:
        n = len(rains)
        result = [-1] * n
        full_lakes = {}  # lake_id -> day when it became full
        dry_days = []    # list of available dry days
        
        for i in range(n):
            if rains[i] == 0:
                # Dry day - add to available dry days
                dry_days.append(i)
            else:
                lake = rains[i]
                if lake in full_lakes:
                    # Lake is already full, need to find a dry day to prevent flood
                    # Find the first dry day after the lake became full
                    last_full_day = full_lakes[lake]
                    
                    # Use binary search to find the first dry day after last_full_day
                    idx = bisect.bisect_right(dry_days, last_full_day)
                    
                    if idx == len(dry_days):
                        # No dry day available after the lake became full
                        return []
                    
                    # Use this dry day to dry the lake
                    dry_day = dry_days[idx]
                    result[dry_day] = lake
                    dry_days.pop(idx)
                    
                    # Remove lake from full_lakes as it's now dried
                    del full_lakes[lake]
                
                # Lake becomes full (either first time or after being dried)
                full_lakes[lake] = i
        
        # Fill remaining dry days with any lake (or 1 if no lakes to dry)
        for day in dry_days:
            result[day] = 1  # Can dry any lake, even empty ones
        
        return result

# Test cases
if __name__ == "__main__":
    solution = Solution()
    
    # Test case 1
    rains1 = [1,2,3,4]
    print(f"Input: {rains1}")
    print(f"Output: {solution.avoidFlood(rains1)}")
    print()
    
    # Test case 2
    rains2 = [1,2,1,0,2,1]
    print(f"Input: {rains2}")
    print(f"Output: {solution.avoidFlood(rains2)}")
    print()
    
    # Test case 3
    rains3 = [1,2,0,1,2]
    print(f"Input: {rains3}")
    print(f"Output: {solution.avoidFlood(rains3)}")
    print()
    
    # Additional test case
    rains4 = [1,0,2,0,2]
    print(f"Input: {rains4}")
    print(f"Output: {solution.avoidFlood(rains4)}")