#pragma once

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


class Paddle
{
private:
	glm::vec3 posPaddle;
	float radiusDisk;
	float radius;
	float widthT;
	float lengthT;
	float speed;

public:
	
	Paddle(glm::vec3 pos, float radDisk, float radPaddle, float widthTab, float lengthTab,float speedP);
	
	glm::vec3 getPos();
	glm::vec3 getNextPos(float deltaT, float mx, float mz);
	
	void setPos(glm::vec3 pos);
	
	void setSpeed(float speedP);
	
	void updatespeed(int check,float T);

	void updatePaddle(float deltaT, float mx, float mz);
	
	
	


};

