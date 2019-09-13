#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool
{
public:
  ThreadPool(int num) {
    workers.reserve(num);
    for (int i = 0; i < num; i++) {
      workers.emplace_back([this]() {
        for (;;) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(this->taskLock);
            this->condition.wait(lock, [this]() {
              std::cout << "Waiting~~" << std::endl;
              return this->stop.load() || this->tasks.empty();
            });
            if (this->stop.load() && this->tasks.empty()) {
              return;
            }
            task = std::move(this->tasks.front());
            this->tasks.pop();
          }
          task();
        }
      });
    }
  }
        template<class F, class... Args>
	auto pushTask(F&& f, Args&& ...args)->std::future<typename std::result_of<F(Args...)>::type>;
	virtual ~ThreadPool()
	{
		stop.store(true);
		condition.notify_all();
		for (auto& thread : workers) {
			thread.join();
		}
	}

private:
	std::mutex taskLock;
	std::queue<std::function<void()>> tasks;
	std::vector<std::thread> workers;
	std::atomic<bool> stop;
public: std::condition_variable condition;
};

template<class F, class ...Args>
inline auto
ThreadPool::pushTask(F&& f, Args&& ...args)
-> std::future<typename std::result_of<F(Args ...)>::type>
{
	using returnType = typename std::result_of<F(Args...)>::type;
	auto task = std::make_shared<std::packaged_task<returnType(void)>>(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);
	auto futureRes = task->get_future();
	{
		std::unique_lock<std::mutex> lock(taskLock);
		if (stop.load()) {
			throw std::runtime_error("Can't add new task");
		}
		tasks.push([task]() {
			(*task)();
			});
	}
	condition.notify_one();
	return futureRes;
}