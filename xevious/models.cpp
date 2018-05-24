#define GLEW_STATIC
#include <GL/glew.h>

// Library for window creation and event handling
#include <GLFW/glfw3.h>

// Library for vertex and matrix math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Library for loading .OBJ model
#include <tiny_obj_loader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "models.h"

#include "utils.h"


namespace models {    
    Model dragon;
    
    
    bool loadModel(Model &model, const char *filename)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename)) {
            std::cerr << err << std::endl;
            return false;
        }

        // Read triangle vertices from OBJ file
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex = {};

                // Retrieve coordinates for vertex by index
                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                // Retrieve components of normal by index
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };

                model.vertices.push_back(vertex);
            }
        }
        
          // Create Vertex Buffer Object
        glGenBuffers(1, &(model.vbo));
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
        glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.vertices.data(), GL_STATIC_DRAW);

        // Bind vertex data to shader inputs using their index (location)
        // These bindings are stored in the Vertex Array Object
        glGenVertexArrays(1, &(model.vao));
        glBindVertexArray(model.vao);

        // The position vectors should be retrieved from the specified Vertex Buffer Object with given offset and stride
        // Stride is the distance in bytes between vertices
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
        glEnableVertexAttribArray(0);

        // The normals should be retrieved from the same Vertex Buffer Object (glBindBuffer is optional)
        // The offset is different and the data should go to input 1 instead of 0
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(1);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return true;
    }
    
    bool loadModels() 
    {
        bool result = loadModel(dragon, "dragon.obj");    
    
        return result;
    }
    
    void drawModel(ModelType modelType)
    {
        Model *model;
        switch(modelType) {
            case ModelType::Dragon:
            default:
                model = &dragon;
        }
    
        // Bind vertex data
        glBindVertexArray(model->vao);

        // Execute draw command
        glDrawArrays(GL_TRIANGLES, 0, model->vertices.size());
    }
}