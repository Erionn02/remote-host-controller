#include "auxiliary/Runnable.hpp"

#include <spdlog/spdlog.h>

void Runnable::start() {
    is_running.store(true);
    set_up_hook();
    worker_thread = std::jthread(
            [&] {
                while (is_running) {
                    workerLoop();
                }
                spdlog::info("{} shutdown...", name());
            });
    spdlog::info("{} started.", name());
}


void Runnable::stop() {
    spdlog::info("Stopping {} ...", name());
    is_running.store(false);

    tear_down();

    if (worker_thread.joinable()) {
        worker_thread.join();
    }
    spdlog::info("{} stopped.", name());
}


Runnable::~Runnable() {
    if(is_running){
        stop();
    }
}




