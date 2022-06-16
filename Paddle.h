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
	
	void updatespeed(int check,float T);

	void updatePaddle1(float deltaT, float mx, float mz);
	void updatePaddle2(float deltaT, float mx, float mz);
	


};

