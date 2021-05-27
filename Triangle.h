#include "core.h"

#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_
class Triangle {
private:
public:
	Particle* A;
	Particle* B;
	Particle* C;
	SpringDampener* AB;
	SpringDampener* AC;
	SpringDampener* BC;
	glm::vec3 Normal;
	glm::vec3 Velocity;
	float CrossArea;

	void Update() {

	};

	Triangle(Particle* a, Particle* b, Particle* c, SpringDampener* ab,	SpringDampener* ac,	SpringDampener* bc) {
		A = a; B = b; C = c;
		AB = ab; AC = ac; BC = bc;
		Normal = glm::vec3(0);
		Velocity = glm::vec3(0);
		CrossArea = 0.0f;
	};

	float GetArea() {
		glm::vec3 v = B->Position - A->Position;
		glm::vec3 w = C->Position - A->Position;
		return glm::length(glm::cross(v, w)) / 2.0f;
	}

	glm::vec3 GetVelocity() {
		//printf("Velocity: %f\n", A->Velocity);
		return (A->Velocity + B->Velocity + C->Velocity) / 3.0f;
	}

	glm::vec3 GetNormal() {
		return glm::normalize(glm::cross(B->Position - A->Position, C->Position - A->Position));
	}

	void update(float density, float dragForce) {
		float area = GetArea();
		//printf("Area: %f\n", area);
		Velocity = GetVelocity();
		//printf("Velocity: %f, %f, %f\n", Velocity.x, Velocity.y, Velocity.z);
		Normal = GetNormal();
		//printf("Normal: %f, %f, %f\n", Normal.x, Normal.y, Normal.z);
		glm::vec3 normVelocity = Velocity == glm::vec3(0) ? Velocity : glm::normalize(Velocity);
		//printf("Normalized Velocity: %f, %f, %f\n", normVelocity.x, normVelocity.y, normVelocity.z);
		float normArea = glm::dot(normVelocity, Normal);
		//printf("Norm Area: %f\n", normArea);
		CrossArea = area * normArea;
		//printf("Cross Area: %f\n", CrossArea);
		float magnitude = glm::length(Velocity);
		glm::vec3 af = -0.5f * density * magnitude * magnitude * dragForce * CrossArea* Normal / 3.0f;
		//printf("Aero: %f, %f, %f\n", af.x, af.y, af.z);
		A->ApplyForce(af);
		B->ApplyForce(af);
		C->ApplyForce(af);
	}

	void draw(int numParticles) {
		glBegin(GL_POLYGON);
		glTexCoord2f(A->Position.x / (numParticles - 1), A->Position.y / (numParticles - 1));
		glNormal3f(Normal.x, Normal.y, Normal.z);
		glVertex3f(A->Position.x, A->Position.y, A->Position.z);
		glTexCoord2f(B->Position.x / (numParticles - 1), B->Position.y / (numParticles - 1));
		glNormal3f(Normal.x, Normal.y, Normal.z);
		glVertex3f(B->Position.x, B->Position.y, B->Position.z);
		glTexCoord2f(C->Position.x / (numParticles - 1), C->Position.y / (numParticles - 1));
		glNormal3f(Normal.x, Normal.y, Normal.z);
		glVertex3f(C->Position.x, C->Position.y, C->Position.z);
		glEnd();
	}
};
#endif
