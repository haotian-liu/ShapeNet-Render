//
// Created by Haotian on 2017/8/23.
//

#include "App.h"
#include <chrono>
#include <opencv/cv.hpp>

void App::init(const std::string &path, const std::string &file) {
    r = new Renderer(glm::vec3(0.f, 0.f, 0.f));
    r->setupPolygon(path, file);
    r->setupShader("shader/phong.vert", "shader/phong.frag");
    r->setupBuffer();
    r->setupTexture();
    background_shader_texture();
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

void App::render(bool freeze) {
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    float bg_color = .95f;

    glClearColor(bg_color,bg_color,bg_color, 1.f);
    glClearDepth(1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (freeze) {
        render_background();
    }

    r->render(freeze);
}

void App::idle(GLFWwindow *window) {
    monitorFPS(window);
}

void App::background_shader_texture() {
    if (background_path == "null") { return; }
    // setup textures
    glGenTextures(1, &background_texture_id);
    glBindTexture(GL_TEXTURE_2D, background_texture_id);

    cv::Mat image = cv::imread(background_path);
    glPixelStorei(GL_UNPACK_ALIGNMENT, (image.step & 3) ? 1 : 4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, image.step / image.elemSize());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // setup shaders
    std::string vs = "shader/bg.vert", fs = "shader/bg.frag";
    auto VertexShader = new Shader(Shader::Shader_Vertex);
    if (!VertexShader->CompileSourceFile(vs)) {
        delete VertexShader;
        exit(-1);
    }

    auto FragmentShader = new Shader(Shader::Shader_Fragment);
    if (!FragmentShader->CompileSourceFile(fs)) {
        delete VertexShader;
        delete FragmentShader;
        exit(-1);
    }
    background_shader.AddShader(VertexShader, true);
    background_shader.AddShader(FragmentShader, true);
    //Link the program.
    background_shader.Link();

    GLfloat background_verts[] = {-1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f, 1.f};
    GLuint background_vert_order[] = {0, 1, 2, 1, 3, 2};

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    glGenBuffers(2, mVBO);
    // vertex coordinate
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &background_verts[0], GL_STATIC_DRAW);
    GLuint locVertXY = glGetAttribLocation(background_shader.ProgramId(), "xy");
    glEnableVertexAttribArray(locVertXY);
    glVertexAttribPointer(locVertXY, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), &background_vert_order[0], GL_STATIC_DRAW);
}

void App::render_background() {
    if (background_path == "null") { return; }
    glClear(GL_DEPTH_BUFFER_BIT);
    background_shader.Activate();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, background_texture_id);
    glUniform1i(glGetUniformLocation(background_shader.ProgramId(), "textureSampler"), 0);
    glBindVertexArray(mVAO);
    glDisable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    background_shader.Deactivate();
    glEnable(GL_CULL_FACE);

    glClear(GL_DEPTH_BUFFER_BIT);
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
