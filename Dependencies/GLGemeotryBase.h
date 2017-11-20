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

struct GLMaterialItem {
    void clear() {
        materialName = "";
        Ns = d = Tr = 0.f;
        Tf = glm::vec3(1.f);
        illum = 0;
        Ka = Kd = Ks = glm::vec3(0.f);
    }

    std::string materialName;
    GLfloat Ns, d, Tr;
    glm::vec3 Tf;
    GLint illum;
    glm::vec3 Ka, Kd, Ks;
};

struct Geometry {
    Geometry(const GLMaterialItem &material) : material(material) {}
    Geometry(const GLMaterialItem &material, const std::vector<GLuint> &faces) :
            material(material), faces(faces) {}

    GLMaterialItem material;
    std::vector<GLuint> faces;
};

struct Shape {
    Shape() = default;
    Shape(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, const std::vector<glm::vec2> &uvs,
          const std::vector<Geometry> &geometries) :
            vertices(vertices), normals(normals), uvs(uvs), geometries(geometries) {}

    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec2> uvs;
    std::vector<Geometry> geometries;
};

#endif //MRBASIC_GLGEMEOTRYBASE_H
