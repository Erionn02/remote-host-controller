#pragma once

namespace portable {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <conio.h>
    char p_getChar(){
        return (char)_getch();
    }
#else

#include <unistd.h>
#include <termios.h>

    char p_getChar() {
        char buf = 0;
        struct termios old{};
        if (tcgetattr(0, &old) < 0)
            perror("tcsetattr()");
        old.c_lflag &= static_cast<tcflag_t>(~ICANON);
        old.c_lflag &= static_cast<tcflag_t>(~ECHO);
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
            perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
            perror("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
            perror("tcsetattr ~ICANON");
        return (buf);
    }

#endif

}
