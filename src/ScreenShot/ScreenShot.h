//
// Created by Haotian on 2018/7/19.
//

#ifndef MRBASIC_SCREENSHOT_H
#define MRBASIC_SCREENSHOT_H

#include <opencv2/core/mat.hpp>
#include "GLInclude.h"

class ScreenShot {
public:
    void set_frame_size(GLuint width, GLuint height) {
        frameWidth = width;
        frameHeight = height;
    }
    void prepare_OGL();
    cv::Mat take();
//private:
public: // TODO: FIX ME!!! SET TO PRIVATE!!!!
    GLuint fbo[2], rbo[2];
    GLuint renderedTexture;
    GLuint frameWidth, frameHeight;
};


#endif //MRBASIC_SCREENSHOT_H
