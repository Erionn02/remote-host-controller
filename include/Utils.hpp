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

//#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//#include <conio.h>
//    void myClear(){
//        clrscr();
//    }
//#else
//    static void __clearStream(std::ostream& stream){
//        stream<<"\033[2J"<<"\033[1;1H"<<std::flush;
//    }
//
//    void myClear(){
//        __clearStream(std::cout);
//        __clearStream(std::cerr);
//    }
//#endif