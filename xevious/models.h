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
        David,
        Terrain,
        Simple,

        PlayerShip,
        PlayerGun,
        StarEnemy

    };
    
    enum class Textures{
        None = -1,
        Sand = 0,
    };
    
    struct Model {
        std::vector<Vertex> vertices;
        GLuint vbo;
        GLuint vao;
    };

    extern Model dragon;
    extern Model playerShip;
    extern Model starEnemy;
    extern Model david;

    BoundingCube makeBoundingCube(std::vector<Vertex> vertices);

    void activateTexture(Textures texture);
    bool loadModels();

    bool loadSimple(std::vector<Vertex>);

    bool loadTextures();
    void drawModel(ModelType model);
    void generateTerrain(double sizeX, double sizeZ, int nbVertX, int nbVertZ);

    void generateSimpleModel(const std::vector<Vertex> vertices, unsigned int r);
}

#endif



