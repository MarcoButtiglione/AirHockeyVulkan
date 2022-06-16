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

void Paddle::updatePaddle1(float deltaT, float mx, float mz) {
	if (posPaddle.x + radius + mx * deltaT / speed <= 0.0 &&
		posPaddle.x - radius + mx * deltaT / speed >= -lengthT/2)
		posPaddle.x += mx * deltaT / speed;
	if (posPaddle.z + radius - mz * deltaT / speed <= widthT/2 &&
		posPaddle.z - radius - mz * deltaT / speed >= -widthT/2)
		posPaddle.z -= mz * deltaT / speed;
}


void Paddle::updatePaddle2(float deltaT, float mx, float mz) {
	if (posPaddle.x - radius + mx * deltaT / speed >= 0.0 &&
		posPaddle.x + radius + mx * deltaT / speed <= lengthT/2)
		posPaddle.x += mx * deltaT / speed;
	if (posPaddle.z + radius - mz * deltaT / speed <= widthT/2 &&
		posPaddle.z - radius - mz * deltaT / speed >= -widthT/2)
		posPaddle.z -= mz * deltaT / speed;
}



