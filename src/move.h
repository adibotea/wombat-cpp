
#ifndef _MOVE_H_
#define _MOVE_H_

#include <string>

struct Move {
    int col;
    int row;
    bool direction;
    std::string word;

    Move(int c, int r, bool dir, std::string w):
    col(c), row(r), direction(dir), word(w)
    {}
};

#endif