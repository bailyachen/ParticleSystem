#include "core.h"
#include "Particle.h"
#include "SpringDampener.h"
#include "Triangle.h"
#include "Cube.h"
#ifndef _CLOTH_H_
#define _CLOTH_H_
class Cloth {
private:
	GLuint VAO;
	GLuint VBO_positions, VBO_normals;
	glm::vec3 diffuseColor;
	glm::vec3 ambientColor;
	glm::mat4 Model;
public:
	float Density;
	float DragForce;
	int NumParticles;
	std::vector<Particle*> particles;
	std::vector<SpringDampener*> springDampeners;
	std::vector<Triangle*> triangles;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;


	Cloth(int numParticles, float sizeX, float sizeY, glm::mat4 model);

	~Cloth() {
		glDeleteBuffers(1, &VBO_positions);
		glDeleteBuffers(1, &VBO_normals);
		glDeleteVertexArrays(1, &VAO);
	};

	void moveFixed(float x, float y, float z, float deltaTime) {
		for (Particle* p : particles) {
			if (p->Fixed) p->Position += glm::vec3(x, y, z) * deltaTime;
		}
	}

	void draw(const glm::mat4& viewProjMtx, GLuint shader, bool ambient, bool wireFrame);
	
	void update(float deltaTime, float windX, float windY, float windZ, Cube* ground);

	std::vector<glm::vec3> GetNormals() {
		std::vector<glm::vec3> normals;
		for (Triangle* t : triangles) {
			normals.push_back(t->Normal);
		}
		return normals;
	}

	std::vector<glm::vec3> GetPositions() {
		std::vector<glm::vec3> positions;
		for (Particle* p : particles) {
			positions.push_back(p->Position);
		}
		return positions;
	}

	void SetMass(float mass) {
		float massPerParticle = mass / NumParticles;
		for (Particle* p : particles) {
			p->Mass = massPerParticle;
		}
	}

	/// <summary>
	/// Get total Mass of cloth.
	/// </summary>
	/// <returns>Total Mass</returns>
	float GetMass() {
		return particles.front()->Mass* NumParticles;
	}

	/// <summary>
	/// Get total force of cloth.
	/// </summary>
	/// <returns>Total Force</returns>
	glm::vec3 GetForce() {
		glm::vec3 fTotal = glm::vec3(0);
		for (Particle* p : particles) {
			fTotal += p->Force;
		}
		return fTotal;
	}

	/// <summary>
	/// Get total acceleration of cloth.
	/// </summary>
	/// <returns>Total Acceleration</returns>
	glm::vec3 GetAcceleration() {
		return GetForce() / GetMass();
	}


};
#endif
