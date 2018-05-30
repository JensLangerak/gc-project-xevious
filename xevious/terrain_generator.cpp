#include "terrain_generator.h"
#include "models.h"

TerrainGenerator::TerrainGenerator(double width, double height) :
 nextChunk(0),
 chunkHeight(height),
 chunkWidth(width)
{
    for (int i = 0; i < NUMBER_OF_CHUNKS; i++) {
        chunks[i].texture = models::Textures::Sand;
        chunks[i].position =  glm::vec3(0.,-3.5,-(i-1)*height);
    }

    chunks[0].model = models::ModelType::Terrain1;
    chunks[1].model = models::ModelType::Terrain2;
    chunks[2].model = models::ModelType::Terrain3;
    chunks[3].model = models::ModelType::Terrain4;

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

    for (int i = 0; i < NUMBER_OF_CHUNKS; i++) {

        UpdateChunk(chunks[i], false);


        models::createModelBuffers(*models::getModel(chunks[i].model));

        chunks[i].position = glm::vec3(0., -3.5, -(i - 1) * chunkHeight);
    }
    lastUpdated = &chunks[3];

}

void TerrainGenerator::UpdateChunks(double delta)
{
    for (int i = 0; i < NUMBER_OF_CHUNKS; i++) {
        chunks[i].position.z += delta * 1.;
    }

    for (int i = 0; i < NUMBER_OF_CHUNKS; i++) {

        if (chunks[i].position.z > 2 * chunkHeight) {
            UpdateChunk(chunks[i], true);
            chunks[i].position.z = lastUpdated->position.z - chunkHeight;
            lastUpdated = &chunks[i];
        }
    }
}

void TerrainGenerator::drawChunks(long tick , glm::mat4 projView)
{
    for (int i = 0; i < NUMBER_OF_CHUNKS; i++) {
        chunks[i].draw(tick, projView);
    }
}