//
// Created by Haotian on 2017/11/18.
//

#ifndef MRBASIC_OBJLOADER_H
#define MRBASIC_OBJLOADER_H

#pragma once

#include "GLInclude.h"
#include "MaterialLoader.h"

#include <list>
#include <tuple>
#include <map>

class OBJLoader {
public:
    OBJLoader() : shape(nullptr) {}
    void removeShape() {}
    void newShape() {
        removeShape();
        shape = new Shape;
    }
    Shape *extractShape() {
        Shape *ret = shape;
        shape = nullptr;
        return ret;
    }
    bool load(const char *path, const char *fileName);
private:
    Shape *shape;
    bool shape_map(std::vector<glm::vec3> &vertices,
                   std::vector<std::vector<std::tuple<GLuint, GLuint>>> &vertice_mapper,
                   std::vector<std::vector<std::tuple<GLuint, GLuint>>> &geometries,
                   std::vector<unsigned int> &vertexIndices, std::vector<unsigned int> &uvIndices,
                   std::vector<unsigned int> &normalIndices);
};

bool OBJLoader::load(const char *path, const char *fileName) {
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<GLMaterialItem> materials;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> uvs;
    std::vector<glm::vec3> normals;

    std::string filePath(path);
    filePath += fileName;

    GLint currentMaterialId = -1;

    FILE *file = fopen(filePath.c_str(), "r");
    if (file == NULL) {
        printf("Failed to open the file in `%s`!\n", filePath.c_str());
        return false;
    }

    std::vector<std::vector<std::tuple<GLuint, GLuint>>> vertice_mapper;
    std::vector<std::vector<std::tuple<GLuint, GLuint>>> geometries;

    while (true) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) { break; }

        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec3 uv;
            fscanf(file, "%f %f %f\n", &uv.x, &uv.y, &uv.z);
            uvs.push_back(uv);
        } else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            normals.push_back(normal);
        } else if (strcmp(lineHeader, "f") == 0) {
            unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
            bool flag = true;
            char buff[255];
            fgets(buff, 250, file);
            int matches = sscanf(buff, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0],
                                 &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2],
                                 &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3]);
            if (matches != 12) {
                matches = sscanf(buff, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2],
                                 &normalIndex[2]);
                if (matches != 9) {
                    matches = sscanf(buff, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0],
                            &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
                    flag = false;
                    if (matches != 6) {
                        fprintf(stderr, "This is not standard obj our program can process!");
                        return false;
                    }
                }
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
            if (flag) {
                uvIndices.push_back(uvIndex[0]);
                uvIndices.push_back(uvIndex[1]);
                uvIndices.push_back(uvIndex[2]);
            } else {
                uvIndices.push_back(1);
                uvIndices.push_back(1);
                uvIndices.push_back(1);
            }
            if (matches == 12) {
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[2]);
                vertexIndices.push_back(vertexIndex[3]);
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[2]);
                normalIndices.push_back(normalIndex[3]);
                uvIndices.push_back(uvIndex[0]);
                uvIndices.push_back(uvIndex[2]);
                uvIndices.push_back(uvIndex[3]);
            }
        } else if (strcmp(lineHeader, "mtllib") == 0) {
            char mtlFile[200];
            fscanf(file, "%s\n", mtlFile);
            GLMaterial material;
            material.load(path, mtlFile);
            materials.insert(materials.end(), material.get_materials().begin(), material.get_materials().end());
        } else if (strcmp(lineHeader, "usemtl") == 0) {
            if (!vertexIndices.empty()) {
                shape_map(vertices, vertice_mapper, geometries, vertexIndices, uvIndices, normalIndices);
                shape->geometries.emplace_back(currentMaterialId);
            }

            char mtl[200];
            fscanf(file, "%s\n", mtl);
            bool flag = true;
            for (int i = 0; i < materials.size(); i++) {
                if (mtl == materials[i].materialName) {
                    currentMaterialId = i;
                    flag = false;
                    break;
                }
            }
            if (flag) { exit(-1); }
        } else {
            while (fgetc(file) != '\n') {
                continue;
            }
        }
    }
    if (!vertexIndices.empty()) {
        shape_map(vertices, vertice_mapper, geometries, vertexIndices, uvIndices, normalIndices);
        shape->geometries.emplace_back(currentMaterialId);
    }
    if (uvs.empty()) {
        uvs.push_back(glm::vec3(0.f));
    }

    std::map<std::tuple<GLuint, GLuint, GLuint>, GLuint> nindex_mapper;
    int index = 0;
    for (int i=0; i<vertice_mapper.size(); i++) {
        for (const auto &each : vertice_mapper[i]) {
            auto tuple = std::make_tuple(i, std::get<0>(each), std::get<1>(each));
            nindex_mapper.insert(std::make_pair(tuple, index++));
            shape->vertices.push_back(vertices[i]);
            shape->normals.push_back(normals[std::get<0>(each)]);
            shape->uvs.emplace_back(uvs[std::get<1>(each)]);
        }
    }

    shape->faces = 0;
    for (int i=0; i<geometries.size(); i++) {
        const auto &face_mapper = geometries[i];
        shape->faces += face_mapper.size();
        for (int j=0; j<face_mapper.size(); j++) {
            const auto &each = face_mapper[j];
            const auto &pair = vertice_mapper[std::get<0>(each)][std::get<1>(each)];
            auto find_key = std::make_tuple(std::get<0>(each), std::get<0>(pair), std::get<1>(pair));
            auto res = nindex_mapper.find(find_key);
            if (res == nindex_mapper.end()) {
                fprintf(stderr, "%d %d %d\n", std::get<0>(each), std::get<0>(pair), std::get<1>(pair));
            }
            GLuint index = res->second;
            shape->geometries[i].faces.push_back(index);
        }
    }
    shape->faces /= 3;

    shape->materials = materials;

    return true;
}

bool OBJLoader::shape_map(std::vector<glm::vec3> &vertices,
                          std::vector<std::vector<std::tuple<GLuint, GLuint>>> &vertice_mapper,
                          std::vector<std::vector<std::tuple<GLuint, GLuint>>> &geometries,
                          std::vector<unsigned int> &vertexIndices, std::vector<unsigned int> &uvIndices,
                          std::vector<unsigned int> &normalIndices) {
    if (vertice_mapper.size() < vertices.size()) {
        vertice_mapper.resize(vertices.size());
    }
    /////////////////////////

    std::vector<std::tuple<GLuint, GLuint>> faces_mapper;

    for (int i=0; i<vertexIndices.size(); i++) {
        auto tuple = std::tuple<GLuint, GLuint>(normalIndices[i] - 1, uvIndices[i] - 1);
        auto &search = vertice_mapper[vertexIndices[i] - 1];

        auto loc = std::find(search.begin(), search.end(), tuple);
        if (loc == search.end()) {
            search.push_back(tuple);
            faces_mapper.emplace_back(vertexIndices[i] - 1, search.size() - 1);
        } else {
            faces_mapper.emplace_back(vertexIndices[i] - 1, loc - search.begin());
        }
    }

    geometries.push_back(faces_mapper);

    /////////////////////////

    vertexIndices.clear();
    uvIndices.clear();
    normalIndices.clear();
    return true;
}

#endif //MRBASIC_OBJLOADER_H
