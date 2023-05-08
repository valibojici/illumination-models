#pragma once
#include "VAO.h"
#include "Shader.h"
#include "Buffer/EBO.h"
#include "Texture.h"
#include <cmath>

class Mesh
{
private:
	VAO *m_vao = nullptr;
	VBO *m_vbo = nullptr;
	EBO *m_ebo = nullptr;
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	
	std::vector<std::shared_ptr<Texture> > m_textures;
public:
	Mesh(
		const std::vector<Vertex> &vertices,
		const std::vector<unsigned int>& indices,
		const std::vector<std::shared_ptr<Texture> >& textures = {});

	~Mesh();

	/// <summary>
	/// Draw this mesh using the shader. Shader is assumed to be bound.
	/// </summary>
	void draw(Shader &shader);

	/// <summary>
	/// Set the textures of the mesh (not owning)
	/// </summary>
	void setTextures(const std::vector<std::shared_ptr<Texture> > textures) { m_textures = textures; }

	/// <summary>
	/// Factory method to get a mesh representing a plane in the xOy plane (centered at origin).
	/// </summary>
	static Mesh *getPlane(float width, float height);

	/// <summary>
	/// Factory method to get a mesh representing a cube centered at the origin.
	/// </summary>
	static Mesh *getCube(float width, float height, float depth);

	/// <summary>
	/// Factory method to get a mesh representing a sphere centered at the origin from an icosahedron
	/// </summary>
	/// <param name="subdivisions">How many times to subdivide the icosahedron</param>
	/// <returns></returns>
	static Mesh *getSphere(float radius, int subdivisions = 3);

	/// <summary>
	/// Factory method to get a mesh representing a cone with the base in the XZ plane centered at origin
	/// and pointing in positive Y direction
	/// </summary>
	/// <param name="radius">: radius of the base</param>
	/// <param name="height">: height of the cone</param>
	/// <param name="sectors">: how many sections the circle base is divided in</param>
	/// <param name="stacks">: how many vertical "slices" (stacks) does the cone have</param>
	static Mesh* getCone(float radius, float height, unsigned int sectors = 20, unsigned int stacks = 3);
};
