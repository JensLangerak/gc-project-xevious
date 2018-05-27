#define GLEW_STATIC
#include <GL/glew.h>

// Library for vertex and matrix math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "bounding_box.h"
#include "utils.h"
#include "models.h"

#include <iostream>
#include <float.h>

BoundingBox::BoundingBox(double x, double y, double width, double height)
{
	topLeft = glm::vec2(x, y);
	dimensions = glm::vec2(width, height);
}

BoundingBox::BoundingBox(BoundingBox box, glm::vec2 translation)
{
	topLeft = box.topLeft + translation;
	dimensions = box.dimensions;
}

BoundingBox::~BoundingBox()
{
}

// @TODO: Once testing is finished, this can probably be removed
void BoundingBox::draw(glm::mat4 projView)
{
	float HEIGHT_VALUE = 0.5;
	draw(projView, glm::vec3(0., 0., 0.1), HEIGHT_VALUE);
}

void BoundingBox::draw(glm::mat4 projView, glm::vec3 drawColor)
{
	float HEIGHT_VALUE = 0.5;
	draw(projView, drawColor, HEIGHT_VALUE);	
}

void BoundingBox::draw(glm::mat4 projView, glm::vec3 drawColor, float height)
{
	// @TODO: Remove color-defects from drawing bounding boxes;
	// @TODO: Draw bounding boxes around objects instead of on top of them
	// @TODO: What normal to use for vertices? (currently using 1.0, 1.0, 1.0)
	// @NOTE: Assuming that origin is in topleft corner of screen, and both axis grow positive
	// @TODO: Scale coordinates from gameplay-size to world-coordinates
	// 1. Create vertex array
	Vertex vTopLeft = {};
	vTopLeft.pos = {topLeft.x , height, -topLeft.y};
	vTopLeft.normal = {1.0, 1.0, 1.0};

	Vertex vTopRight = {};
	vTopRight.pos = {topLeft.x + dimensions.x , height, -topLeft.y};
	vTopRight.normal = {1.0, 1.0, 1.0};

	Vertex vBottomLeft = {};
	vBottomLeft.pos = {topLeft.x, height, -(topLeft.y + dimensions.y)};
	vBottomLeft.normal = {1.0, 1.0, 1.0};

	Vertex vBottomRight = {};
	vBottomRight.pos = {topLeft.x + dimensions.x, height, -(topLeft.y + dimensions.y)};
	vBottomRight.normal = {1.0, 1.0, 1.0};

	// Counterclockwise orientation
	Vertex vertexList[4] = {vTopLeft, vBottomLeft, vBottomRight, vTopRight};

	// 2. Load vertex data into buffer object
	glBindBuffer(GL_ARRAY_BUFFER, globals::boundingBoxVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), vertexList, GL_STATIC_DRAW);

	// 3. Set projection matrix
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(projView));	
	glUniform3fv(glGetUniformLocation(globals::mainProgram, "color"), 1, glm::value_ptr(drawColor));

	// 4. Bind vertex data to correct shader inputs
	glBindVertexArray(globals::boundingBoxVAO);

	// 5. Issue draw call
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

bool BoundingBox::checkIntersection(BoundingBox other)
{
	return !(this->topLeft.x + this->dimensions.x < other.topLeft.x
			|| this->topLeft.x > other.topLeft.x + this->dimensions.x
			|| this->topLeft.y + this->dimensions.y < other.topLeft.y
			|| this->topLeft.y > other.topLeft.y + this->dimensions.y);

	// Box is either to the left, right, bottom or top if the box does not intersect
	// to the left: rightmost edge is to the left of left edge
	// to the right: leftmost edge is to the right of right edge
	// to the top: bottom edge is above top edge of other
}

void BoundingBox::print()
{
	std::cout << "BB2: " << topLeft.x << ", " << topLeft.y << " dims: "
				<< dimensions.x << ", " << dimensions.y << std::endl << std::endl;

}







BoundingCube::BoundingCube()
{
}

BoundingCube::~BoundingCube()
{
}

BoundingCube::BoundingCube(glm::vec3 farBottomLeft, glm::vec3 dimensions)
{
	// Initialize variables
	this->farBottomLeft = farBottomLeft;
	this->dimensions = dimensions;
}


void BoundingCube::draw(glm::mat4 projViewModel)
{
	glm::vec3 drawColor = glm::vec3(0.0, 1.0, 1.0);
	BoundingCube::draw(projViewModel, drawColor);
}

