//
// Created by Haotian on 2017/8/23.
//

#ifndef INC_3DRECONSTRUCTION_RENDERER_H
#define INC_3DRECONSTRUCTION_RENDERER_H

#include "GLInclude.h"
#include "GLFW/glfw3.h"
#include <string>
#include <vector>
#include "Shader/ShaderProgram.h"

class Renderer {
public:
    static GLfloat winWidth, winHeight, ratio;
    Renderer(const glm::vec3 &trans, const glm::mat4 transformation = glm::mat4(1.f)) : shader(new ShaderProgram), translation(trans), mVao(nullptr), textures(nullptr) {
        selected = false;
        isLight = false;
        modelMatrix = transformation * glm::translate(trans);
        updateCamera();
    };

    void setAsLight() {
        hasLight = true;
        isLight = true;
        lightDirection = glm::normalize(translation);
        lightDistance = glm::length(translation);
    }
    struct stat_t {
        float deg_horizontal, deg_vertical;
        stat_t&operator+=(const stat_t &s) {
            deg_horizontal += s.deg_horizontal;
            deg_vertical += s.deg_vertical;
            return *this;
        }
        stat_t&operator-=(const stat_t &s) {
            deg_horizontal -= s.deg_horizontal;
            deg_vertical -= s.deg_vertical;
            return *this;
        }
        stat_t align() {
            deg_horizontal = std::fmod(deg_horizontal, 360);
            deg_vertical = std::fmod(deg_vertical, 360);
            return *this;
        }
    };
    void setupPolygon(const std::string &filepath, const std::string &filename);
    void setupShader(const std::string &vs, const std::string &fs);
    void setupBuffer();
    void setupTexture();
    void render(bool freeze=false);
    void mouseCallback(GLFWwindow *window, int button, int action, int mods);
    void cursorPosCallback(GLFWwindow *window, double xpos, double ypos, double lastX, double lastY);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void updateCamera();
    static void unselectall() { childSelected = false; }
    stat_t get_statistics() const { return statistics; }
    void set_background_path(const std::string &path) { background_path = path; }

private:
    bool compileShader(ShaderProgram *shader, const std::string &vs, const std::string &fs);
    bool loadPolygon();
    void testIntersection(double x, double y);
    GLfloat tdsign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
    bool inTriangle(glm::vec2 pt, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3);
    bool processPolygon();
    GLfloat calculateDepth(const glm::mat3 &triangle, glm::vec2 p);
//    bool generateNormals();
//    glm::vec3 getVertVector(int index);
//    bool updateNormal(int index, const glm::vec3 &Normal);
    void centralizeShape();
    void normalizeShape();
    static constexpr GLfloat fovy = 60.f, near = 0.001f, far = 50.f;
    static bool hasLight;

    Shape *shape;

    std::string background_path;

    bool isLight;

    stat_t statistics;

    static glm::vec3 viewDirection, lightDirection;
    static glm::mat4 viewTransform;
    static glm::vec3 upVector, frontVector;
    static GLfloat lightDistance;

    constexpr static GLfloat MaxDepth = 9999999999.f;
    GLfloat maxDepth;
    glm::mat3 lastMaxTriangle;

    glm::vec3 shapeOffset, translation;
    std::vector<glm::vec3> bboxes;

    ShaderProgram *shader;
    GLuint *mVao, mVbo[5];
    GLuint *textures;

    static GLfloat Yaw, Pitch, Dist;

    std::string filepath;
    std::string filename;
/*    std::vector<GLfloat> verts;
    std::vector<GLfloat> norms;
    std::vector<GLfloat> colors;

    std::vector<GLuint> faces;
    std::vector<GLfloat> uvCoords;*/

    glm::mat4 modelMatrix, viewMatrix, projMatrix;
    bool LBtnDown = false, RBtnDown = false;
    static bool childSelected;
    bool selected;
};

#endif //INC_3DRECONSTRUCTION_RENDERER_H
