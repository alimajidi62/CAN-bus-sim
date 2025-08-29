export module SemaphoreTest;

import <iostream>;
import <semaphore>;
import <mutex>;
import <thread>;
import <vector>;
import <chrono>;
import <random>;
import <queue>;

using namespace std;
using namespace std::chrono;

// C++20 Semaphore-based Resource Pool
export class SemaphoreResourcePool {
private:
    std::counting_semaphore<10> semaphore_;  // Allow up to 10 concurrent accesses
    std::queue<int> resources_;
    std::mutex queue_mutex_;  // Still need mutex for queue operations
    int next_resource_id_;

public:
    SemaphoreResourcePool(int initial_resources = 5) 
        : semaphore_(initial_resources), next_resource_id_(1) {
        
        // Initialize resource pool
        for (int i = 0; i < initial_resources; ++i) {
            resources_.push(next_resource_id_++);
        }
    }

    // Acquire a resource using semaphore
    int acquire_resource() {
        // Wait for available resource (semaphore handles the waiting)
        semaphore_.acquire();
        
        // Get resource from pool (mutex needed for queue safety)
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (resources_.empty()) {
            // Create new resource if pool is empty
            return next_resource_id_++;
        }
        
        int resource_id = resources_.front();
        resources_.pop();
        return resource_id;
    }

    // Release a resource using semaphore
    void release_resource(int resource_id) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            resources_.push(resource_id);
        }
        
        // Signal that resource is available
        semaphore_.release();
    }

    // Try to acquire with timeout
    bool try_acquire_resource(int& resource_id, std::chrono::milliseconds timeout) {
        if (semaphore_.try_acquire_for(timeout)) {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (resources_.empty()) {
                resource_id = next_resource_id_++;
            } else {
                resource_id = resources_.front();
                resources_.pop();
            }
            return true;
        }
        return false;
    }

    // Get current available resources count (approximate)
    size_t available_count() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(queue_mutex_));
        return resources_.size();
    }
};

// Traditional Mutex-based Resource Pool for comparison
export class MutexResourcePool {
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<int> resources_;
    int next_resource_id_;
    const int max_resources_;

public:
    MutexResourcePool(int initial_resources = 5, int max_resources = 10) 
        : next_resource_id_(1), max_resources_(max_resources) {
        
        for (int i = 0; i < initial_resources; ++i) {
            resources_.push(next_resource_id_++);
        }
    }

    int acquire_resource() {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Wait until resource is available
        cv_.wait(lock, [this] { 
            return !resources_.empty() || resources_.size() < max_resources_; 
        });

        if (resources_.empty()) {
            return next_resource_id_++;
        }
        
        int resource_id = resources_.front();
        resources_.pop();
        return resource_id;
    }

    void release_resource(int resource_id) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            resources_.push(resource_id);
        }
        cv_.notify_one();
    }

    bool try_acquire_resource(int& resource_id, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (cv_.wait_for(lock, timeout, [this] { 
            return !resources_.empty() || resources_.size() < max_resources_; 
        })) {
            if (resources_.empty()) {
                resource_id = next_resource_id_++;
            } else {
                resource_id = resources_.front();
                resources_.pop();
            }
            return true;
        }
        return false;
    }

    size_t available_count() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mutex_));
        return resources_.size();
    }
};

// Binary Semaphore example (like a mutex but with additional features)
export class BinarySemaphoreExample {
private:
    std::binary_semaphore semaphore_;
    int shared_data_;

public:
    BinarySemaphoreExample() : semaphore_(1), shared_data_(0) {}  // Initially available

    void increment_data() {
        semaphore_.acquire();  // Lock
        ++shared_data_;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Simulate work
        semaphore_.release();  // Unlock
    }

    void decrement_data() {
        semaphore_.acquire();  // Lock
        --shared_data_;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Simulate work
        semaphore_.release();  // Unlock
    }

    int get_data() const {
        return shared_data_;
    }

    // Try to acquire with timeout
    bool try_modify_data(int delta, std::chrono::milliseconds timeout) {
        if (semaphore_.try_acquire_for(timeout)) {
            shared_data_ += delta;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            semaphore_.release();
            return true;
        }
        return false;
    }
};

