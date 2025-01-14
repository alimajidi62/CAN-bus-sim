export module CoroutineTest;
import <iostream>;
import <coroutine>;
import <thread>;
import <queue>;
import <functional>;

export std::queue<std::function<bool()>> task_queue_coroutine;

struct sleep {
    sleep(int n) : delay{ n } {}

    constexpr bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) const noexcept {
        auto start = std::chrono::steady_clock::now();
        task_queue_coroutine.push([start, h, d = delay] {
            if (decltype(start)::clock::now() - start > d) {
                h.resume();
                return true;
            }
            else {
                return false;
            }
            });
    }

    void await_resume() const noexcept {}

    std::chrono::milliseconds delay;
};
struct task
{
    struct promise_type
    {
        task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };
};
export task foo1_coroutine() noexcept {
    std::cout << "1. hello from foo1_coroutine" << std::endl;
    for (int i = 0; i < 10; ++i) {
        co_await sleep{ 10 };
        std::cout << "2. hello from foo1_coroutine" << std::endl;
    }
}

export task foo2_coroutine() noexcept {
    std::cout << "1. hello from foo2_coroutine" << std::endl;
    for (int i = 0; i < 10; ++i) {
        co_await sleep{ 10 };
        std::cout << "2. hello from foo2_coroutine" << std::endl;
    }
}

