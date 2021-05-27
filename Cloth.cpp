#include "Cloth.h"

Cloth::Cloth(int numParticles, float sizeX, float sizeY, glm::mat4 model) {
	NumParticles = numParticles;
	Density = 1.225f;
	DragForce = 1.28f;
	Model = model;
	diffuseColor = glm::vec3(1.0f, 0.25f, 0.25f);
	ambientColor = glm::vec3(0.5f, 1.0f, 1.0f);
	float massPerParticle = 0.001f / (float)numParticles;
	for (int x = 0; x < numParticles; x++) {
		for (int y = 0; y < numParticles; y++) {
			glm::vec3 position = glm::vec3(-sizeX / 2.0f + (x * sizeX / numParticles), 0, -sizeY / 2.0f + y * sizeY / numParticles);
			Particle* p = new Particle(position, massPerParticle, false);
			particles.push_back(p);
		}
	}

	for (int x = 0; x < numParticles - 1; x++) {
		for (int y = 0; y < numParticles - 1; y++) {
			SpringDampener* edgeAB = new SpringDampener(10000.0f, 0.9f, particles[x * numParticles + y], particles[(x + 1) * numParticles + y]);
			SpringDampener* edgeAD = new SpringDampener(10000.0f, 0.9f, particles[x * numParticles + y], particles[x * numParticles + (y + 1)]);
			SpringDampener* edgeBD = new SpringDampener(10000.0f, 0.9f, particles[(x + 1) * numParticles + y], particles[x * numParticles + (y + 1)]);
			SpringDampener* edgeCB = new SpringDampener(10000.0f, 0.9f, particles[(x + 1) * numParticles + (y + 1)], particles[(x + 1) * numParticles + y]);
			SpringDampener* edgeCD = new SpringDampener(10000.0f, 0.9f, particles[(x + 1) * numParticles + (y + 1)], particles[x * numParticles + (y + 1)]);
			SpringDampener* edgeAC = new SpringDampener(10000.0f, 0.9f, particles[x * numParticles + y], particles[(x + 1) * numParticles + (y + 1)]);
			Triangle* t1 = new Triangle(particles[x * numParticles + y], particles[(x + 1) * numParticles + y], particles[x * numParticles + (y + 1)], edgeAB, edgeAD, edgeBD);
			Triangle* t2 = new Triangle(particles[(x + 1) * numParticles + (y + 1)], particles[(x + 1) * numParticles + y], particles[x * numParticles + (y + 1)], edgeCB, edgeCD, edgeBD);
			springDampeners.push_back(edgeAB);
			springDampeners.push_back(edgeAD);
			springDampeners.push_back(edgeBD);
			springDampeners.push_back(edgeCB);
			springDampeners.push_back(edgeCD);
			springDampeners.push_back(edgeAC);
			triangles.push_back(t1);
			triangles.push_back(t2);


		}
	}
	positions = GetPositions();
	normals = GetNormals();

	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);

	// Bind to the VAO.
	glBindVertexArray(VAO);
	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Cloth::draw(const glm::mat4& viewProjMtx, GLuint shader, bool ambient, bool wireFrame) {
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
	if (wireFrame) {
		glPointSize(5);
		glDisable(GL_TEXTURE_2D);
		for (Particle* p : particles) {
			p->draw();
		}
		glEnable(GL_TEXTURE_2D);
		glPointSize(1);

		for (SpringDampener* sp : springDampeners) {
			sp->draw();
		}
	}
	else {
		for (Triangle* t : triangles) {
			t->draw(NumParticles);
		}
	}

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

void Cloth::update(float deltaTime, float windX, float windY, float windZ, Cube* ground) {
	// Apply Gravity
	for (Particle* p : particles) {
		p->ApplyForce(glm::vec3(0, -10.0f, 0));
	}

	// Apply Wind
	for (Particle* p : particles) {
		p->ApplyForce(glm::vec3(windX * sin(p->Position.x + deltaTime), windY * sin(p->Position.y + deltaTime), windZ * sin(p->Position.z + deltaTime)));
	}

	for (SpringDampener* sp : springDampeners) {
		sp->ComputeForce();
	}
	// apply drag force
	for (Triangle* t : triangles) {
		t->update(Density, DragForce);
	}


	for (Particle* p : particles) {
		p->Integrate(deltaTime, 0.99f, ground);
	}

	positions = GetPositions();
	normals = GetNormals();
}