void BoundingCube::draw(glm::mat4 projViewModel, glm::vec3 drawColor)
{
	// 1. Create vertex array
	Vertex vNearBottomLeft = {};
	vNearBottomLeft.pos = { farBottomLeft.x, farBottomLeft.y, farBottomLeft.z + dimensions.z };
	vNearBottomLeft.normal = {1., 1., 1.};

	Vertex vNearBottomRight = {};
	vNearBottomRight.pos = {farBottomLeft.x + dimensions.x, farBottomLeft.y, farBottomLeft.z + dimensions.z};
	vNearBottomRight.normal = {1., 1., 1.};

	Vertex vFarBottomRight = {};
	vFarBottomRight.pos = {farBottomLeft.x + dimensions.x, farBottomLeft.y, farBottomLeft.z};
	vFarBottomRight.normal = {1., 1., 1.};

	Vertex vFarBottomLeft = {};
	vFarBottomLeft.pos = {farBottomLeft.x, farBottomLeft.y, farBottomLeft.z};
	vFarBottomLeft.normal = {1., 1., 1.};

	Vertex vNearTopLeft = {};
	vNearTopLeft.pos = {farBottomLeft.x, farBottomLeft.y + dimensions.y, farBottomLeft.z + dimensions.z};
	vNearTopLeft.normal = {1., 1., 1.};

	Vertex vNearTopRight = {};
	vNearTopRight.pos = {farBottomLeft.x + dimensions.x, farBottomLeft.y + dimensions.y, farBottomLeft.z + dimensions.z};
	vNearTopRight.normal = {1., 1., 1.};

	Vertex vFarTopRight = {};
	vFarTopRight.pos = {farBottomLeft.x + dimensions.x, farBottomLeft.y + dimensions.y, farBottomLeft.z};
	vFarTopRight.normal = {1., 1., 1.};

	Vertex vFarTopLeft = {};
	vFarTopLeft.pos = {farBottomLeft.x, farBottomLeft.y + dimensions.y, farBottomLeft.z};
	vFarTopLeft.normal = {1., 1., 1.};


	Vertex vertexList[8] = {vNearBottomLeft, vNearBottomRight, vFarBottomRight, vFarBottomLeft, 
							vNearTopLeft, vNearTopRight, vFarTopRight, vFarTopLeft};

	// @TODO: Let's do 2 simple rings for now (no vertical edges)

	// 2. Load vertex data into buffer object
	glBindBuffer(GL_ARRAY_BUFFER, globals::boundingBoxVBO);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(Vertex), vertexList, GL_STATIC_DRAW);

	// 3. Set projection matrix
    glUniformMatrix4fv(glGetUniformLocation(globals::debugProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(projViewModel));	
	glUniform3fv(glGetUniformLocation(globals::debugProgram, "color"), 1, glm::value_ptr(drawColor));

	// 4. Bind vertex data to correct shader inputs
	glBindVertexArray(globals::boundingBoxVAO);

	// 5. Issue draw calls
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	glDrawArrays(GL_LINE_LOOP, 4, 4);
}



//@DEBUG: Method for printing the basic information of a bounding box;
void BoundingCube::print()
{
	std::cout << "BoundingBox pos: " << farBottomLeft.x << ", " << farBottomLeft.y << ", " << farBottomLeft.z 
				<< " dims: " << dimensions.x << ", " << dimensions.y << ", " << dimensions.z << std::endl;  
}

// @TODO: Implement
/* Rotates the bounding cube by the given orientation, and provides a BoundingBox
 *	That indicates the ground area that bounding cube would cover 
 *  (occluded view when viewing from orthographic top perspective)
 * @NOTE: Assumes that bounding cube is not affine! (for speedup)
 */
BoundingBox BoundingCube::getProjectedBoundingBox(glm::mat4 transform)
{
	// Fetch and rotate the corner and the dimensions
	glm::vec3 vNearBottomLeft = glm::vec3(farBottomLeft.x, farBottomLeft.y, farBottomLeft.z + dimensions.z);
	glm::vec3 vNearBottomRight = glm::vec3(farBottomLeft.x + dimensions.x, farBottomLeft.y, farBottomLeft.z + dimensions.z);
	glm::vec3 vFarBottomRight = glm::vec3(farBottomLeft.x + dimensions.x, farBottomLeft.y, farBottomLeft.z);
	glm::vec3 vFarBottomLeft = glm::vec3(farBottomLeft.x, farBottomLeft.y, farBottomLeft.z);
	glm::vec3 vNearTopLeft = glm::vec3(farBottomLeft.x, farBottomLeft.y + dimensions.y, farBottomLeft.z + dimensions.z);
	glm::vec3 vNearTopRight = glm::vec3(farBottomLeft.x + dimensions.x, farBottomLeft.y + dimensions.y, farBottomLeft.z + dimensions.z);
	glm::vec3 vFarTopRight = glm::vec3(farBottomLeft.x + dimensions.x, farBottomLeft.y + dimensions.y, farBottomLeft.z);
	glm::vec3 vFarTopLeft = glm::vec3(farBottomLeft.x, farBottomLeft.y + dimensions.y, farBottomLeft.z);

	glm::vec3 vertexList[8] = {vNearBottomLeft, vNearBottomRight, vFarBottomRight, vFarBottomLeft, 
							vNearTopLeft, vNearTopRight, vFarTopRight, vFarTopLeft};

	float minX = FLT_MAX;
	float minZ = FLT_MAX;
	float maxX = -FLT_MAX;
	float maxZ = -FLT_MAX;

	for (int i = 0; i < 8; ++i)
	{
		glm::vec4 point = transform * glm::vec4(vertexList[i].x, vertexList[i].y, vertexList[i].z, 1.0);
		minX = fmin(minX, point.x);
		maxX = fmax(maxX, point.x);
		minZ = fmin(minZ, point.z);
		maxZ = fmax(maxZ, point.z);	
	}

	// Construct bounding box
	return BoundingBox(minX, -minZ, maxX - minX, maxZ - minZ);
}