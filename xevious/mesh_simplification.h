#ifndef GRID_H
#define GRID_H

#include <vector>
#include "utils.h"
#include <map>

typedef std::map<unsigned, std::vector<unsigned int> > CellContent;
typedef std::map<unsigned, Vertex> RepresentativeList;

class MeshSimplification
{
public:
    MeshSimplification (std::vector<Vertex>, unsigned int r);

    void init();
	//The corners with the smallest and largest coordinates.
    void addCell(const glm::vec3 & min,const glm::vec3& Max);
	//draw all the cells
    void drawGrid( glm::mat4 projView);

	//number of grid cells
    unsigned int r;
    //position of the grid (min corner is at origin and its extent is defined by size).
    glm::vec3 origin;
    float size;
	//remark: for simplicity, we work with a cube and the size/extent values are the same for all axes.

    int isContainedAt(const glm::vec3 & pos);

    GLuint vao;
    GLuint vbo;
    std::vector<glm::vec3> gridPoints;
    RepresentativeList representatives;

    std::vector<Vertex> simplifiedMesh;

protected:
    void createGrid(const std::vector<Vertex> vertices);
    void putVerticesInCells(const std::vector<Vertex> vertices, CellContent &verticesInCell);
    void calculateRepresentatices(const std::vector<Vertex> vertices, CellContent &verticesInCell);
    void constructTriangles(const std::vector<Vertex> vertices);
    void recalculateNormals();
};

#endif // GRID_H
