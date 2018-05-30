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
        Terrain1,
        Terrain2,
        Terrain3,
        Terrain4,
        Simple,

        Bullet,
        PlayerShip,
        PlayerGun,
        StarEnemy,

        BossDetailLevel1,
        BossDetailLevel2,
        BossDetailLevel3
    };
    
    enum class Textures{
        None = -1,
        Sand = 0,
        Beam1,
        Beam2
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
    extern Model bullet;

    extern Model bossDetailLevel1;
    extern Model bossDetailLevel2;
    extern Model bossDetailLevel3;

    BoundingCube makeBoundingCube(std::vector<Vertex> vertices);

    void activateTexture(Textures texture);
    bool loadModels();

    bool loadSimple(std::vector<Vertex>);

    bool loadTextures();
    void drawModel(ModelType model);

    void generateSimpleModel(const std::vector<Vertex> vertices, unsigned int r);

    Model *getModel(ModelType model);
    bool createModelBuffers(Model &model);
}

#endif



