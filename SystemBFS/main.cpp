#include "BFS.h"
#include <chrono>
#include <iostream>

int main() {
    auto start = std::chrono::system_clock::now();
    BFS bfs(L"C:");
    bfs.traverse();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << elapsed_seconds.count() << std::endl;
    return 0;
}