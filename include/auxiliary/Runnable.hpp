#pragma once

#include <atomic>
#include <thread>


class Runnable {
public:
    virtual void start() {
        startUpHook();
        is_running = true;
        t = std::jthread{
                [this] {
                    while (is_running) {
                        workerLoop();
                    }
                }
        };
    }

    void stop() {
        is_running = false;
        if (t.joinable()) {
            t.join();
        }
    }

    virtual ~Runnable() = default;

    bool isRunning() { return is_running; }

protected:
    virtual void workerLoop() = 0;

    virtual void startUpHook() {}

    std::jthread t{};
    std::atomic<bool> is_running{false};
};


