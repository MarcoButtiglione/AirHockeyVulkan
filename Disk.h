#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Disk
{
private:
	glm::vec2 posDisk;
	glm::vec2 speedDisk;
	float radiusDisk;
	float radiusPaddle;
	float widthTable;
	float lengthTable;

public:
	Disk(glm::vec2 pos, glm::vec2 speed, float radDisk,float radPaddle, float widthT, float lengthT);
	glm::vec2 getPos();
	glm::vec2 getSpeed();

	void setPos(glm::vec2 pos);
	void setSpeed(glm::vec2 speed);

	int updateDisk(float deltaT,glm::vec2 posPaddle1, glm::vec2 posPaddle2, glm::vec2 speedPaddle1, glm::vec2 speedPaddle2);
	void checkPaddleCollision(float deltaT, glm::vec2 posPaddle1, glm::vec2 posPaddle2, glm::vec2 speedPaddle1, glm::vec2 speedPaddle2);
	int checkTableCollision(float deltaT);

};


