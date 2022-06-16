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

int Disk::updateDisk(float deltaT, glm::vec2 posPaddle1, glm::vec2 posPaddle2, glm::vec2 speedPaddle1, glm::vec2 speedPaddle2)
{
	checkPaddleCollision(deltaT,posPaddle1, posPaddle2, speedPaddle1, speedPaddle2);
	int status = checkTableCollision(deltaT);


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
	return status;
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
int Disk::checkTableCollision(float deltaT) {

	float cut=0.2;
	float maxdistance=0.165;


	//Edge and door collision control
	
		if (posDisk.y - radiusDisk + speedDisk.y * deltaT <= -widthTable / 2 || posDisk.y + radiusDisk + speedDisk.y * deltaT >= widthTable / 2) {
			speedDisk.y = -speedDisk.y;
		}
		//2ND half of the table case (also including intersection)
		if (posDisk.y - radiusDisk + speedDisk.y * deltaT >= (-widthTable / 2) + cut &&
			posDisk.y + radiusDisk + speedDisk.y * deltaT <= (widthTable / 2) - cut)
		{	//DOOR COLLISION CASE
			if (posDisk.x + speedDisk.x * deltaT <= (- lengthTable / 2)-0.04) //possibile problema
			{
				//WIN BLUE
				posDisk = glm::vec2(-0.3f, 0.0f);
				speedDisk = glm::vec2(0.0f, 0.0f);
				return 1;
			}
			if (posDisk.x + speedDisk.x * deltaT >= (lengthTable / 2)+0.04) 
			{
				//WIN RED
				posDisk = glm::vec2(0.3f, 0.0f);
				speedDisk = glm::vec2(0.0f, 0.0f);
				return 2;
			}
		}
		else {
			//Check for the edges of the table
			glm::vec2 edgeC0 = glm::vec2((-lengthTable / 2) + cut, (-widthTable / 2) + cut);
			glm::vec2 edgeC1 = glm::vec2((-lengthTable / 2) + cut, (widthTable / 2) - cut);
			glm::vec2 edgeC2 = glm::vec2((lengthTable / 2) - cut, (-widthTable / 2) + cut);
			glm::vec2 edgeC3 = glm::vec2((lengthTable / 2) - cut, (widthTable / 2) - cut);

			float distanceDiskC0 = distance(posDisk, edgeC0);
			float distanceDiskC1 = distance(posDisk, edgeC1);
			float distanceDiskC2 = distance(posDisk, edgeC2);
			float distanceDiskC3 = distance(posDisk, edgeC3);
	
			if ((posDisk.x - radiusDisk + speedDisk.x * deltaT <= (-lengthTable / 2) + cut &&
				posDisk.y - radiusDisk + speedDisk.y * deltaT <= (-widthTable / 2) + cut) &&
				distanceDiskC0 > maxdistance) {
				glm::vec2 vCollisionC0 = posDisk - edgeC0;
				glm::vec2 vCollisionC0Norm = glm::normalize(vCollisionC0);
				float speed1 = dot(speedDisk, vCollisionC0Norm);
				if (speed1 < 0) {
					return 0;
				}
				speedDisk = -speed1 * vCollisionC0Norm;
			}
			if ((posDisk.x - radiusDisk + speedDisk.x * deltaT <= (-lengthTable / 2) + cut &&
				posDisk.y + radiusDisk + speedDisk.y * deltaT >= (widthTable / 2) - cut) &&
				distanceDiskC1 > maxdistance) {
				glm::vec2 vCollisionC1 = posDisk - edgeC1;
				glm::vec2 vCollisionC1Norm = glm::normalize(vCollisionC1);
				float speed1 = dot(speedDisk, vCollisionC1Norm);
				if (speed1 < 0) {
					return 0;
				}
				speedDisk = -speed1 * vCollisionC1Norm;
			}
			if ((posDisk.x + radiusDisk + speedDisk.x * deltaT >= (lengthTable / 2) - cut &&
				posDisk.y - radiusDisk + speedDisk.y * deltaT <= (-widthTable / 2) + cut) &&
				distanceDiskC2 > maxdistance) {
				glm::vec2 vCollisionC2 = posDisk - edgeC2;
				glm::vec2 vCollisionC2Norm = glm::normalize(vCollisionC2);
				float speed1 = dot(speedDisk, vCollisionC2Norm);
				if (speed1 < 0) {
					return 0;
				}
				speedDisk = -speed1 * vCollisionC2Norm;
			}
			if ((posDisk.x - radiusDisk + speedDisk.x * deltaT >= (lengthTable / 2) - cut &&
				posDisk.y + radiusDisk + speedDisk.y * deltaT >= (widthTable / 2) - cut) &&
				distanceDiskC3 > maxdistance) {
				glm::vec2 vCollisionC3 = posDisk - edgeC3;
				glm::vec2 vCollisionC3Norm = glm::normalize(vCollisionC3);
				float speed1 = dot(speedDisk, vCollisionC3Norm);
				if (speed1 < 0) {
					return 0;
				}
				speedDisk = -speed1 * vCollisionC3Norm;
			}
		}
		return 0;
};