#pragma once
#include "GL/glew.h"
#include "glm/glm.hpp"
#include <vector>

struct Vertex {
	Vertex(
		const glm::vec3& pos = { 0.0f, 0.0f, 0.0f }, 
		const glm::vec2& texCoords = { 0.0f, 0.0f },
		const glm::vec3& normal = {0.0f, 1.0f, 0.0f},
		const glm::vec3& tangent = {1.0f, 0.0f, 0.0f}
	)
		: position(pos), texCoords(texCoords), normal(normal), tangent(tangent) {}
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
	glm::vec3 tangent;
};

class VBO {
private:
	static unsigned int s_currentBoundVBO; // currently bound VBO, 0 if not bound
	unsigned int m_id = 0;
public:

	VBO() = default;
	VBO(void* data, unsigned int size);
	VBO(const std::vector<Vertex> vertices);
	~VBO();
	// create EBO and assign id
	void create();

	// load data
	void bufferData(void *data, unsigned int size);

	// bind this VBO if not bound already
	void bind() const;

	// unbind / bind(0)
	void unbind() const;
};