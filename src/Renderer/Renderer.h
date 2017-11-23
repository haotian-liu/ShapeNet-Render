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
    Renderer(const glm::mat4 &modelMatrix) : shader(new ShaderProgram), modelMatrix(modelMatrix), mVao(nullptr), textures(nullptr) {
        selected = false;
        isLight = false;
        updateCamera();
    };

    void setAsLight() { isLight = true; }
    void setupPolygon(const std::string &filepath, const std::string &filename);
    void setupShader(const std::string &vs, const std::string &fs);
    void setupBuffer();
    void setupTexture();
    void render();
    void mouseCallback(GLFWwindow *window, int button, int action, int mods);
    void cursorPosCallback(GLFWwindow *window, double xpos, double ypos, double lastX, double lastY);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void updateCamera();
    static void unselectall() { childSelected = false; }

private:
    bool compileShader(ShaderProgram *shader, const std::string &vs, const std::string &fs);
    bool loadPolygon();
    void testIntersection(double x, double y);
    GLfloat tdsign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
    bool inTriangle(glm::vec2 pt, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3);
    bool processPolygon();
//    bool generateNormals();
//    glm::vec3 getVertVector(int index);
//    bool updateNormal(int index, const glm::vec3 &Normal);
    void centralizeShape();
    void normalizeShape();

    static constexpr GLfloat winWidth = 800.f, winHeight = 600.f, ratio = winWidth / winHeight;
    static constexpr GLfloat fovy = 60.f, near = 0.001f, far = 50.f;

    Shape *shape;

    bool isLight;

    static glm::vec3 viewDirection, lightDirection;
    static glm::mat4 viewTransform;

    constexpr static GLfloat MaxDepth = 9999999999.f;
    GLfloat maxDepth;

    glm::vec3 shapeOffset;

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
