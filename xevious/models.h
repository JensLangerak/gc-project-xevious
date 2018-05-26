#ifndef MODELS_H
#define MODELS_H

// Library for loading .OBJ model
#include <tiny_obj_loader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "bounding_box.h"

#include "utils.h"

namespace models {
    enum class ModelType{
        Dragon,
        PlayerShip,
        StarEnemy
    };
    
    struct Model {
        std::vector<Vertex> vertices;
        GLuint vbo;
        GLuint vao;
    };

    extern Model dragon;
    extern Model playerShip;
    extern Model starEnemy;

    BoundingCube makeBoundingCube(std::vector<Vertex> vertices);
    bool loadModels();
    void drawModel(ModelType model);
}

#endif



