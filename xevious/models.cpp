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
#include <GL/glut.h>
#include "utils.h"
#include "loadppm.h"


namespace models {    
    Model dragon;
    Model terrain;
    std::vector<GLuint> textures;
    
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
                
                vertex.color = glm::vec3(1,1,1);

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
        glVertexAttribPointer(glGetAttribLocation(globals::mainProgram, "pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
        glEnableVertexAttribArray(0);

        // The normals should be retrieved from the same Vertex Buffer Object (glBindBuffer is optional)
        // The offset is different and the data should go to input 1 instead of 0
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
        glVertexAttribPointer(glGetAttribLocation(globals::mainProgram, "normal"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(1);
        
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
        glVertexAttribPointer(glGetAttribLocation(globals::mainProgram, "texCoord"), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
        glEnableVertexAttribArray(2);
    
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
        glVertexAttribPointer(glGetAttribLocation(globals::mainProgram, "vertColor"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
        glEnableVertexAttribArray(3);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return true;
    }
    
    bool loadModels() 
    {
        bool result = loadModel(dragon, "resources/dragon.obj");
    
        return result;
    }
    
    void activateTexture(Textures texture)
    {
        if (texture == Textures::None) {
            glBindTexture(GL_TEXTURE_2D, 0);
            glUniform1i(glGetUniformLocation(globals::mainProgram, "useTexture"), 0);
            return;
        }
        
        glUniform1i(glGetUniformLocation(globals::mainProgram, "useTexture"), 1);
                
        glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[(int) texture]);
        glUniform1i(glGetUniformLocation(globals::mainProgram, "tex"), 0);
    }
    
    void loadTexture(int index, const char *filename)
    {    
        PPMImage image(filename);
        glGenTextures(1, &textures[index]);
        glBindTexture(GL_TEXTURE_2D, textures[index]);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.sizeX, image.sizeY, 
		GL_RGB, GL_UNSIGNED_BYTE, image.data);
        
        // Set behaviour for when texture coordinates are outside the [0, 1] range
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set interpolation for texture sampling (GL_NEAREST for no interpolation)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    bool loadTextures()
    {
        textures.resize(1);
        textures[0]=0;
        loadTexture((int) Textures::Sand, "resources/sand.ppm");
        
        return true;
    }
    
    void drawModel(ModelType modelType)
    {
        Model *model;
        switch(modelType) {
            case ModelType::Terrain:
                model = &terrain;
                break;
            case ModelType::Dragon:
            default:
                model = &dragon;
                break;
        }
    
        // Bind vertex data
        glBindVertexArray(model->vao);


        // Execute draw command
        glDrawArrays(GL_TRIANGLES, 0, model->vertices.size());
    }
    
    void generateTerrain(double sizeX, double sizeZ, int nbVertX, int nbVertZ){
        Vertex vertices[nbVertX][nbVertZ];
        for (int i = 0; i < nbVertX; i++) {
            for (int j = 0; j < nbVertZ; j++) {
                 Vertex vertex = {};
                 double x = sizeX / nbVertX * (i - 0.5 * nbVertX);
                 double z = sizeZ / nbVertZ * (j - 0.5 * nbVertZ);
                 double y = 0.9 * sin(1 * x + 1 * z + 23.42)
                  + 0.7 * sin(sin(x * 0.1 + 0.3) * x + 1 * z + 2.32)
                    + 0.8 * sin(1 * x + 0 * cos(x * 0.3) + 1.12)
                    + 0.6 * sin(cos(z * 0.2) * x + 1 * z + 6.32);

                 vertex.pos = glm::vec3(x, y, z);
                 
                 vertex.normal = glm::vec3(0, 0, 0);
               
               //define colors
                float c = y  ;
                c = c > 1 ? 1 : c < 0 ? 0 : c;
                vertex.color = glm::vec3(c, 1, c);
                
                 vertex.texCoord = glm::vec2(x * 0.2,z * 0.2);
                 
                 vertices[i][j] = vertex;
            }
        }
        
        for (int i = 1; i < nbVertX; i++) {
            for (int j = 1; j < nbVertZ; j++) {
                glm::vec3 u = vertices[i - 1][j - 1].pos - vertices[i][j].pos;
                glm::vec3 v = vertices[i - 1][j].pos - vertices[i][j].pos;

                glm::vec3 n =  glm::normalize(glm::cross(u, v));

                vertices[i - 1][j - 1].normal += n;
                vertices[i - 1][j].normal += n;
                vertices[i][j].normal += n;
                
                
                v = vertices[i - 1][j - 1].pos - vertices[i][j].pos;
                u = vertices[i][j - 1].pos - vertices[i][j].pos;

                n =  glm::normalize(glm::cross(u, v));

                vertices[i - 1][j - 1].normal += n;
                vertices[i][j - 1].normal += n;
                vertices[i][j].normal += n;
              
            }
        }
        
        for (int i = 0; i < nbVertX; i++) {
            for (int j = 0; j < nbVertZ; j++) {
                vertices[i][j].normal = glm::normalize(vertices[i][j].normal);
        
            }
        }
        for (int i = 1; i < nbVertX; i++) {
            for (int j = 1; j < nbVertZ; j++) {
                terrain.vertices.push_back(vertices[i -1][j - 1]);
                terrain.vertices.push_back(vertices[i -1][j]);
                terrain.vertices.push_back(vertices[i][j]);
                
                terrain.vertices.push_back(vertices[i -1][j - 1]);
                terrain.vertices.push_back(vertices[i][j - 1]);
                terrain.vertices.push_back(vertices[i][j]);
            }
        }
        
        
        
              // Create Vertex Buffer Object
        glGenBuffers(1, &(terrain.vbo));
        glBindBuffer(GL_ARRAY_BUFFER, terrain.vbo);
        glBufferData(GL_ARRAY_BUFFER, terrain.vertices.size() * sizeof(Vertex), terrain.vertices.data(), GL_STATIC_DRAW);

        // Bind vertex data to shader inputs using their index (location)
        // These bindings are stored in the Vertex Array Object
        glGenVertexArrays(1, &(terrain.vao));
        glBindVertexArray(terrain.vao);

        // The position vectors should be retrieved from the specified Vertex Buffer Object with given offset and stride
        // Stride is the distance in bytes between vertices
        glBindBuffer(GL_ARRAY_BUFFER, terrain.vbo);
        glVertexAttribPointer(glGetAttribLocation(globals::mainProgram, "pos"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
        glEnableVertexAttribArray(0);

        // The normals should be retrieved from the same Vertex Buffer Object (glBindBuffer is optional)
        // The offset is different and the data should go to input 1 instead of 0
        glBindBuffer(GL_ARRAY_BUFFER, terrain.vbo);
        glVertexAttribPointer(glGetAttribLocation(globals::mainProgram, "normal"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(1);
    
        glBindBuffer(GL_ARRAY_BUFFER, terrain.vbo);
        glVertexAttribPointer(glGetAttribLocation(globals::mainProgram, "texCoord"), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));
        glEnableVertexAttribArray(2);
        
        glBindBuffer(GL_ARRAY_BUFFER, terrain.vbo);
        glVertexAttribPointer(glGetAttribLocation(globals::mainProgram, "vertColor"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
        glEnableVertexAttribArray(3);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}