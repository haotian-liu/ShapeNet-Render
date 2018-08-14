#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include <opencv/cv.hpp>

#include "App/App.h"
#include "ScreenShot/ScreenShot.h"

const int MSAA = 2;
const int WinWidth = 512 * MSAA, WinHeight = 512 * MSAA;
const std::string windowTitle = "3D Reconstruction";
GLfloat Renderer::winWidth, Renderer::winHeight, Renderer::ratio;

auto app = new App;
auto shot = new ScreenShot;

inline static void mouseCallback(GLFWwindow *window, int button, int action, int mods) {
    app->mouseCallback(window, button, action, mods);
}
inline static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    app->keyCallback(window, key, scancode, action, mods);
}
inline static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    app->cursorPosCallback(window, xpos, ypos);
}

int main(int argc, const char **argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 2);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(WinWidth, WinHeight, windowTitle.c_str(), nullptr, nullptr);

    int screenWidth, screenHeight;

    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    if (nullptr == window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (GLEW_OK != glewInit()) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
    }

    app->setViewport(screenWidth, screenHeight);
    app->setTitle(windowTitle);
    app->set_background_path(argv[1]);
    app->init(argv[2], argv[3]);

    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, keyCallback);

    shot->set_frame_size(screenWidth, screenHeight);
    shot->prepare_OGL();

    std::vector<cv::Mat> images, bboxes;
    std::vector<Renderer::stat_t> stats;

    for (int i=0; i<3*20; i++) {
        app->idle(window);

        app->render();

        auto stat = app->get_render_statistics();

//        cv::imshow("window", shot->take());
//        cv::waitKey(0);

        bboxes.push_back(shot->take());

        app->render(true);

        images.push_back(shot->take());
        stats.push_back(stat);
    }

    float scale = 1.0 / MSAA;
    cv::Size size(WinWidth * scale, WinHeight * scale);

    cv::Mat preview = cv::Mat::zeros(size.height * 3, size.width * 20, CV_8UC3);
    for (int i=0; i<3; i++) {
        for (int j=0; j<20; j++) {
            int id = i * 20 + j;
            cv::Mat thumb;
            auto stat = stats[id];

            // store bbox image
            cv::resize(bboxes[id], thumb, size, 0, 0, cv::INTER_AREA);
            cv::imwrite(std::string(argv[5]) + std::to_string(id) + ".png", thumb);

            // store bg image
            cv::resize(images[id], thumb, size, 0, 0, cv::INTER_AREA);
//            cv::imwrite(std::string(argv[3]) + std::to_string(stat.deg_vertical) + "," + std::to_string(stat.deg_horizontal) + ".png", thumb);
            cv::imwrite(std::string(argv[4]) + std::to_string(id) + ".png", thumb);
//            thumb.copyTo(preview(cv::Rect(j * size.width, i * size.height, size.width, size.height)));
        }
    }

//    cv::imwrite("preview.jpg", preview);

    glfwTerminate();
    return 0;
}
