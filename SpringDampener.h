#include "core.h"
#include "Particle.h"
#ifndef _SPRINGDAMPENER_H_
#define _SPRINGDAMPENER_H_
class SpringDampener {
private:
public:
	float SpringConstant;
	float DampingConstant;
	float RestLength;
	Particle* P1, * P2;
	void ComputeForce() {
		glm::vec3 ePrime = P2->Position - P1->Position;
		float length = glm::length(ePrime);
		glm::vec3 e = glm::normalize(ePrime);
		float closingVelocity = glm::dot((P1->Velocity - P2->Velocity), e);
		float force = -SpringConstant * (RestLength - length) - (DampingConstant * closingVelocity);
		glm::vec3 f1 = force * e;
		glm::vec3 f2 = -force * e;
		P1->ApplyForce(f1);
		P2->ApplyForce(f2);
	};
	SpringDampener(float springConstant, float dampingConstant, Particle* p1, Particle* p2) {
		SpringConstant = springConstant;
		DampingConstant = dampingConstant;
		RestLength = glm::length(p1->Position - p2->Position);
		P1 = p1;
		P2 = p2;
	}

	void update() {

	}

	void draw() {
		glColor3f(0, 1, 1);
		glBegin(GL_LINES);
		glVertex3f(P1->Position.x, P1->Position.y, P1->Position.z);
		glVertex3f(P2->Position.x, P2->Position.y, P2->Position.z);
		glEnd();
	}
};
#endif
