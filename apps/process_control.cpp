#include <boost/process.hpp>
#include <boost/process/pipe.hpp>
#include <boost/asio/io_service.hpp>
#include <tbb/concurrent_queue.h>

#include <thread>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

int main() {
    using namespace boost;


    process::pstream in;
    std::unique_ptr<FILE, decltype(&fclose)> out_stream{fopen("output.txt", "w"), fclose};
    process::child c(
            "bash -i",
            process::std_out > out_stream.get(),
            process::std_err > out_stream.get(),
            process::std_in < in
    );

    std::cout << "STARTING LOOP: \n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ssize_t total_bytes_read{0};
    do {
        std::size_t len{0};
        ssize_t bytes_read{0};
        std::unique_ptr<char, decltype(&free)> line{NULL, free};
        char *line_ptr = line.get();
        std::unique_ptr<FILE, decltype(&fclose)> please_work{fopen("output.txt", "r"), fclose};
        fseek(please_work.get(), total_bytes_read, SEEK_CUR);
        while ((bytes_read = getline(&line_ptr, &len, please_work.get())) != EOF) {
            printf("%s", line_ptr);
            total_bytes_read += bytes_read;
        }

        std::string input_command{};
        std::getline(std::cin, input_command);
        if (c.running()) { //to prevent sigpipe if process dies during input
            in << input_command << std::endl;
        }
    } while (c.running());

    return 0;
}


