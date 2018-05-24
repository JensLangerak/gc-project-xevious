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
    std::vector<Vertex> dragonVertices;
    GLuint vbo;
    GLuint vao;
    
    bool loadModels() 
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, "dragon.obj")) {
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

                dragonVertices.push_back(vertex);
            }
        }
        
          // Create Vertex Buffer Object
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, models::dragonVertices.size() * sizeof(Vertex), dragonVertices.data(), GL_STATIC_DRAW);

        // Bind vertex data to shader inputs using their index (location)
        // These bindings are stored in the Vertex Array Object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // The position vectors should be retrieved from the specified Vertex Buffer Object with given offset and stride
        // Stride is the distance in bytes between vertices
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
        glEnableVertexAttribArray(0);

        // The normals should be retrieved from the same Vertex Buffer Object (glBindBuffer is optional)
        // The offset is different and the data should go to input 1 instead of 0
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(1);
    
    
        return true;
    }
}