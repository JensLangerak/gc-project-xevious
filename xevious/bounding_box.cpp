#define GLEW_STATIC
#include <GL/glew.h>

// Library for vertex and matrix math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "bounding_box.h"
#include "utils.h"
#include "models.h"

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
	draw(projView, glm::vec3(0., 0., 0.1));
}

void BoundingBox::draw(glm::mat4 projView, glm::vec3 drawColor)
{
	// @TODO: Remove color-defects from drawing bounding boxes;
	// @TODO: Draw bounding boxes around objects instead of on top of them
	// @TODO: What normal to use for vertices? (currently using 1.0, 1.0, 1.0)
	// @TODO: What 3Dy value to use for vertices? (currently using 0.5);
	// @NOTE: Assuming that origin is in topleft corner of screen, and both axis grow positive
	// @TODO: Scale coordinates from gameplay-size to world-coordinates
	// 1. Create vertex array
	float HEIGHT_VALUE = 0.5;
	Vertex vTopLeft = {};
	vTopLeft.pos = {topLeft.x , HEIGHT_VALUE, -topLeft.y};
	vTopLeft.normal = {1.0, 1.0, 1.0};

	Vertex vTopRight = {};
	vTopRight.pos = {topLeft.x + dimensions.x , HEIGHT_VALUE, -topLeft.y};
	vTopRight.normal = {1.0, 1.0, 1.0};

	Vertex vBottomLeft = {};
	vBottomLeft.pos = {topLeft.x, HEIGHT_VALUE, -(topLeft.y + dimensions.y)};
	vBottomLeft.normal = {1.0, 1.0, 1.0};

	Vertex vBottomRight = {};
	vBottomRight.pos = {topLeft.x + dimensions.x, HEIGHT_VALUE, -(topLeft.y + dimensions.y)};
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
