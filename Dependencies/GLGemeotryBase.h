//
// Created by Haotian on 2017/11/19.
//

#ifndef MRBASIC_GLGEMEOTRYBASE_H
#define MRBASIC_GLGEMEOTRYBASE_H

#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include "ext.hpp"

struct MaterialMap {
    char filepath[200];
    GLint width, height;
    GLubyte *image;
};

struct GLMaterialItem {
    void clear() {
        materialName = "";
        Ns = d = Tr = 0.f;
        Tf = glm::vec3(1.f);
        illum = 0;
        Ka = Kd = Ks = glm::vec3(0.f);
        memset(map_Kd.filepath, 0, sizeof(map_Kd.filepath));
    }

    MaterialMap map_Kd;
    std::string materialName;
    GLfloat Ns, d, Tr;
    glm::vec3 Tf;
    GLint illum;
    glm::vec3 Ka, Kd, Ks;
};

struct Geometry {
    explicit Geometry(GLuint materialID) : materialID(materialID) {}

    GLuint materialID;
    std::vector<GLuint> faces;
};

struct Shape {
    Shape() = default;

    int faces;
    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec2> uvs;
    std::vector<Geometry> geometries;
    std::vector<GLMaterialItem> materials;
};

#endif //MRBASIC_GLGEMEOTRYBASE_H
