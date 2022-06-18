#include "Paddle.h"

Paddle::Paddle(glm::vec3 pos, float radDisk, float radPaddle, float widthTab, float lengthTab,float speedP)
{
	posPaddle = pos;
	radiusDisk = radDisk;
	radius = radPaddle;
	widthT = widthTab;
	lengthT = lengthTab;
	speed = speedP;
}

glm::vec3 Paddle::getPos() {
	return posPaddle;
}
void Paddle::setPos(glm::vec3 pos) {
	posPaddle = pos;
}

void Paddle::updatespeed(int check,float T) {
	if (check == 1) {
		if (speed > 0.6)
			speed -= 0.5 * T;
		return;
	}
	if (speed < 1.75)
		speed += 0.5 * T;
	return;
}

void Paddle::updatePaddle(float deltaT, float mx, float mz) {
	/*
	if (posPaddle.x + radius + mx * deltaT / speed <= 0.0 &&
		posPaddle.x - radius + mx * deltaT / speed >= -lengthT/2)
		*/
	if (posPaddle.x < 0) {
		if (posPaddle.x + radius + mx * deltaT / speed <= 0.0){
			posPaddle.x += mx * deltaT / speed;
		    
			}
		posPaddle.z -= mz * deltaT / speed;
	}
	else {
		if (posPaddle.x - radius + mx * deltaT / speed >= 0.0) {
			
			posPaddle.x += mx * deltaT / speed;
		}
		posPaddle.z -= mz * deltaT / speed;
	}
	/*
	if (posPaddle.z + radius - mz * deltaT / speed <= widthT/2 &&
		posPaddle.z - radius - mz * deltaT / speed >= -widthT/2)
		*/
}



glm::vec3 Paddle::getNextPos(float deltaT, float mx, float mz) {
	glm::vec3 nextPosPaddle = glm::vec3(posPaddle.x + mx * deltaT / speed,0.0f, posPaddle.z - mz * deltaT / speed);
	return nextPosPaddle;
}

void Paddle::setSpeed(float speedP) {
	speed = speedP;
}



