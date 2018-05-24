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
    extern std::vector<Vertex> dragonVertices;
    extern GLuint vbo;
    extern GLuint vao;
    
    bool loadModels();
    
}

#endif



