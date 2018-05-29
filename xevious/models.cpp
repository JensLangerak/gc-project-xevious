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
#include <float.h>
#include "bounding_box.h"
#include "loadppm.h"
#include "mesh_simplification.h"

namespace models {    
    Model dragon;
    Model playerShip;
    Model playerGun;
    Model starEnemy;
    Model terrain1;
    Model terrain2;
    Model terrain3;
    Model terrain4;

    Model bullet;
    
    Model simple;
    Model david;

    Model bossDetailLevel1;
    Model bossDetailLevel2;
    Model bossDetailLevel3;

    std::vector<GLuint> textures;

    BoundingCube makeBoundingCube(std::vector<Vertex> vertices)
    {
        // Find corner points (minX, minY, minZ, maxX, maxY, maxZ)
        float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
        float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

        for (unsigned int i = 0; i < vertices.size(); ++i)
        {
            glm::vec3 vertex = vertices[i].pos;
            minX = fmin(minX, vertex.x);
            minY = fmin(minY, vertex.y);
            minZ = fmin(minZ, vertex.z);

            maxX = fmax(maxX, vertex.x);
            maxY = fmax(maxY, vertex.y);
            maxZ = fmax(maxZ, vertex.z);
        }

        // Find farLowerLeft;
        // @NOTE: Which coordinate would be most intuitive?
        // y grows upwards from min
        // x grows rightwards from left
        // z grows .... from near
        glm::vec3 farLowerLeft = glm::vec3(minX, minY, minZ);

        // Calculate dimensions
        glm::vec3 dims = glm::vec3(maxX - minX, maxY - minY, maxZ - minZ);

        // Construct BoundingCube
        return BoundingCube(farLowerLeft, dims);
    }

    bool createModelBuffers(Model &model)
    {
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


        // @TODO: Generate bounding cube, not here also used by terrain etc...

        return true;
    }

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

        model.vertices.clear();
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


        return createModelBuffers(model);
    }

    bool loadSimplifiedMesh(std::vector<Vertex> vertices, ModelType into)
    {
        Model* model;
        switch (into)
        {
            case ModelType::BossDetailLevel1:
                model = &bossDetailLevel1;
                break;
            case ModelType::BossDetailLevel2:
                model = &bossDetailLevel2;
                break;
            case ModelType::BossDetailLevel3:
                model = &bossDetailLevel3;
                break;
            default:
            // load into simple
				break;	
        }

        model->vertices.clear();
        for (auto v=vertices.begin(); v!=vertices.end();++v)
        {
            model->vertices.push_back(*v);
        }

        // @NOTE: From pointer to reference is kind of ugly.
        return createModelBuffers(*model);
    }
    

    void loadBullet()
    {
        bullet.vertices.clear();
        Vertex bottomLeft = {{-1, 0, 1}, {0, 1, 0}, {0, 0, 0}, {0, 1}};
        Vertex bottomRight = {{1, 0, 1}, {0, 1, 0}, {0, 0, 0}, {1, 1}}; 
        Vertex topRight = {{1, 0, -1}, {0, 1, 0}, {0, 0, 0}, {1, 0}};
        Vertex topLeft = {{-1, 0, -1}, {0, 1, 0}, {0, 0, 0}, {0, 0}}; 

        bullet.vertices.push_back(bottomLeft);
        bullet.vertices.push_back(bottomRight);
        bullet.vertices.push_back(topRight);

        bullet.vertices.push_back(topRight);
        bullet.vertices.push_back(topLeft);
        bullet.vertices.push_back(bottomLeft);



        createModelBuffers(bullet);
    }

    bool loadModels() 
    {
        bool result = loadModel(dragon, "resources/dragon.obj"); 
        bool result2 = loadModel(playerShip, "resources/ship.obj");
        bool result3 = loadModel(starEnemy, "resources/starship.obj");
        bool result4 = loadModel(playerGun, "resources/cannon.obj");
        bool result5 = loadModel(david, "resources/dragon.obj");

        loadBullet();

        // Load levels of detail into bosses
        MeshSimplification mesh = MeshSimplification(dragon.vertices, 20);
        models::loadSimplifiedMesh(mesh.simplifiedMesh, ModelType::BossDetailLevel1);
        mesh = MeshSimplification(dragon.vertices, 10);
        models::loadSimplifiedMesh(mesh.simplifiedMesh, ModelType::BossDetailLevel2);
        mesh = MeshSimplification(dragon.vertices, 5);
        models::loadSimplifiedMesh(mesh.simplifiedMesh, ModelType::BossDetailLevel3);


        return result && result2 && result3 && result4;
    }
    
    void activateTexture(Textures texture)
    {

        if (texture == Textures::None) {
            //glBindTexture(GL_TEXTURE_2D, 0);
            glUniform1i(glGetUniformLocation(globals::mainProgram, "useTexture"), 0);
            return;
        }

        //return;

        glUniform1i(glGetUniformLocation(globals::mainProgram, "useTexture"), 1);

        glActiveTexture(GL_TEXTURE0 + textures[(int) texture]);
		glBindTexture(GL_TEXTURE_2D, textures[(int) texture]);
        glUniform1i(glGetUniformLocation(globals::mainProgram, "tex"), textures[(int) texture]);
    }
    
    void loadTexture(int index, const char *filename)
    {    
        PPMImage image(filename);
        glGenTextures(1, &textures[index]);
        glActiveTexture( GL_TEXTURE0 +  textures[index]);
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

    Model* getModel(ModelType modelType)
    {
        Model *model;
        switch(modelType) {
            case ModelType::Terrain1:
                model = &terrain1;
                break;
            case ModelType::Terrain2:
                model = &terrain2;
                break;
            case ModelType::Terrain3:
                model = &terrain3;
                break;
            case ModelType::Terrain4:
                model = &terrain4;
                break;
            case ModelType::Simple:
                model = &simple;
                break;
            case ModelType::Dragon:
                model = &dragon;
                break;
            case ModelType::PlayerShip:
                model = &playerShip;
                break;
            case ModelType::PlayerGun:
                model = &playerGun;
                break;
            case ModelType::BossDetailLevel1:
                model = &bossDetailLevel1;
                break;
            case ModelType::BossDetailLevel2:
                model = &bossDetailLevel2;
                break;
            case ModelType::BossDetailLevel3:
                model = &bossDetailLevel3;
                break;
            case ModelType::Bullet:
                model = &bullet;
                break;
            case ModelType::StarEnemy:
            default:
                model = &starEnemy;
        }
        return model;
    }
    void drawModel(ModelType modelType)
    {
        Model *model = getModel(modelType);

    
        // Bind vertex data
        glBindVertexArray(model->vao);


        // Execute draw command
        glDrawArrays(GL_TRIANGLES, 0, model->vertices.size());
    }

}