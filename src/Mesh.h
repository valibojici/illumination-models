#pragma once
#include "VAO.h"
#include "Shader.h"
#include "EBO.h"

class Mesh
{
private:
	VAO *m_vao = nullptr;
	VBO *m_vbo = nullptr;
	EBO *m_ebo = nullptr;
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	// TODO: texturi
public:
	Mesh(
		const std::vector<Vertex> &vertices,
		const std::vector<unsigned int> indices);

	~Mesh();

	/// <summary>
	/// Draw this mesh using the shader. Shader is assumed to be bound.
	/// </summary>
	void draw(const Shader &shader);

	/// <summary>
	/// Factory method to get a mesh representing a plane in the xOy plane (centered at origin).
	/// </summary>
	static Mesh *getPlane(float width, float height);

	/// <summary>
	/// Factory method to get a mesh representing a cube centered at the origin.
	/// </summary>
	static Mesh *getCube(float width, float height, float depth);

	/// <summary>
	/// Factory method to get a mesh representing a sphere centered at the origin.
	/// </summary>
	/// <param name="smoothness">How many meridians and parralels</param>
	/// <returns></returns>
	static Mesh *getSphere(float radius, float smoothness = 10);
};
