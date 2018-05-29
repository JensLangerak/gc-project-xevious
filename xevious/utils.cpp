#include "utils.h"


namespace globals {  
    bool debugMode = false;
    GLuint mainProgram;
    GLuint debugProgram;
    GLuint boundingBoxVBO;
    GLuint boundingBoxVAO;
}

glm::mat4 getTranslationMatrix(glm::vec3 vecTranslation)
{
    glm::vec4 vec4Translation = glm::vec4(vecTranslation, 1.0);
    glm::mat4 translation = glm::mat4();
    translation[0][0] = 1.;
    translation[1][1] = 1.;
    translation[2][2] = 1.;
    translation[3] = vec4Translation;
    return translation;
}


glm::mat4 getRotationMatrix(double xRot ,double yRot ,double zRot)
{
	glm::mat4 model = glm::mat4();
    model[0][0] = cos(yRot) * cos(zRot);
    model[0][1] = cos(yRot) * sin(zRot);
    model[0][2] = -sin(yRot);
    model[1][0] = sin(xRot) * sin(yRot) * cos(zRot) - cos(xRot) * sin(zRot);
    model[1][1] = sin(xRot) * sin(yRot) * sin(zRot) + cos(xRot) * cos(zRot);
    model[1][2] = sin(xRot) * cos(yRot);
    model[2][0] = cos(xRot) * sin(yRot) * cos(zRot) + sin(xRot) * sin(zRot);
    model[2][1] = cos(xRot) * sin(yRot) * sin(zRot) - sin(xRot) * sin(zRot);
    model[2][2] = cos(xRot) * cos(yRot);

    return model;
}

glm::mat4 getScalingMatrix(double scale)
{
    glm::mat4 matrix = glm::mat4();
    matrix[0][0] = scale;
    matrix[1][1] = scale;
    matrix[2][2] = scale;
    matrix[3][3] = 1.;
    return matrix;
}