#pragma once
#include <string>
#include <iostream>

struct JsonStructure {
    static inline std::string STREAM_TYPE_KEY{"stream_type"};
    static inline std::string STDOUT{"stdout"};
    static inline std::string STDERR{"stderr"};

    static inline std::string CONTENT_KEY{"content"};

    std::ostream& getStream(const std::string& stream_type) {
        return stream_type == JsonStructure::STDOUT ? std::cout : std::cerr;
    }
};

enum class StreamType {
    normal,
    error
};

enum class OS {
    POSIX,
    WINDOWS,
    MAC
};