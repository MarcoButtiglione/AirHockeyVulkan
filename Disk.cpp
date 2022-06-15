#include "Disk.h"


Disk::Disk(glm::vec2 pos, glm::vec2 speed, float radDisk,float radPaddle, float widthT, float lengthT)
{
	posDisk = pos;
	speedDisk = speed;
	radiusDisk = radDisk;
	radiusPaddle = radPaddle;
	widthTable = widthT;
	lengthTable = lengthT;
}


glm::vec2 Disk::getPos()
{
	return posDisk;
}

glm::vec2 Disk::getSpeed()
{
	return speedDisk;
}

void Disk::setPos(glm::vec2 pos)
{
	posDisk = pos;
}

void Disk::setSpeed(glm::vec2 speed)
{
	speedDisk = speed;
}

void Disk::updateDisk(float deltaT, bool isMovingPad1, bool isMovingPad2,glm::vec2 posPaddle1, glm::vec2 posPaddle2)
{
	checkPaddleCollision(deltaT,isMovingPad1, isMovingPad2, posPaddle1, posPaddle2);
	checkTableCollision(deltaT);

	
	posDisk += speedDisk * deltaT;

	if (speedDisk.x > 0) {
		speedDisk.x-=0.1f * deltaT;
	}
	else {
		speedDisk.x += 0.1f * deltaT;
	}

	if (speedDisk.y > 0) {
		speedDisk.y -= 0.1f * deltaT;
	}
	else
	{
		speedDisk.y += 0.1f * deltaT;
	}
	
};
void Disk::checkPaddleCollision(float deltaT, bool isMovingPad1, bool isMovingPad2,glm::vec2 posPaddle1, glm::vec2 posPaddle2){
	

	float distancePad1 = distance(posPaddle1, posDisk);
	float distancePad2 = distance(posPaddle2, posDisk);

	if (distancePad1 < radiusDisk + radiusPaddle) {
		if (isMovingPad1) {
			if (speedDisk.x > 0) {
				speedDisk.x += 1.5f * deltaT;
			}
			else {
				speedDisk.x -= 1.5f * deltaT;
			}

			if (speedDisk.y > 0) {
				speedDisk.y += 1.5f * deltaT;
			}
			else
			{
				speedDisk.y -= 1.5f * deltaT;
			}
		}
		speedDisk = -speedDisk;

		
	}
	else if (distancePad2 < radiusDisk + radiusPaddle) {
		if (isMovingPad2) {
			if (speedDisk.x > 0) {
				speedDisk.x += 1.5f * deltaT;
			}
			else {
				speedDisk.x -= 1.5f * deltaT;
			}

			if (speedDisk.y > 0) {
				speedDisk.y += 1.5f * deltaT;
			}
			else
			{
				speedDisk.y -= 1.5f * deltaT;
			}
		}
		speedDisk = -speedDisk;
	}
};
void Disk::checkTableCollision(float deltaT){
	if (posDisk[0] - radiusDisk + speedDisk[0]*deltaT < -lengthTable/2 || posDisk[0] + radiusDisk + speedDisk[0] * deltaT> lengthTable / 2) {
		speedDisk[0] = -speedDisk[0];
	}
	if (posDisk[1] - radiusDisk + speedDisk[1] * deltaT< -widthTable / 2 || posDisk[1] + radiusDisk + speedDisk[1] * deltaT> widthTable/2) {
		speedDisk[1] = -speedDisk[1];
	}

};