// Demonstration functions
export void demonstrate_semaphore_vs_mutex() {
    std::cout << "\n=== C++20 Semaphore vs Mutex Demonstration ===\n";

    // Test Semaphore Resource Pool
    std::cout << "\n--- Testing Semaphore Resource Pool ---\n";
    SemaphoreResourcePool sem_pool(3);  // Start with 3 resources, max 10
    
    std::vector<std::thread> sem_threads;
    std::atomic<int> sem_completed{0};

    auto sem_worker = [&sem_pool, &sem_completed](int worker_id) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(100, 500);

        for (int i = 0; i < 3; ++i) {
            int resource_id = sem_pool.acquire_resource();
            std::cout << "Worker " << worker_id << " acquired resource " << resource_id << std::endl;
            
            // Simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
            
            sem_pool.release_resource(resource_id);
            std::cout << "Worker " << worker_id << " released resource " << resource_id << std::endl;
        }
        sem_completed++;
    };

    // Start semaphore test
    auto sem_start = high_resolution_clock::now();
    for (int i = 0; i < 5; ++i) {
        sem_threads.emplace_back(sem_worker, i + 1);
    }

    for (auto& t : sem_threads) {
        t.join();
    }
    auto sem_duration = duration_cast<milliseconds>(high_resolution_clock::now() - sem_start);

    std::cout << "Semaphore test completed in " << sem_duration.count() << "ms\n";

    // Test Mutex Resource Pool
    std::cout << "\n--- Testing Mutex Resource Pool ---\n";
    MutexResourcePool mutex_pool(3, 10);
    
    std::vector<std::thread> mutex_threads;
    std::atomic<int> mutex_completed{0};

    auto mutex_worker = [&mutex_pool, &mutex_completed](int worker_id) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(100, 500);

        for (int i = 0; i < 3; ++i) {
            int resource_id = mutex_pool.acquire_resource();
            std::cout << "Worker " << worker_id << " acquired resource " << resource_id << std::endl;
            
            // Simulate work
            std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
            
            mutex_pool.release_resource(resource_id);
            std::cout << "Worker " << worker_id << " released resource " << resource_id << std::endl;
        }
        mutex_completed++;
    };

    // Start mutex test
    auto mutex_start = high_resolution_clock::now();
    for (int i = 0; i < 5; ++i) {
        mutex_threads.emplace_back(mutex_worker, i + 1);
    }

    for (auto& t : mutex_threads) {
        t.join();
    }
    auto mutex_duration = duration_cast<milliseconds>(high_resolution_clock::now() - mutex_start);

    std::cout << "Mutex test completed in " << mutex_duration.count() << "ms\n";
    
    std::cout << "\nPerformance comparison:\n";
    std::cout << "Semaphore: " << sem_duration.count() << "ms\n";
    std::cout << "Mutex: " << mutex_duration.count() << "ms\n";
}

export void demonstrate_binary_semaphore() {
    std::cout << "\n=== Binary Semaphore Demonstration ===\n";
    
    BinarySemaphoreExample example;
    std::vector<std::thread> threads;

    // Create threads that increment and decrement
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&example, i]() {
            for (int j = 0; j < 10; ++j) {
                if (j % 2 == 0) {
                    example.increment_data();
                    std::cout << "Thread " << i << " incremented data\n";
                } else {
                    example.decrement_data();
                    std::cout << "Thread " << i << " decremented data\n";
                }
            }
        });
    }

    // Thread that tries to modify with timeout
    threads.emplace_back([&example]() {
        for (int i = 0; i < 5; ++i) {
            if (example.try_modify_data(10, std::chrono::milliseconds(50))) {
                std::cout << "Timeout thread successfully modified data by +10\n";
            } else {
                std::cout << "Timeout thread failed to acquire semaphore\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final data value: " << example.get_data() << std::endl;
}

export void semaphore_performance_test() {
    std::cout << "\n=== Semaphore Performance Test ===\n";
    
    const int num_operations = 1000;
    const int num_threads = 8;
    
    // Test with different semaphore capacities
    std::vector<int> capacities = {1, 2, 4, 8};
    
    for (int capacity : capacities) {
        std::cout << "\nTesting with semaphore capacity: " << capacity << std::endl;
        
        std::counting_semaphore<8> semaphore(capacity);
        std::atomic<int> operations_completed{0};
        
        auto start_time = high_resolution_clock::now();
        
        std::vector<std::thread> threads;
        for (int t = 0; t < num_threads; ++t) {
            threads.emplace_back([&semaphore, &operations_completed, num_operations]() {
                for (int i = 0; i < num_operations / num_threads; ++i) {
                    semaphore.acquire();
                    
                    // Simulate some work
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                    operations_completed++;
                    
                    semaphore.release();
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start_time);
        std::cout << "Capacity " << capacity << ": " << duration.count() 
                  << " microseconds (" << operations_completed.load() << " operations)\n";
    }
}
