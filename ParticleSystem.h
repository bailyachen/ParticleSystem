#include "core.h"
#include "Cube.h"
#include "Particle.h"
#include <cstdlib>
#include <math.h>

#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_
class ParticleSystem {
private:
	GLuint VAO;
	//GLuint VBO_positions, VBO_normals;
	glm::vec3 diffuseColor;
	glm::vec3 ambientColor;
	glm::mat4 Model;
	Particle* particles[10000];
	int lastUsedParticle = 0;
public:
	glm::vec3 sourcePos = glm::vec3(0);
	int maxParticles = 10000;
	int particlesPerSecond = 1;
	float gravity = -10.0f;
	int seed = 0;
	float mass = 1;
	int minLifeSpan = 1, maxLifeSpan = 10;
	float RoundOffError = 0;
	float airDensity = 0;
	float dragCoefficient = 0;
	float particleRadius = 5;
	float elasticity = 0.99f;
	float friction = 0;
	int maxSpeed = 10;

	ParticleSystem(glm::mat4 model) {
		Model = model;
		diffuseColor = glm::vec3(1.0f, 0.25f, 0.25f);
		ambientColor = glm::vec3(0.5f, 1.0f, 1.0f);
		for (int i = 0; i < maxParticles; i++) {
			particles[i] = nullptr;
		}

		// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
		glGenVertexArrays(1, &VAO);
		//glGenBuffers(1, &VBO_positions);
		//glGenBuffers(1, &VBO_normals);

		// Bind to the VAO.
		glBindVertexArray(VAO);
		// Bind to the first VBO - We will use it to store the vertices
		//glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

		// Bind to the second VBO - We will use it to store the normals
		//glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_DYNAMIC_DRAW);
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

		// Unbind the VBOs.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	};

	int FindUnusedParticle() {

		for (int i = lastUsedParticle; i < maxParticles; i++) {
			if (particles[i] == nullptr || particles[i]->Life < 0) {
				lastUsedParticle = i;
				return i;
			}
		}

		for (int i = 0; i < lastUsedParticle; i++) {
			if (particles[i] == nullptr || particles[i]->Life < 0) {
				lastUsedParticle = i;
				return i;
			}
		}

		return 0;
	}

	void clear() {
		for (Particle* p : particles) {
			p->Life = 0;
		}
	}

	// Called Every Frame
	void CreateParticles(float deltaTime) {
		// Creates a number of particles per update loop depending on time since last update
		float num = particlesPerSecond * deltaTime + RoundOffError;
		int numParticles = (int)num;
		RoundOffError = num - (float)numParticles;
		//std::cout << "NumParticles: " + numParticles << std::endl;
		for (int i = 0; i < numParticles; i++) {
			// Random Position [-1, 1] on each axis
			glm::vec3 position = sourcePos; // glm::vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5);
			float s = (float)(rand() % 1000) / 1000;
			float t = (float)(rand() % 1000) / 1000;
			float u = 2 * 2 * acos(0.0f) * s;
			float v = sqrt(t * (1 - t));
			glm::vec3 direction = glm::vec3(2 * v * cos(u), 1 - 2 * t, 2 * v * sin(u));
			float speed = rand() % maxSpeed + 1;
			// Random lifespan based on min and max
			float life = (float)(rand() % (maxLifeSpan - minLifeSpan) + minLifeSpan);
			int index = FindUnusedParticle();
			if (particles[index] != nullptr) delete particles[index];
			particles[index] = new Particle(position, mass, particleRadius, life, false);
			particles[index]->Velocity = direction * 5.0f;
			//particles[index]->ApplyForce(direction);
		}
	}

	int Simulate(float deltaTime, Cube* ground) {
		// Apply Gravity
		
		int numParticles = 0;
		for (Particle* p : particles) {
			if (p == nullptr) continue;
			p->Life -= deltaTime;
			if (p->Life > 0.0f) {
				// Simulate Particle
				p->ApplyForce(glm::vec3(0, gravity, 0));
				// Apply Drag
				float speed = glm::length(p->Velocity);
				glm::vec3 dragForce = 0.5f * airDensity * speed * speed * dragCoefficient * 2 * acos(0.0f) * p->Radius * p->Radius * -glm::normalize(p->Velocity);
				p->ApplyForce(dragForce);
				p->Integrate(deltaTime, elasticity, ground);
				/*if (p->Velocity != glm::vec3(0))
					printf("Velocity: %f, %f, %f\n", p->Velocity.x, p->Velocity.y, p->Velocity.z);*/
				numParticles++;
			}
		}
		return numParticles;
	}

	void draw(const glm::mat4& viewProjMtx, GLuint shader, bool ambient) {
		// actiavte the shader program 
		glUseProgram(shader);
		// get the locations and send the uniforms to the shader 
		glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&Model);
		glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &diffuseColor[0]);
		if (ambient) glUniform3fv(glGetUniformLocation(shader, "AmbientColor"), 1, &ambientColor[0]);
		else glUniform3fv(glGetUniformLocation(shader, "AmbientColor"), 1, &glm::vec3(0.2f)[0]);
		// Bind the VAO
		glBindVertexArray(VAO);
		for (Particle* p : particles) {
			if (p == nullptr) continue;
			glPointSize(p->Radius);
			p->draw();
			glPointSize(1);
		}
		
		// Unbind the VAO and shader program
		glBindVertexArray(0);
		glUseProgram(0);
	}

};
#endif

