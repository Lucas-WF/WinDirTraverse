#include "BFS.h"
#include "ThreadPool.h"
#include <Windows.h>
#include <iostream>

BFS::BFS(const std::wstring& rootDir) {
    dirQueue.push_back(rootDir);
}

void BFS::listFiles(const std::wstring& dir) {
    WIN32_FIND_DATAW fdFile;
    HANDLE hFind = NULL;
    std::deque<std::wstring> auxQueue;

    if ((hFind = FindFirstFileW((dir + L"\\*.*").c_str(), &fdFile)) == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (wcscmp(fdFile.cFileName, L".") != 0 && wcscmp(fdFile.cFileName, L"..") != 0) {
            std::wstring fullPath = dir + L"\\" + fdFile.cFileName;

            if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                auxQueue.push_back(fullPath);
            }
            else {
                findAFile(fullPath);
            }
        }
    } while (FindNextFileW(hFind, &fdFile));

    {
        std::lock_guard<std::mutex> lock(mtx);
        dirQueue.insert(dirQueue.end(), auxQueue.begin(), auxQueue.end());
    }
    cv.notify_one();

    FindClose(hFind);
}

void BFS::traverse() {
    ThreadPool threadPool;
    std::wstring curr;

    threadPool.start();

    do {
        {
            std::unique_lock<std::mutex> lock(mtx);

            cv.wait(lock, [this, &threadPool] {
                return !dirQueue.empty() || !threadPool.is_busy();
            });

            if (dirQueue.empty() && !threadPool.is_busy()) {
                break;
            }

            if (!dirQueue.empty()) {
                curr = dirQueue.front();
                dirQueue.pop_front();
            }
            else {
                continue;
            }
        }

        threadPool.enqueue([this, curr]() {
            listFiles(curr);
        });
    } while (true);

    threadPool.stop();
}

void BFS::findAFile(const std::wstring& file) {
    return;
}
