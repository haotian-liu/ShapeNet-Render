//
// Created by Haotian on 2017/11/18.
//

#ifndef MRBASIC_MATERIALLOADER_H_H
#define MRBASIC_MATERIALLOADER_H_H

#pragma once

#include "GLInclude.h"

class GLMaterial {
public:
    bool load(const char *path, const char *fileName);
    const std::vector<GLMaterialItem> &get_materials() const { return materials; }

private:
    std::vector<GLMaterialItem> materials;
};

bool GLMaterial::load(const char *path, const char *fileName) {
    GLMaterialItem item;
    char materialNameBuffer[255];
    char lineHeaderBuffer[255];
    bool hasMaterial = false;
    std::string filePath(path);
    filePath += fileName;

    FILE *file = fopen(filePath.c_str(), "r");
    if (file == NULL) {
        printf("Failed to open the file in `%s`!\n", filePath.c_str());
        return false;
    }

    while (true) {
        int res = fscanf(file, "%s", lineHeaderBuffer);
        if (res == EOF) { break; }

        if (strcmp(lineHeaderBuffer, "newmtl") == 0) {
            if (!hasMaterial) { hasMaterial = true; }
            else { materials.push_back(item); }

            item.clear();
            fscanf(file, "%s\n", materialNameBuffer);
            item.materialName = materialNameBuffer;
        } else if (strcmp(lineHeaderBuffer, "Ns") == 0) {
            fscanf(file, "%f\n", &item.Ns);
        } else if (strcmp(lineHeaderBuffer, "d") == 0) {
            fscanf(file, "%f\n", &item.d);
        } else if (strcmp(lineHeaderBuffer, "Tr") == 0) {
            fscanf(file, "%f\n", &item.Tr);
        } else if (strcmp(lineHeaderBuffer, "Tf") == 0) {
            fscanf(file, "%f %f %f\n", &item.Tf.x, &item.Tf.y, &item.Tf.z);
        } else if (strcmp(lineHeaderBuffer, "illum") == 0) {
            fscanf(file, "%d\n", &item.illum);
        } else if (strcmp(lineHeaderBuffer, "Ka") == 0) {
            fscanf(file, "%f %f %f\n", &item.Ka.x, &item.Ka.y, &item.Ka.z);
        } else if (strcmp(lineHeaderBuffer, "Kd") == 0) {
            fscanf(file, "%f %f %f\n", &item.Kd.x, &item.Kd.y, &item.Kd.z);
        } else if (strcmp(lineHeaderBuffer, "Ks") == 0) {
            fscanf(file, "%f %f %f\n", &item.Ks.x, &item.Ks.y, &item.Ks.z);
        } else {
            while (fgetc(file) != '\n');
        }
    }

    if (hasMaterial) {
        materials.push_back(item);
    }
    return true;
}

#endif //MRBASIC_MATERIALLOADER_H_H
