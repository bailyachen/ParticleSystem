#include "Particle.h"

void Particle::ApplyForce(glm::vec3 f) {
	if (Life <= 0.0f) return;
	Force += f; 
}

void Particle::Integrate(float deltaTime, float elasticity, Cube* ground) {
	if (!Fixed || Life <= 0.0f) {
		//printf("Force: %f, %f, %f\n", Force.x, Force.y, Force.z);
		glm::vec3 accel = Force / Mass;
		//printf("Accel: %f, %f, %f\n", accel.x, accel.y, accel.z);
		Velocity += accel * deltaTime;
		Position += Velocity * deltaTime;
	}

	for (glm::vec3 position : ground->positions) {
		if (Position.y <= position.y + 0.0001f) {
			float vClosed = glm::dot(Velocity, glm::vec3(0, 1, 0));
			glm::vec3 u = vClosed * glm::vec3(0, 1, 0);
			glm::vec3 w = Velocity - u;
			glm::vec3 momentum = -1.05f * Mass * u;
			Velocity = momentum / Mass;
			Position += Velocity * deltaTime;
			if (Position.y > position.y) {
				Velocity -= elasticity * momentum / Mass;
				break;
			}
		}
	}
	Force = glm::vec3(0);
}
