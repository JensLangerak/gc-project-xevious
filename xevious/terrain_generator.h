//
// Created by jens on 29-5-18.
//

#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include "entity.h"

class TerrainGenerator
{
public:
    TerrainGenerator(double width, double height);

    void UpdateChunk(Entity &chunk, bool update);
    void InitTerrainBuffers();
    void UpdateChunks(double delta);
    Entity terrain1;
    Entity terrain2;
protected:
    int nextChunk;
    double chunkHeight;
    double chunkWidth;

};
#endif //TERRAIN_GENERATOR_H
