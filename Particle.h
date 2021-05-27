#include "core.h"
#include "Cube.h"

#ifndef _PARTICLE_H_
#define _PARTICLE_H_
class Particle {
private:

public:
	glm::vec3 Position;
	glm::vec3 Velocity;
	glm::vec3 Force;
	float Mass;
	float Radius;
	float Life;
	bool Fixed;
	void ApplyForce(glm::vec3 f);
	void Integrate(float deltaTime, float elasticity, Cube* ground);
	Particle() {
		Position = glm::vec3(0.0f);
		Velocity = glm::vec3(0.0f);
		Force = glm::vec3(0.0f);
		Mass = 1;
		Radius = 5;
		Fixed = true;
		Life = 0; 
	};
	Particle(glm::vec3 position, float mass, bool fixed) {
		Position = position;
		Velocity = glm::vec3(0.0f);
		Force = glm::vec3(0.0f);
		Mass = mass;
		Radius = 5;
		Fixed = fixed;
		Life = -1;
	};
	Particle(glm::vec3 position, float mass, float life, float radius, bool fixed) {
		Position = position;
		Velocity = glm::vec3(0.0f);
		Force = glm::vec3(0.0f);
		Mass = mass;
		Radius = radius;
		Life = life;
		Fixed = fixed;
	};

	void draw() {
		if (Life <= 0) return;
		glBegin(GL_POINTS);
		glVertex3f(Position.x, Position.y, Position.z);
		glEnd();
	}
};
#endif
