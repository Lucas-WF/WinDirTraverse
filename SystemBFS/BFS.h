#pragma once
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

class BFS {
private:
    std::deque<std::wstring> dirQueue;
    std::mutex mtx;
    void listFiles(const std::wstring& dir);
public:
    BFS(const std::wstring& rootDir);
    std::condition_variable cv;
    void traverse();
    void findAFile(const std::wstring& file);
};

