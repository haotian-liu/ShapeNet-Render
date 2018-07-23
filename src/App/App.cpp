//
// Created by Haotian on 2017/8/23.
//

#include "App.h"
#include <chrono>

void App::init(const std::string &path, const std::string &file) {
    r = new Renderer(glm::vec3(0.f, 0.f, 0.f));
    r->setupPolygon(path, file);
    r->setupShader("shader/phong.vert", "shader/phong.frag");
    r->setupBuffer();
    r->setupTexture();
}

void App::setViewport(int width, int height) {
    winWidth = width;
    winHeight = height;
    Renderer::winWidth = width;
    Renderer::winHeight = height;
    Renderer::ratio = width * 1.0 / height;
    updateViewport();
}

void App::updateViewport() {
    glViewport(0, 0, winWidth, winHeight);
}

void App::render() {
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    float bg_color = .8f;

    glClearColor(bg_color,bg_color,bg_color, 1.f);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    r->render();
}

void App::idle(GLFWwindow *window) {
    monitorFPS(window);
}

void App::monitorFPS(GLFWwindow *window) {
    static auto last = std::chrono::high_resolution_clock::now();
    static auto lastRefresh = last;
    auto current = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1> >>
            (current - last).count();
    auto refreshDuration = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1> >>
            (current - lastRefresh).count();
    last = current;
    if (duration < 0.0001) { return; }
    if (refreshDuration < 0.1) { return; }
    lastRefresh = current;
    GLuint FPS = static_cast<GLuint>(1.0 / duration);
    std::string newTitle = windowTitle + " (" + std::to_string(FPS) +  " fps)";
    glfwSetWindowTitle(window, newTitle.c_str());
}

void App::mouseCallback(GLFWwindow *window, int button, int action, int mods) {
    Renderer::unselectall();
    r->mouseCallback(window, button, action, mods);
}

void App::cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    static GLfloat lastX, lastY;

    r->cursorPosCallback(window, xpos, ypos, lastX, lastY);
    lastX = xpos;
    lastY = ypos;
}

void App::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    r->keyCallback(window, key, scancode, action, mods);
}
