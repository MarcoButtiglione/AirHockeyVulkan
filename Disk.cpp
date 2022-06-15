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

void Disk::updateDisk(float deltaT, glm::vec2 posPaddle1, glm::vec2 posPaddle2, glm::vec2 speedPaddle1, glm::vec2 speedPaddle2)
{
	//checkPaddleCollision(deltaT,posPaddle1, posPaddle2, speedPaddle1, speedPaddle2);
	checkTableCollision(deltaT);


	posDisk += speedDisk * deltaT;

	if (speedDisk.x > 0) {
		speedDisk.x -= 0.1f * deltaT;
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
void Disk::checkPaddleCollision(float deltaT, glm::vec2 posPaddle1, glm::vec2 posPaddle2, glm::vec2 speedPaddle1, glm::vec2 speedPaddle2) {

	float distancePad1 =  distance(posPaddle1, posDisk);
	float distancePad2 = distance(posPaddle2, posDisk);
	if (distancePad1 < radiusDisk + radiusPaddle) {
		glm::vec2 vCollision1 = posDisk - posPaddle1;
		glm::vec2 vCollision1Norm = glm::normalize(vCollision1);
		glm::vec2 vRelativeVelocity1 = speedPaddle1 - speedDisk;
		float speed1 = dot(vRelativeVelocity1, vCollision1Norm);
		if (speed1 < 0) {
			return;
		}

		speedDisk += speed1 * vCollision1Norm * deltaT;
		if (abs(speedPaddle1.x) == 0.0f && abs(speedPaddle1.y) == 0.0f) {
			speedDisk = speed1 * vCollision1Norm;
		}
		else {
			speedDisk = glm::vec2(1.7, 1.7) * vCollision1Norm;
		}

	}
	if (distancePad2 < radiusDisk + radiusPaddle) {
		glm::vec2 vCollision2 = posDisk - posPaddle2;
		glm::vec2 vCollision2Norm = glm::normalize(vCollision2);
		glm::vec2 vRelativeVelocity2 = speedPaddle2 - speedDisk;
		float speed2 = dot(vRelativeVelocity2, vCollision2Norm);
		if (speed2 < 0 ) {
			return;
		}
		if (abs(speedPaddle2.x) == 0.0f && abs(speedPaddle2.y) == 0.0f) {
			speedDisk = speed2 * vCollision2Norm;
		}
		else {
			speedDisk = glm::vec2(1.7, 1.7) * vCollision2Norm;
		}
	}




};
void Disk::checkTableCollision(float deltaT) {
	//Collision check on the long side of the table
	if (posDisk[0] - radiusDisk + speedDisk[0] * deltaT > (-lengthTable / 2) + 0.2 &&
		posDisk[0] + radiusDisk + speedDisk[0] * deltaT < (lengthTable / 2) - 0.2)
	{
		if (posDisk[1] - radiusDisk + speedDisk[1] * deltaT< -widthTable / 2 || posDisk[1] + radiusDisk + speedDisk[1] * deltaT> widthTable / 2) {
			speedDisk[1] = -speedDisk[1];
		}
	}
	//Edge and door collision control
	else {
		if (posDisk[1] - radiusDisk + speedDisk[1] * deltaT > (-widthTable / 2) + 0.2 &&
			posDisk[1] + radiusDisk + speedDisk[1] * deltaT < (widthTable / 2) - 0.2)
		{
			if (posDisk[0] - radiusDisk + speedDisk[0] * deltaT < (- lengthTable / 2)-0.1)
			{
				//WIN BLUE
				posDisk = glm::vec2(0.0f, 0.0f);
				speedDisk = glm::vec2(0.0f, 0.0f);
				std::cout << "WIN BLUE";
			}
			if (posDisk[0] + radiusDisk + speedDisk[0] * deltaT > (lengthTable / 2) + 0.1) 
			{
				//WIN RED
				posDisk = glm::vec2(0.0f, 0.0f);
				speedDisk = glm::vec2(0.0f, 0.0f);
				std::cout << "WIN RED";
			}
			
		}
	}


	/*
	if (posDisk[0] - radiusDisk + speedDisk[0] * deltaT < -lengthTable / 2 || posDisk[0] + radiusDisk + speedDisk[0] * deltaT> lengthTable / 2) {
		speedDisk[0] = -speedDisk[0];
	}
	*/
	
};