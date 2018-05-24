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
        Dragon
    };
    
    struct Model {
        std::vector<Vertex> vertices;
        GLuint vbo;
        GLuint vao;
    };

    extern Model dragon;
    
    bool loadModels();
    void drawModel(ModelType model);
    
}

#endif



