//
// Created by Haotian on 2017/8/23.
//

#include "Renderer.h"
#include <fstream>
#include "OBJLoader.h"
#include "SOIL.h"

void Renderer::setupPolygon(const std::string &filepath, const std::string &filename) {
    this->filepath = filepath;
    this->filename = filename;
    loadPolygon();
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
        GLuint locVertUV = (GLuint)glGetAttribLocation(shader->ProgramId(), "vertUV");
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
        } else if (GLFW_RELEASE == action) {
            LBtnDown = false;
        }
    }
}

void Renderer::cursorPosCallback(GLFWwindow *window, double currentX, double currentY) {
    static double lastX, lastY;
    GLfloat diffX, diffY;

    if (LBtnDown) {
        diffX = currentX - lastX;
        diffY = currentY - lastY;

        Yaw += diffX;
        Pitch += diffY;

        viewDirection = glm::rotate(viewDirection, glm::radians(diffX), glm::vec3(1.f, 0.f, 0.f));
        viewDirection = glm::rotate(viewDirection, glm::radians(diffY), glm::vec3(0.f, 1.f, 0.f));

        updateCamera();
    }

    lastX = currentX;
    lastY = currentY;
}

void Renderer::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
        viewDirection = glm::rotate(viewDirection, glm::radians(2.f), glm::vec3(0.f, 0.f, 1.f));
    }
    if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
        viewDirection = glm::rotate(viewDirection, glm::radians(-2.f), glm::vec3(0.f, 0.f, 1.f));
    }
    updateCamera();
}

void Renderer::updateCamera() {
    viewDirection = glm::vec3(
            sin(glm::radians(Yaw)) * cos(glm::radians(Pitch)),
            sin(glm::radians(Yaw)) * sin(glm::radians(Pitch)),
            cos(glm::radians(Yaw))
    );

    viewDirection = glm::normalize(viewDirection);
    lightDirection = viewDirection;

    viewMatrix = glm::lookAt(
            viewDirection * Dist,
            glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(0.f, 1.f, 0.f)
    );
}

void Renderer::render() {
    projMatrix = glm::perspective(glm::radians(60.f), 800.f / 600, 0.005f, 20.f);
    modelMatrix = glm::rotate(glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(glm::radians(180.f), glm::vec3(0.f, 0.f, 1.f)) * glm::translate(shapeOffset);

    shader->Activate();
    glUniformMatrix4fv(glGetUniformLocation(shader->ProgramId(), "viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ProgramId(), "modelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->ProgramId(), "projMatrix"), 1, GL_FALSE, &projMatrix[0][0]);

    glUniform3fv(glGetUniformLocation(shader->ProgramId(), "LightDirection"), 1, &lightDirection[0]);

    for (int i=0; i<shape->geometries.size(); i++) {
        glBindVertexArray(mVao[i]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[shape->geometries[i].materialID]);
        glUniform1i(glGetUniformLocation(shader->ProgramId(), "textureSampler"), 0);
        glDrawElements(GL_TRIANGLES, shape->geometries[i].faces.size(), GL_UNSIGNED_INT, 0);
    }

    shader->Deactivate();
}

bool Renderer::loadPolygon() {
    auto *loader = new OBJLoader();
    loader->newShape();
    loader->load(filepath.c_str(), filename.c_str());
    shape = loader->extractShape();
    fprintf(stdout, "Loaded %d vertices, %d faces!", shape->vertices.size(), shape->faces);
    return true;
}

bool Renderer::processPolygon() {
    return true;
    centralizeShape();
    if (norms.size() == 0) {
        generateNormals();
    }
    if (colors.size() == 0) {
        colors.resize(verts.size() / 3 * 4, 0.2f);
    }
    return true;
}

void Renderer::centralizeShape() {
    glm::vec3 Max, Min;
    if (verts.size() == 0) { return; }
    Max = Min = getVertVector(0);
    for (int i=1; i<verts.size() / 3; i++) {
        auto v = getVertVector(i);
        if (Max.x < v.x) { Max.x = v.x; }
        if (Max.y < v.y) { Max.y = v.y; }
        if (Max.z < v.z) { Max.z = v.z; }
        if (Min.x > v.x) { Min.x = v.x; }
        if (Min.y > v.y) { Min.y = v.y; }
        if (Min.z > v.z) { Min.z = v.z; }
    }
    shapeOffset = -(Max + Min) / 2;
}

glm::vec3 Renderer::getVertVector(int index) {
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
}

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
