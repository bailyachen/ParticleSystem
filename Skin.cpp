#include "Skin.h"

bool Skin::load(const char* file, Skeleton* skeleton) {
	this->skeleton = skeleton;
	model = glm::mat4(1.0f);
	diffuseColor = glm::vec3(1.0f, 0.5f, 0.5f);
	ambientColor = glm::vec3(0.5f, 1.0f, 1.0f);
	Tokenizer token;
	if (token.Open(file)) {
		while (1) {
			char temp[256];
			token.GetToken(temp);
			if (strcmp(temp, "positions") == 0) {
				int count = token.GetInt();
				token.FindToken("{");
				for (int i = 0; i < count; i++) {
					float x = token.GetFloat();
					float y = token.GetFloat();
					float z = token.GetFloat();
					glm::vec3 position = glm::vec3(x, y, z);
					positions.push_back(position);
				}
				token.FindToken("}");
			}
			//else // Check for other tokens
			else if (strcmp(temp, "normals") == 0) {
				int count = token.GetInt();
				token.FindToken("{");
				for (int i = 0; i < count; i++) {
					float x = token.GetFloat();
					float y = token.GetFloat();
					float z = token.GetFloat();
					glm::vec3 normal = glm::vec3(x, y, z);
					normals.push_back(normal);
				}
				token.FindToken("}");
			}
			else if (strcmp(temp, "skinweights") == 0) {
				int count = token.GetInt();
				token.FindToken("{");
				for (int i = 0; i < count; i++) {
					std::vector<glm::vec2> skinWeight;
					int numBindings = token.GetInt();
					for (int j = 0; j < numBindings; j++) {
						int joint = token.GetInt();
						float weight = token.GetFloat();
						glm::vec2 binding = glm::vec2(joint, weight);
						skinWeight.push_back(binding);
					}
					skinWeights.push_back(skinWeight);
				}
				token.FindToken("}");
			}
			else if (strcmp(temp, "triangles") == 0) {
				int count = token.GetInt();
				token.FindToken("{");
				for (int i = 0; i < count; i++) {
					int triangle1 = token.GetInt();
					int triangle2 = token.GetInt();
					int triangle3 = token.GetInt();
					triangles.push_back(unsigned(triangle1));
					triangles.push_back(unsigned(triangle2));
					triangles.push_back(unsigned(triangle3));
				}
				token.FindToken("}");
			}
			else if (strcmp(temp, "bindings") == 0) {
				int count = token.GetInt();
				token.FindToken("{");
				for (int i = 0; i < count; i++) {
					token.FindToken("matrix");
					token.FindToken("{");
					glm::mat4 matrix = glm::mat4(1.0f);
					for (int y = 0; y < 4; y++) {
						for (int x = 0; x < 3; x++) {
							matrix[y][x] = token.GetFloat();
						}
					}
					bindings.push_back(matrix);
					token.FindToken("}");
				}
			}
			else if (strcmp(temp, "}") == 0) {
				token.Close();


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

				// Generate EBO, bind the EBO to the bound VAO and send the data
				glGenBuffers(1, &EBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangles.size(), triangles.data(), GL_DYNAMIC_DRAW);

				// Unbind the VBOs.
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
				return true;
			}
			else token.SkipLine(); // Unrecognized token
		}
	}
	return false;
}

void Skin::update() {
	if (skeleton == nullptr) return;
	skinningMatrices.clear();
	vertices.clear();
	vertexNormals.clear();
	// Calculate each M
	for (unsigned int i = 0; i < bindings.size(); i++) {
		glm::mat4 skinningMatrix = skeleton->getWorldMatrix(i) * glm::inverse(bindings[i]);
		skinningMatrices.push_back(skinningMatrix);
	}

	// Calculate each v' = w * W * B^-1 * v
	// Calculate each n' = normalize(w * (W * B^-1)^-1T * n)
	// Loop through vertices
	for (unsigned int i = 0; i < skinWeights.size(); i++) {
		glm::vec4 vertex = glm::vec4(0);
		glm::vec4 vertexNormal = glm::vec4(0);
		// Loop through weights
		for (unsigned int j = 0; j < skinWeights[i].size(); j++) {
			// w * M * v
			vertex += skinWeights[i][j][1] * skinningMatrices[skinWeights[i][j][0]] * glm::vec4(positions[i], 1);
			// w * M ^-1T * n
			vertexNormal += skinWeights[i][j][1] * glm::inverse(glm::transpose(skinningMatrices[skinWeights[i][j][0]])) * glm::vec4(normals[i], 0);
		}
		vertexNormal = glm::normalize(vertexNormal);
		vertices.push_back(glm::vec3(vertex));
		vertexNormals.push_back(glm::vec3(vertexNormal));
	}
}

void Skin::draw(const glm::mat4& viewProjMtx, GLuint shader, bool ambient) {
	// actiavte the shader program 
	glUseProgram(shader);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &diffuseColor[0]);
	if (ambient) glUniform3fv(glGetUniformLocation(shader, "AmbientColor"), 1, &ambientColor[0]);
	else glUniform3fv(glGetUniformLocation(shader, "AmbientColor"), 1, &glm::vec3(0.2f)[0]);
	// Bind the VAO
	glBindVertexArray(VAO); 

	if (skeleton != nullptr) {

		// Bind to the first VBO - We will use it to store the vertices
		glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertices.size(), vertices.data());

		// Bind to the second VBO - We will use it to store the normals
		glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertexNormals.size(), vertexNormals.data());
	}
	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

void Skin::print() {
	/*for (glm::vec3 position : positions) {
		//printf("Position: %f, %f, %f\n", position.x, position.y, position.z);
	}
	for (glm::vec3 normal : normals) {
		//printf("Normal: %f, %f, %f\n", normal.x, normal.y, normal.z);
	}
	for (std::vector<glm::vec2> skinWeight : skinWeights) {
		int numBinding = skinWeight.size();
		//printf("NumBindings: %d ", numBinding);
		for (glm::vec2 binding : skinWeight) {
			//printf("Joint: %f Weight: %f\n", binding.x, binding.y);
		}
	}
	for (unsigned int triangle : triangles) {
		//printf("Triangle: %d\n", triangle);
	}*/
	for (int i = 0; i < bindings.size(); i++) {
		skeleton->joints[i]->print();
		printf("Skin Matrix:\n");
		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++) {
				printf("%f ", bindings[i][y][x]);
			}
			printf("\n");
		}
	}
}

Skin::~Skin() {
	// Delete the VBOs and the VAO.
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}