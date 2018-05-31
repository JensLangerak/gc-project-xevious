#include "entity.h"
#include "bullet_entity.h"
#include <iostream>
#include "models.h"

#define PI 3.14
#define FLASH_DURATION 0.3
#define BULLET_SCALE 0.02

BulletEntity::~BulletEntity()
{
}

BulletEntity::BulletEntity(glm::vec3 pos, glm::vec3 dir)
{
	// Rendering
	color = glm::vec3(1., 1., 1.);
	texture = models::Textures::Beam1;
	scale = BULLET_SCALE;
	hasCollided = false;
	flashRemaining = 0;

	// Gameplay
	position = pos;
	direction = dir;
	model = models::ModelType::Bullet;
	boundingCube = models::makeBoundingCube(models::starEnemy.vertices);
	type = EntityType::Bullet;
}

void BulletEntity::update(double tick, Gamestate* state )
{
	// @NOTE: Perhaps move velocity into global, or into constructor (if multiple enemies of different speeds)
	float velocity = 1.5;
	float distance = velocity * tick;
	position += direction * distance;
}

void BulletEntity::draw(long tick, glm::mat4 projView)
{
    glm::mat4 mvp = projView * getTransformationMatrix();
    glm::mat4 modelMatrix = getTransformationMatrix();
    
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    
    glUniform3fv(glGetUniformLocation(globals::mainProgram, "color"), 1, glm::value_ptr(color));

    models::activateTexture(texture);

    models::drawModel(models::ModelType::Bullet);

}

void BulletEntity::onCollision(Entity* entity )
{
	isAlive = false;
	isCollidable = false;
	hasCollided = true;
	canBeRemoved = true;
}