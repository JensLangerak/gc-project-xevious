#include "terrain_generator.h"
#include "models.h"

TerrainGenerator::TerrainGenerator(double width, double height) :
 nextChunk(0),
 chunkHeight(height),
 chunkWidth(width)
{
    terrain1.model = models::ModelType::Terrain1;
    terrain1.texture = models::Textures::Sand;
    terrain1.position =  glm::vec3(0.,-3.5,height);
    terrain2.model = models::ModelType::Terrain2;
    terrain2.texture = models::Textures::Sand;
    terrain2.position =  glm::vec3(0.,-3.5,0.0);
    terrain3.model = models::ModelType::Terrain3;
    terrain3.texture = models::Textures::Sand;
    terrain3.position =  glm::vec3(0.,-3.5,-1*height);
    terrain4.model = models::ModelType::Terrain4;
    terrain4.texture = models::Textures::Sand;
    terrain4.position =  glm::vec3(0.,-3.5,-2*height);

    InitTerrainBuffers();
}

const int TERRAIN_WIDTH = 40;
const int TERRAIN_HEIGHT = 20;
const int TERRAIN_ARRAY_WIDTH = TERRAIN_WIDTH + 2; //need extra for normals;
const int TERRAIN_ARRAY_HEIGHT = TERRAIN_HEIGHT + 2;

void TerrainGenerator::UpdateChunk(Entity &chunk, bool update)
{
    // @NOTE: DIRTY TEMPORARY BAD HACK PLEASE FIX

    Vertex vertices[TERRAIN_ARRAY_WIDTH][TERRAIN_ARRAY_HEIGHT];

    for (int i = 0; i < TERRAIN_ARRAY_WIDTH; i++) {
        for (int j = 0; j < TERRAIN_ARRAY_HEIGHT; j++) {
            Vertex vertex = {};
            double x = (chunkWidth) / (TERRAIN_WIDTH-1) * (((double)i) - 0.5 * (TERRAIN_WIDTH - 1));
            double z = (chunkHeight) / (TERRAIN_HEIGHT-1) * (((double)j) - 0.5 * (TERRAIN_HEIGHT-1)) ;
            double rz = z - nextChunk * chunkHeight;
            double y = 0.9 * sin(3 * x + 3 * rz + 23.42)
                       + 0.7 * sin(sin(x * 0.3 + 0.3) * x  * 3+  3 * rz + 2.32)
                       + 0.8 * sin(3 * x + 0 * cos(x * 0.9) + 1.12)
                       + 0.6 * sin(cos(rz * 0.6) * x + 3 * rz + 6.32);

            //  y = 0;
            vertex.pos = glm::vec3(x, y, z);

            vertex.normal = glm::vec3(0, 0, 0);

            //define colors
            float c = y  ;
            c = c > 1 ? 1 : c < 0 ? 0 : c;
            vertex.color = glm::vec3(c, 1, c);

            vertex.texCoord = glm::vec2(x * 0.6,z * 0.6);

            vertices[i][j] = vertex;
        }
    }

    for (int i = 1; i < TERRAIN_ARRAY_WIDTH; i++) {
        for (int j = 1; j < TERRAIN_ARRAY_HEIGHT; j++) {
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

    for (int i = 0; i < TERRAIN_ARRAY_WIDTH; i++) {
        for (int j = 0; j < TERRAIN_ARRAY_HEIGHT; j++) {
            vertices[i][j].normal = glm::normalize(vertices[i][j].normal);

        }
    }

    models::Model * model = models::getModel(chunk.model);
    model->vertices.clear();
    for (int i = 2; i < TERRAIN_ARRAY_WIDTH - 1; i++) {
        for (int j = 2; j < TERRAIN_ARRAY_HEIGHT - 1; j++) {
            model->vertices.push_back(vertices[i -1][j - 1]);
            model->vertices.push_back(vertices[i -1][j]);
            model->vertices.push_back(vertices[i][j]);

            model->vertices.push_back(vertices[i -1][j - 1]);
            model->vertices.push_back(vertices[i][j - 1]);
            model->vertices.push_back(vertices[i][j]);
        }
    }

    nextChunk++;
    if (update) {
        glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
        glBufferData(GL_ARRAY_BUFFER, model->vertices.size() * sizeof(Vertex), model->vertices.data(), GL_STATIC_DRAW);
    } else {
        models::createModelBuffers(*model);
    }
}
void TerrainGenerator::InitTerrainBuffers(){

    //TODO
    UpdateChunk(terrain1, false);
    UpdateChunk(terrain2, false);

    UpdateChunk(terrain3, false);
    UpdateChunk(terrain4, false);
    models::createModelBuffers(*models::getModel(terrain1.model));
    models::createModelBuffers(*models::getModel(terrain2.model));
    models::createModelBuffers(*models::getModel(terrain3.model));
    models::createModelBuffers(*models::getModel(terrain4.model));


    terrain1.position =  glm::vec3(0.,-3.5, 1 * chunkHeight);
    terrain2.position =  glm::vec3(0.,-3.5, 0 * chunkHeight);
    terrain3.position =  glm::vec3(0.,-3.5,-1 * chunkHeight);
    terrain4.position =  glm::vec3(0.,-3.5,-2 * chunkHeight);
    lastUpdated = &terrain4;

}
void TerrainGenerator::UpdateChunks(double delta)
{
    terrain1.position.z += delta * 2.5;
    terrain2.position.z += delta * 2.5;
    terrain3.position.z += delta * 2.5;
    terrain4.position.z += delta * 2.5;

    if (terrain1.position.z > 2 * chunkHeight){
        UpdateChunk(terrain1, true);
        terrain1.position.z = lastUpdated->position.z - chunkHeight;
        std::cout << "update1 " <<std::endl;
        lastUpdated = &terrain1;
    }

    if (terrain2.position.z > 2 * chunkHeight){
        UpdateChunk(terrain2, true);
        terrain2.position.z = lastUpdated->position.z - chunkHeight;
        std::cout << "update 2" <<std::endl;
        lastUpdated = &terrain2;
    }

    if (terrain3.position.z > 2 * chunkHeight){
        UpdateChunk(terrain3, true);
        terrain3.position.z = lastUpdated->position.z - chunkHeight;
        std::cout << "update 3" <<std::endl;
        lastUpdated = &terrain3;
    }

    if (terrain4.position.z > 2 * chunkHeight){
        UpdateChunk(terrain4, true);
        terrain4.position.z = lastUpdated->position.z - chunkHeight;
        std::cout << "update 4" <<std::endl;
        lastUpdated = &terrain4;
    }
}