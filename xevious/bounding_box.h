#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class BoundingBox
{
public:
	//BoundingBox();
	BoundingBox(double x, double y, double width, double height);
	BoundingBox(BoundingBox box, glm::vec2 translation);
	
	~BoundingBox();

	bool checkIntersection(BoundingBox other);

	void draw(glm::mat4 projView, glm::vec3 color);
	void draw(glm::mat4 projView); // @NOTE: This may need a transformation matrix to be able to succeed

	// @TEST: After debugging, this should be moved into protected
	glm::vec2 topLeft;
	glm::vec2 dimensions;

protected:
};


#endif