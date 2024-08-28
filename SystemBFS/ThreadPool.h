#pragma once
#include <vector>
#include <queue>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
private:
	void loop();
	std::mutex queue_mtx;
	std::condition_variable condition;
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> jobs;
public:
	std::atomic<bool> stopThreads{ false };
	bool is_busy();
	void start();
	void enqueue(const std::function<void()>& job);
	void stop(bool drain);
};

