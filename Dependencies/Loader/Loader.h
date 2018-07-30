//
// Created by Haotian on 2018/7/23.
//

#ifndef MRBASIC_LOADER_H
#define MRBASIC_LOADER_H

#include "GLInclude.h"

class Loader {
public:
    Loader() {}
    void removeShape();
    void newShape();
    Shape *extractShape();
    bool load(const char *path, const char *fileName);
private:
    Shape *shape;
};


#endif //MRBASIC_LOADER_H
