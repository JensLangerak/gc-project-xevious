#ifndef MODELS_H
#define MODELS_H

// Library for loading .OBJ model
#include <tiny_obj_loader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


#include "utils.h"

namespace models {
    enum class ModelType{
        Dragon,
        Terrain,
        Simple
    };
    
    enum class Textures{
        None = -1,
        Sand = 0
    };
    
    struct Model {
        std::vector<Vertex> vertices;
        GLuint vbo;
        GLuint vao;
    };
    extern Model dragon;
    void activateTexture(Textures texture);
    bool loadModels();

    bool loadSimple(std::vector<Vertex>);

    bool loadTextures();
    void drawModel(ModelType model);
    void generateTerrain(double sizeX, double sizeZ, int nbVertX, int nbVertZ);
}

#endif



