#include "ThreadPool.h"
#include <iostream>

void ThreadPool::loop() {
	while (true) {
		std::function<void()> job;
		{
			std::unique_lock<std::mutex> lock(queue_mtx);

			condition.wait(lock, [this] {
				return stopThreads.load() || !jobs.empty();
			});

			if (stopThreads.load()) return;

			job = std::move(jobs.front());
			jobs.pop();
		}
		job();
	}
}

void ThreadPool::start() {
	const uint32_t num_threads = std::thread::hardware_concurrency();
	for (uint32_t i = 0; i < num_threads; ++i)
		threads.emplace_back(std::thread(&ThreadPool::loop, this));
}

void ThreadPool::enqueue(const std::function<void()>& job) {
	{
		std::unique_lock<std::mutex> lock(queue_mtx);
		jobs.push(job);
	}
	condition.notify_one();
}

void ThreadPool::stop(bool drain) {
	if (drain) {
		std::unique_lock<std::mutex> lock(queue_mtx);
		condition.wait(lock, [this] { return jobs.empty(); });
	}

	stopThreads.store(true);
	condition.notify_all();
	for (std::thread& thread : threads) {
		if (thread.joinable())
			thread.join();
	}
	threads.clear();
}

bool ThreadPool::is_busy() {
	bool poolbusy;
	{
		std::unique_lock<std::mutex> lock(queue_mtx);
		poolbusy = !jobs.empty();
	}
	return poolbusy;
}