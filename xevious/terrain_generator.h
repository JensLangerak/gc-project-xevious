//
// Created by jens on 29-5-18.
//

#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "entity.h"


#define NUMBER_OF_CHUNKS 4

class TerrainGenerator
{
public:
    TerrainGenerator(double width, double height);

    void UpdateChunk(Entity &chunk, bool update);
    void InitTerrainBuffers();
    void UpdateChunks(double delta);
    void drawChunks(long tick , glm::mat4 projView);
    Entity chunks[NUMBER_OF_CHUNKS];

    Entity * lastUpdated;
protected:
    int nextChunk;
    double chunkHeight;
    double chunkWidth;



};
#endif //TERRAIN_GENERATOR_H
