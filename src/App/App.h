//
// Created by Haotian on 2017/8/23.
//

#ifndef INC_3DRECONSTRUCTION_APP_H
#define INC_3DRECONSTRUCTION_APP_H

#include <string>
#include "GLInclude.h"
#include "GLFW/glfw3.h"
#include "Renderer/Renderer.h"

class App {
public:
    void init(const std::string &path, const std::string &file);
    void setTitle(const std::string &title) { windowTitle = title; }
    void setViewport(int width, int height);
    void updateViewport();
    void render(bool freeze=false);
    Renderer::stat_t get_render_statistics() const { return r->get_statistics(); }
    void idle(GLFWwindow *window);
    void mouseCallback(GLFWwindow *window, int button, int action, int mods);
    void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void set_background_path(const std::string &path) { background_path = path; }

private:
    void monitorFPS(GLFWwindow *window);
    void render_background();
    void background_shader_texture();

    std::string windowTitle;
    std::string background_path;
    GLuint background_texture_id, mVAO, mVBO[2];

    ShaderProgram background_shader;
    Renderer *r;
    int winWidth, winHeight;
};


#endif //INC_3DRECONSTRUCTION_APP_H
