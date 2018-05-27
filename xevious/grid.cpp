#include "grid.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glut.h>

#include <iostream>
Grid:: Grid (const std::vector<Vertex> vertices, unsigned int r) : r(r)
{
    gridPoints.clear();
    glm::vec3 min;
    glm::vec3 max;
    bool first = true;
    for (auto v=vertices.begin(); v!=vertices.end();++v) {
        if (first) {
            min = v->pos;
            max = v->pos;
            first = false;
        } else {
            for (int i = 0; i < 3; i++) {
                min[i] = std::min(min[i], v->pos[i]);
                max[i] = std::max(max[i], v->pos[i]);
            }
        }

    }
    origin = min;
    size = std::max(max[0] - min[0],  std::max(max[1] - min[1], max[2] - min[2]));

    float edge = size / r;
    for (int x = 0; x < r; x++)
    {
        for (int y = 0; y < r; y++)
        {
            for (int z = 0; z < r; z++)
            {
                addCell(glm::vec3(edge * x, edge * y , edge * z) + origin, glm::vec3(edge * (x + 1), edge * (y + 1 ), edge * (z + 1)) + origin);
            }
        }
    }

    int i = 0;
    CellContent verticesInCell;
    for (auto v=vertices.begin(); v!=vertices.end();++v)
    {
        int cellIndex = isContainedAt(v->pos);
        verticesInCell[cellIndex].push_back(i);
        i++;
    }

    for (auto cellVertices=verticesInCell.begin(); cellVertices!=verticesInCell.end();++cellVertices)
    {
        Vertex result;
        float scale = 1.0 / cellVertices->second.size();
        for (auto v=cellVertices->second.begin(); v!=cellVertices->second.end();++v)
        {
            result.pos += vertices[*v].pos * scale;
            result.color += vertices[*v].color * scale;
            result.texCoord += vertices[*v].texCoord * scale;
        }

        representatives[cellVertices->first] = Vertex(result);
    }

    for (int i = 0; i < vertices.size(); i += 3)
    {
        int a = isContainedAt(vertices[i].pos);
        int b = isContainedAt(vertices[i + 1].pos);
        int c = isContainedAt(vertices[i + 2].pos);

        if (a != b && b != c && c != a)
        {
            simple.push_back(representatives[a]);
            simple.push_back(representatives[b]);
            simple.push_back(representatives[c]);
        }
    }

    for (int i = 0; i < simple.size(); i += 3)
    {
        Vertex a = simple[i];
        Vertex b = simple[i + 1];
        Vertex c = simple[i + 2];

        glm::vec3 u = b.pos - a.pos;
        glm::vec3 v = c.pos - a.pos;

        glm::vec3 n =  glm::normalize(glm::cross(u, v));

        simple[i].normal += n;
        simple[i+1].normal += n;
        simple[i+2].normal += n;
    }

    for (int i = 0; i < simple.size(); i ++)
    {
        simple[i].normal =   glm::normalize(simple[i].normal);

    }




    // Create a Vector Buffer Object that will store the vertices on video memory
    vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, gridPoints.size() * sizeof(glm::vec3), gridPoints.data(), GL_STATIC_DRAW);

    // Bind vertex data to shader inputs using their index (location)
    // These bindings are stored in the Vertex Array Object
    glGenVertexArrays(1, &(vao));
    glBindVertexArray(vao);

    // The position vectors should be retrieved from the specified Vertex Buffer Object with given offset and stride
    // Stride is the distance in bytes between vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(glGetAttribLocation(globals::boxProgram, "pos"), 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glEnableVertexAttribArray(0);
}

int Grid::isContainedAt(const glm::vec3 & pos){
    //returns index that contains the position
    float edgeSize = size / (float)r;

    int x = (pos[0] - origin[0]) / edgeSize;
    int y = (pos[1] - origin[1]) / edgeSize;
    int z = (pos[2] - origin[2]) / edgeSize;

    return (z * r + y) * r + x;
}

void Grid::addCell(const glm::vec3 & Min,const glm::vec3& Max) {

    const glm::vec3 corners[8] =
    {
        glm::vec3(Min[0],Min[1],Min[2]),
        glm::vec3(Max[0],Min[1],Min[2]),
        glm::vec3(Min[0],Max[1],Min[2]),
        glm::vec3(Max[0],Max[1],Min[2]),
        glm::vec3(Min[0],Min[1],Max[2]),
        glm::vec3(Max[0],Min[1],Max[2]),
        glm::vec3(Min[0],Max[1],Max[2]),
        glm::vec3(Max[0],Max[1],Max[2])
    };


    static const unsigned short faceCorners[6][4] =
    {
        { 0,4,6,2 },
        { 5,1,3,7 },
        { 0,1,5,4 },
        { 3,2,6,7 },
        { 0,2,3,1 },
        { 6,4,5,7 }
    };

    for (unsigned short f=0;f<6;++f)
    {
        const unsigned short* face = faceCorners[f];
        for(unsigned int v = 0; v<4; v++) {
            gridPoints.push_back(corners[face[v]]);
            gridPoints.push_back(corners[face[(v+1)%4]]);
        }

    }

}

void Grid::drawGrid( glm::mat4 projView){
    glUseProgram(globals::boxProgram);
    glLineWidth(4);
    glUniformMatrix4fv(glGetUniformLocation(globals::boxProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(projView));
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES,  0, gridPoints.size());
    glUseProgram(globals::mainProgram);
 //   drawVertices();
}


