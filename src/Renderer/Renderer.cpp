//
// Created by Haotian on 2017/8/23.
//

#include "Renderer.h"
#include <fstream>
#include "OBJLoader.h"
#include "SOIL.h"

glm::vec3 Renderer::viewDirection(0.f, 0.f, 1.f), Renderer::lightDirection;
glm::mat4 Renderer::viewTransform(1.f);
GLfloat Renderer::Yaw = 270.f, Renderer::Pitch = 90.f, Renderer::Dist = 3.f;
bool Renderer::childSelected;

void Renderer::setupPolygon(const std::string &filepath, const std::string &filename) {
    this->filepath = filepath;
    this->filename = filename;
    loadPolygon();
    processPolygon();
}

void Renderer::setupShader(const std::string &vs, const std::string &fs) {
    compileShader(shader, vs, fs);
}

void Renderer::setupTexture() {
    delete [] textures;
    textures = new GLuint[shape->materials.size()];
    glGenTextures(shape->materials.size(), textures);

    for (int i=0; i<shape->materials.size(); i++) {
        auto &material = shape->materials[i];
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        material.map_Kd.image = SOIL_load_image(material.map_Kd.filepath, &material.map_Kd.width, &material.map_Kd.height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, material.map_Kd.width, material.map_Kd.height, 0, GL_RGB, GL_UNSIGNED_BYTE, material.map_Kd.image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Renderer::setupBuffer() {
    // Create the buffers for the vertices atttributes
    delete [] mVao;
    mVao = new GLuint[shape->geometries.size()];
    glGenVertexArrays(shape->geometries.size(), mVao);

    for (int i=0; i<shape->geometries.size(); i++) {
        glBindVertexArray(mVao[i]);
        glGenBuffers(sizeof(mVbo) / sizeof(GLuint), mVbo);
        // vertex coordinate
        glBindBuffer(GL_ARRAY_BUFFER, mVbo[0]);
        glBufferData(GL_ARRAY_BUFFER, shape->vertices.size() * sizeof(glm::vec3), &shape->vertices[0], GL_STATIC_DRAW);
        GLuint locVertPos = glGetAttribLocation(shader->ProgramId(), "vertPos");
        glEnableVertexAttribArray(locVertPos);
        glVertexAttribPointer(locVertPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
        // vertex normal
        glBindBuffer(GL_ARRAY_BUFFER, mVbo[1]);
        glBufferData(GL_ARRAY_BUFFER, shape->normals.size() * sizeof(glm::vec3), &shape->normals[0], GL_STATIC_DRAW);
        GLuint locVertNormal = glGetAttribLocation(shader->ProgramId(), "vertNormal");
        glEnableVertexAttribArray(locVertNormal);
        glVertexAttribPointer(locVertNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
        // vertex uv
        glBindBuffer(GL_ARRAY_BUFFER, mVbo[2]);
        glBufferData(GL_ARRAY_BUFFER, shape->uvs.size() * sizeof(glm::vec2), &shape->uvs[0], GL_STATIC_DRAW);
        GLuint locVertUV = glGetAttribLocation(shader->ProgramId(), "vertUV");
        glEnableVertexAttribArray(locVertUV);
        glVertexAttribPointer(locVertUV, 2, GL_FLOAT, GL_FALSE, 0, 0);
        // index
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVbo[3]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape->geometries[i].faces.size() * sizeof(GLuint),
                     &shape->geometries[i].faces[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }
}

void Renderer::mouseCallback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (GLFW_PRESS == action) {
            LBtnDown = true;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            testIntersection(xpos, ypos);
        } else if (GLFW_RELEASE == action) {
            LBtnDown = false;
        }
    }
}

void Renderer::cursorPosCallback(GLFWwindow *window, double currentX, double currentY, double lastX, double lastY) {
    GLfloat diffX, diffY;

    if (LBtnDown) {
        diffX = currentX - lastX;
        diffY = currentY - lastY;

        if (!childSelected) {
            Yaw += diffX;
            Pitch += diffY;

            viewTransform = glm::rotate(glm::radians(diffX), glm::vec3(0.f, 1.f, 0.f)) *
                            glm::rotate(glm::radians(diffY), glm::vec3(1.f, 0.f, 0.f)) * viewTransform;
            viewDirection = viewTransform * glm::vec4(0.f, 0.f, 1.f, 1.f);
            updateCamera();
        } else if (selected) {
            glm::vec3 trans(diffX / winWidth, -diffY / winHeight, 0.f);
            trans = glm::inverse(viewTransform) * glm::vec4(trans, 1.f);
#ifdef DEBUG
            printf("Selected and moving.. %f %f\n", diffX, diffY);
            printf("Selected and moving.. %f %f %f\n", trans.x, trans.y, trans.z);
#endif
            modelMatrix = glm::translate(trans) * modelMatrix;
        }
    }
}

void Renderer::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
        Dist *= 0.99;
    }
    if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
        Dist *= 1.01;
    }
    updateCamera();
}

void Renderer::updateCamera() {
//    viewDirection = glm::vec3(
//            sin(glm::radians(Yaw)) * cos(glm::radians(Pitch)),
//            sin(glm::radians(Yaw)) * sin(glm::radians(Pitch)),
//            cos(glm::radians(Yaw))
//    );

    viewDirection = glm::normalize(viewDirection);
    lightDirection = viewDirection;

    viewMatrix = glm::lookAt(
            viewDirection * Dist,
            glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(0.f, 1.f, 0.f)
    );
}

void Renderer::render() {
    projMatrix = glm::perspective(glm::radians(fovy), ratio, near, far);

    shader->Activate();
    glUniformMatrix4fv(glGetUniformLocation(shader->ProgramId(), "viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ProgramId(), "modelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ProgramId(), "projMatrix"), 1, GL_FALSE, &projMatrix[0][0]);

    glUniform3fv(glGetUniformLocation(shader->ProgramId(), "LightDirection"), 1, &lightDirection[0]);

    for (int i=0; i<shape->geometries.size(); i++) {
        const auto &material = shape->materials[shape->geometries[i].materialID];
        glBindVertexArray(mVao[i]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[shape->geometries[i].materialID]);
        glUniform1i(glGetUniformLocation(shader->ProgramId(), "hasTexture"), strlen(material.map_Kd.filepath) != 0);
        glUniform1i(glGetUniformLocation(shader->ProgramId(), "textureSampler"), 0);
        glUniform1i(glGetUniformLocation(shader->ProgramId(), "selected"), selected);
        glUniform3fv(glGetUniformLocation(shader->ProgramId(), "Ka"), 1, &material.Ka[0]);
        glUniform3fv(glGetUniformLocation(shader->ProgramId(), "Kd"), 1, &material.Kd[0]);
        glUniform3fv(glGetUniformLocation(shader->ProgramId(), "Ks"), 1, &material.Ks[0]);
        glUniform1f(glGetUniformLocation(shader->ProgramId(), "Ns"), material.Ns);
        glDrawElements(GL_TRIANGLES, shape->geometries[i].faces.size(), GL_UNSIGNED_INT, 0);
    }

    shader->Deactivate();
}

bool Renderer::loadPolygon() {
    auto *loader = new OBJLoader();
    loader->newShape();
    loader->load(filepath.c_str(), filename.c_str());
    shape = loader->extractShape();
    fprintf(stdout, "Loaded %d vertices, %d faces!\n", shape->vertices.size(), shape->faces);
    return true;
}

bool Renderer::processPolygon() {
//    centralizeShape();
    normalizeShape();
    return true;
}

void Renderer::normalizeShape() {
    GLfloat maxVector = 0.00000001f;
    for (const auto &vert : shape->vertices) {
        if (glm::length(vert) > maxVector) {
            maxVector = glm::length(vert);
        }
    }
    for (auto &vert : shape->vertices) {
        vert /= maxVector;
    }
}

void Renderer::centralizeShape() {
    glm::vec3 Max, Min;
    if (shape->vertices.empty()) { return; }
    Max = Min = shape->vertices[0];
    for (int i=1; i<shape->vertices.size(); i++) {
        const auto &v = shape->vertices[i];
        if (Max.x < v.x) { Max.x = v.x; }
        if (Max.y < v.y) { Max.y = v.y; }
        if (Max.z < v.z) { Max.z = v.z; }
        if (Min.x > v.x) { Min.x = v.x; }
        if (Min.y > v.y) { Min.y = v.y; }
        if (Min.z > v.z) { Min.z = v.z; }
    }
    shapeOffset = -(Max + Min) / 2;
}

/* glm::vec3 Renderer::getVertVector(int index) {
    return glm::vec3(verts[index], verts[index + 1], verts[index + 2]);
}

bool Renderer::updateNormal(int index, const glm::vec3 &Normal) {
    norms[index] = Normal.x;
    norms[index + 1] = Normal.y;
    norms[index + 2] = Normal.z;
    return true;
}

bool Renderer::generateNormals() {
    norms.resize(verts.size());
    for (int i = 0; i < faces.size(); i += 3) {
        glm::vec3 p1 = getVertVector(faces[i] * 3);
        glm::vec3 p2 = getVertVector(faces[i + 1] * 3);
        glm::vec3 p3 = getVertVector(faces[i + 2] * 3);

        glm::vec3 u = p2 - p1, v = p3 - p1;
        glm::vec3 Normal;
        Normal.x = u.y * v.z - u.z * v.y;
        Normal.y = u.z * v.x - u.x * v.z;
        Normal.z = u.x * v.y - u.y * v.x;

        updateNormal(faces[i] * 3, Normal);
        updateNormal(faces[i + 1] * 3, Normal);
        updateNormal(faces[i + 2] * 3, Normal);
    }
    return true;
}*/

bool Renderer::compileShader(ShaderProgram *shader, const std::string &vs, const std::string &fs) {
    auto VertexShader = new Shader(Shader::Shader_Vertex);
    if (!VertexShader->CompileSourceFile(vs)) {
        delete VertexShader;
        return false;
    }

    auto FragmentShader = new Shader(Shader::Shader_Fragment);
    if (!FragmentShader->CompileSourceFile(fs)) {
        delete VertexShader;
        delete FragmentShader;
        return false;
    }
    shader->AddShader(VertexShader, true);
    shader->AddShader(FragmentShader, true);
    //Link the program.
    return shader->Link();
}

GLfloat Renderer::tdsign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool Renderer::inTriangle(glm::vec2 pt, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3) {
    bool b1, b2, b3;

    b1 = tdsign(pt, v1, v2) < 0.0f;
    b2 = tdsign(pt, v2, v3) < 0.0f;
    b3 = tdsign(pt, v3, v1) < 0.0f;

    return ((b1 == b2) && (b2 == b3));
}

void Renderer::testIntersection(double x, double y) {
    selected = false;

    double lx = (x / winWidth - 0.5) * 2;
    double ly = (0.5 - y / winHeight) * 2;

    glm::vec2 pp(lx, ly);

    glm::mat4 MVPMatrix = projMatrix * viewMatrix * modelMatrix;

    for (const auto &geometry : shape->geometries) {
        for (int i=0; i<geometry.faces.size(); i+=3) {
            glm::vec4 v1(shape->vertices[geometry.faces[i]], 1.f);
            glm::vec4 v2(shape->vertices[geometry.faces[i+1]], 1.f);
            glm::vec4 v3(shape->vertices[geometry.faces[i+2]], 1.f);

            v1 = MVPMatrix * v1; v1 /= v1.w;
            v2 = MVPMatrix * v2; v2 /= v2.w;
            v3 = MVPMatrix * v3; v3 /= v3.w;

            glm::vec2 a1(v1), a2(v2), a3(v3);

            if (inTriangle(pp, a1, a2, a3)) {
                selected = true;
                childSelected = true;
            }
        }
    }
}
