//
// Created by Haotian on 2018/7/19.
//

#include "ScreenShot.h"
#include <opencv/cv.hpp>

void ScreenShot::prepare_OGL() {
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenFramebuffers(2, fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
    glGenRenderbuffers(2, rbo);
//    glBindRenderbuffer(GL_RENDERBUFFER, rbo[0]);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, frameWidth, frameHeight);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_COMPONENTS, GL_RENDERBUFFER, rbo[0]);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo[1]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, frameWidth, frameHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo[1]);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "INCOMPLETE FBO\n");
        exit(-1);
    }
}

cv::Mat ScreenShot::take() {
    cv::Mat screenshot = cv::Mat::zeros(frameHeight, frameWidth, CV_8UC3);
    cv::Mat view;

    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, screenshot.data);
    cv::flip(screenshot, view, 0);
    return view;
}
