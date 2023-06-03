#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex> &vertices, 
	const std::vector<unsigned int>& indices,
	const std::vector<std::shared_ptr<Texture> >& textures
)
	: m_textures(textures)
{
	// create vao and bind it
	m_vao = new VAO();
	m_vao->create();
	m_vao->bind();

	// set the layout of the VBO
	m_vao->addLayout(VAO::DataType::FLOAT, 3); // position
	m_vao->addLayout(VAO::DataType::FLOAT, 2); // texCoord
	m_vao->addLayout(VAO::DataType::FLOAT, 3); // normal
	m_vao->addLayout(VAO::DataType::FLOAT, 3); // tangent

	// create and link VBO
	m_vbo = new VBO(vertices);
	m_vao->linkVBO(*m_vbo);
	// create EBO
	m_ebo = new EBO(indices);
	m_ebo->bind();

	m_indicesCount = indices.size();
}

Mesh::~Mesh()
{
	// delete VBO before VAO
	delete m_vbo;
	delete m_ebo;
	delete m_vao;
}

void Mesh::draw(Shader &shader)
{
	// set all texture uniforms to false
	shader.setBool("u_hasDiffTexture", false);
	shader.setBool("u_hasSpecTexture", false);
	shader.setBool("u_hasNormTexture", false);
	shader.setBool("u_hasRoughTexture", false);
	shader.setBool("u_hasMetallicTexture", false);
	shader.setBool("u_hasEmissiveTexture", false);
	shader.setBool("u_hasOpacityTexture", false);

	// set uniforms for used textures
	for (unsigned int i = 0; i < m_textures.size(); ++i) {
		// bind this texture
		m_textures[i]->bind(i);

		switch (m_textures[i]->getType())
		{
		case Texture::Type::DIFFUSE:
			shader.setInt("u_DiffuseTex", i);
			shader.setBool("u_hasDiffTexture", true);
			break;
		case Texture::Type::SPECULAR:
			shader.setInt("u_SpecularTex", i);
			shader.setBool("u_hasSpecTexture", true);
			break;
		case Texture::Type::NORMAL:
			shader.setInt("u_NormalTex", i);
			shader.setBool("u_hasNormTexture", true);
			break;
		case Texture::Type::ROUGHNESS:
			shader.setInt("u_RoughTex", i);
			shader.setBool("u_hasRoughTexture", true);
			break;
		case Texture::Type::METALLIC:
			shader.setInt("u_MetallicTex", i);
			shader.setBool("u_hasMetallicTexture", true);
			break;
		case Texture::Type::EMISSIVE:
			shader.setInt("u_EmissiveTex", i);
			shader.setBool("u_hasEmissiveTexture", true);
			break;
		case Texture::Type::OPACITY:
			shader.setInt("u_OpacityTex", i);
			shader.setBool("u_hasOpacityTexture", true);
			break;
		}


	}
	m_vao->bind();
	glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);
	// reset to avoid bugs
	shader.setBool("u_hasDiffTexture", false);
	shader.setBool("u_hasSpecTexture", false);
	shader.setBool("u_hasNormTexture", false);
	shader.setBool("u_hasRoughTexture", false);
	shader.setBool("u_hasMetallicTexture", false);
	shader.setBool("u_hasEmissiveTexture", false);
	shader.setBool("u_hasOpacityTexture", false);
}

Mesh *Mesh::getCube(float width, float height, float depth)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// center of the cube = origin
	height = height / 2;
	depth = depth / 2;
	width = width / 2;

	// 8 positions for the vertices of the cube
	const std::vector<glm::vec3> positions{
		{-width, -height, depth},
		{width, -height, depth},
		{width, height, depth},
		{-width, height, depth},
		{-width, -height, -depth},
		{width, -height, -depth},
		{width, height, -depth},
		{-width, height, -depth},
	};

	// indices to vertices representing a face (4 per face)
	const std::vector<std::vector<int>> faces = {
		{0, 1, 2, 3}, // front
		{5, 4, 7, 6}, // back
		{4, 0, 3, 7}, // left
		{1, 5, 6, 2}, // right
		{3, 2, 6, 7}, // top
		{4, 5, 1, 0}, // bottom
	};

	// normals per face (1 per face)
	const std::vector<glm::vec3> normalsPerFace = {
		{0.0f, 0.0f, 1.0f},	 // front
		{0.0f, 0.0f, -1.0f}, // back
		{-1.0f, 0.0f, 0.0f}, // left
		{1.0f, 0.0f, 0.0f},	 // right
		{0.0f, 1.0f, 0.0f},	 // top
		{0.0f, -1.0f, 0.0f}, // bottom
	};

	// texture coords
	const std::vector<glm::vec2> texCoords = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f}};

	// tangent vectors for a cube
	const std::vector<glm::vec3> tangentsPerFace = {
		{1.0f, 0.0f, 0.0f},	 // front => +Ox
		{-1.0f, 0.0f, 0.0f}, // back  => -Ox
		{0.0f, 0.0f, 1.0f},  // left  => +Oz
		{0.0f, 0.0f, -1.0f}, // right => -Oz
		{1.0f, 0.0f, 0.0f},	 // top => +Ox
		{1.0f, 0.0f, 0.0f}, // bottom => +Ox
	};

	// iterate through each face
	for (int i = 0; i < 6; ++i)
	{
		// iterate through this face's indices
		for (int j = 0; j < 4; ++j)
		{
			// get the index of this vertex which is part of the current face
			int vertexIndex = faces[i][j];
			vertices.push_back(Vertex(
				positions[vertexIndex], 
				texCoords[j], 
				normalsPerFace[i],
				tangentsPerFace[i]
			));
		}

		// with every face, 4 vertices are added
		// for a face i, the vertices making up the face
		// are [i*4, i*4+1, i*4+2, i*4+3]
		indices.push_back(i * 4);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);

		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
		indices.push_back(i * 4);
	}

	return new Mesh(vertices, indices);
}

Mesh *Mesh::getPlane(float width, float height)
{
	// get plane in xOy plane => z = 0
	const std::vector<glm::vec3> positions = {
		{-width / 2, -height / 2, 0.0f},
		{width / 2, -height / 2, 0.0f},
		{width / 2, height / 2, 0.0f},
		{-width / 2, height / 2, 0.0f}};
	const std::vector<glm::vec2> texCoords = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f},
	};
	
	// normal for xOy plane is +Oz
	const glm::vec3 normal = { 0.0f, 0.0f, 1.0f }; // +Oz

	// tangent for xOy plane is +Ox
	const glm::vec3 tangent = { 1.0f, 0.0f, 0.0f }; // +Ox

	// create vertices using the data above
	std::vector<Vertex> vertices;
	for (int i = 0; i < 4; ++i)
	{
		vertices.push_back(Vertex(positions[i], texCoords[i], normal, tangent));
	}

	std::vector<unsigned int> indices = {
		0, 1, 2,
		2, 3, 0};

	return new Mesh(vertices, indices);
}

Mesh *Mesh::getSphere(float radius, int subdivisions)
{
	const float PI = 3.14159265359f;
	const float phi = 1.618033988749895f; // golden ratio
	
	// helper to get a Vertex at a certain radius from normalized coordinates
	auto getVertex = [radius](const glm::vec3& coords) {
		return Vertex(
			coords * radius,
			{ 0.0f, 0.0f },
			coords,
			glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), coords) // tangent
		);
	};

	// create a sphere by subdividing an icosahedron

	// start with a hardcoded icosahedron
	// order of vertices and indices from OpenGL Programming Guide versions 3.0 and 3.1 p. 115
	// replaced X and Z with 1 and golden ratio
	
	std::vector<Vertex> vertices = {
		// coordinates for an icosahedron with edge lenght of 2 is (0, +-1, +-phi) (+-phi, 0, +-1) (+-1, +-phi, 0)
		// normalize to get the vertices on the unit spehre
		getVertex(glm::normalize(glm::vec3(-1.0f, 0.0f, phi))),
		getVertex(glm::normalize(glm::vec3(1.0f, 0.0f, phi))),
		getVertex(glm::normalize(glm::vec3(-1.0f, 0.0f, -phi))),
		getVertex(glm::normalize(glm::vec3(1.0f, 0.0f, -phi))),

		getVertex(glm::normalize(glm::vec3(0.0f, phi, 1.0f))),
		getVertex(glm::normalize(glm::vec3(0.0f, phi, -1.0f))),
		getVertex(glm::normalize(glm::vec3(0.0f, -phi, 1.0f))),
		getVertex(glm::normalize(glm::vec3(0.0f, -phi, -1.0f))),

		getVertex(glm::normalize(glm::vec3(phi, 1.0f, 0.0f))),
		getVertex(glm::normalize(glm::vec3(-phi, 1.0f, 0.0f))),
		getVertex(glm::normalize(glm::vec3(phi, -1.0f, 0.0f))),
		getVertex(glm::normalize(glm::vec3(-phi, -1.0f, 0.0f))),
	};
	// rearrange indices to be in counter-clockwise order
	std::vector<unsigned int> indices{
		0,1,4,   0,4,9,  9,4,5, 4,8,5, 4,1,8,
		8,1,10,  8,10,3, 5,8,3, 5,3,2, 2,3,7,
		7,3,10,  7,10,6, 7,6,11, 11,6,0, 0,6,1,
		6,10,1, 9,11,0, 9,2,11, 9,5,2, 7,11,2
	};


	// used to cache newly created vertices (midpoints)
	std::unordered_map<unsigned long long, unsigned	int> middlePoints;
	// the same new vertex will be created 2 times (because any 2 triangles share an edge) => this cuts the new vertices amount in half
	// key = (index1 | index2 << 32) where index1, index2 are indices of 2 vertices that are the ends of an edge
	// value = the index of the created vertex which is at the middle of vertex[index1], vertex[index2] 

	// helper to get the middle point between 2 vertices
	auto getMiddle = [&middlePoints, &vertices, &getVertex](unsigned int i1, unsigned int i2) -> unsigned int {
		// the edge is not oriented, swap i1 i2 to have i1 <= i2
		if (i1 > i2) {
			std::swap(i1, i2);
		}
		// get the one number representation of this edge
		unsigned long long key = (1ULL * i1) | ((1ULL * i2) << 32);
		
		// if the middle of this edge was not created
		if (middlePoints.count(key) == 0) {
			glm::vec3 v1 = vertices[i1].position;
			glm::vec3 v2 = vertices[i2].position;
			// get the middle point between v1 and v2 which is on the unit sphere
			auto a = glm::normalize(v1 + v2);
			// push vertex scaled with radius
			vertices.push_back(getVertex(a));
			// middlePoints the index of the newly created vertex
			middlePoints[key] = vertices.size() - 1;
		}
		return middlePoints[key];
	};

	// subdivide each triangle into 4 triangles
	for (int k = 0; k < subdivisions; ++k) {
		// vector to hold the newly created vertices
		std::vector<unsigned int> newIndices;
		// iterate through each triangle (3 indices at a time)
		for (unsigned int i = 0; i < indices.size(); i += 3) {
			// get the indices of this triangle
			unsigned int i1 = indices[i];
			unsigned int i2 = indices[i + 1];
			unsigned int i3 = indices[i + 2];
			// get the indices of the (newly) created middle vertices
			unsigned int t1 = getMiddle(i1, i2);
			unsigned int t2 = getMiddle(i2, i3);
			unsigned int t3 = getMiddle(i3, i1);

			// replace triangle i1 i2 i3 with 4 triangles
			/* 
			         i3
				 	 /\
				 t3 /__\t2
				   /_\/_\
				 i1  t1  i2
			*/
			std::vector<unsigned int> newTriangles{
				i1, t1, t3,
				t1, t2, t3,
				t1, i2, t2,
				t3, t2, i3
			};
			// insert the all the indices for these 4 triangles
			newIndices.insert(newIndices.end(), newTriangles.begin(), newTriangles.end());
		}
		// replace the original indices after this subdivision
		indices = newIndices;
	}

	// calculate texture coords
	for (auto& vertex : vertices) {
		glm::vec3 normalizedPosition = glm::normalize(vertex.position);
		// to get u coord calculate the angle using atan2 then map to 0,1
		// there will be problems when the texture wraps, when the angle becomes 2 pi :(
		float u;
		// get angle from Z+ axis (on xOz plane) (atan2(y,x) = atan2(-x, z) in opengl coords)
		u = std::atan2f(-normalizedPosition.x, normalizedPosition.z) + PI;

		// map [0,2pi] to [0,1]
		u /= (2 * PI);

		// v is y coordinate, map from -1,1 to 0,1
		float v = normalizedPosition.y * 0.5f + 0.5f;
		vertex.texCoords = { u, v };
	}

	return new Mesh(vertices, indices);
}

Mesh* Mesh::getCone(float radius, float height, unsigned int sectors, unsigned int stacks)
{
	const float PI = 3.14159265359f;
	// make the cone out of vertical parts (sectors) and horizontal parts (stacks)
	const float sectorStep = (2 * PI) / sectors; // divide 2PI in 'sectors' parts
	const float stackStep = height / stacks; // divide height in 'stacks' parts
	
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	/* Formula for cone:
	* x = u cos(v)
	* y = u sin(v)
	* z = u
	*  where v in [0, 2pi]
	*/

	// helper to get a Vertex at a height H and radius R and angle angle
	auto getVertex = [radius, height](float H, float R, float angle) {
		// swap cos and sin, because 'x' coordinate is z and 'y' coordinate is x
		glm::vec3 coords(R * glm::sin(angle), H,  R * glm::cos(angle));
		// y coordinate of the normal is based on the ratio: cone radius / cone height
		glm::vec3 normal(glm::sin(angle), radius / height, glm::cos(angle));
		normal = glm::normalize(normal);
		return Vertex(
			coords,
			{ 0.0f, 0.0f },
			normal,
			glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), normal)
		);
	};
	 
	// iterate through every sector and stack (vertical portion) of the cone
	for (unsigned int sector = 0; sector < sectors; ++sector) {
		for (unsigned int stack = 0; stack <= stacks; ++stack) {
			// get coords
			float currentHeight = stack * stackStep;
			float currentAngle = sector * sectorStep;
			// calculate current radius
			// clamp to a small value so the normal will not point straight up (bugs with lighting)
			float currentRadius = std::max(0.0001f, 1.0f * (stacks - stack) / stacks * radius);
			vertices.push_back(getVertex(currentHeight, currentRadius, currentAngle));

			// if we are at the "top" dont add triangles
			if (stack == stacks) {
				continue;
			}

			// get indices of the 2 triangles to be added
			unsigned int bottomLeft = vertices.size() - 1;
			unsigned int topLeft = bottomLeft + 1;
			unsigned int bottomRight = bottomLeft + stacks + 1;
			unsigned int topRight = bottomLeft + stacks + 2;

			// special case for last sector (for wrapping around)
			if (sector == sectors - 1) {
				bottomRight = stack;
				topRight = stack + 1;
			}

			std::vector<unsigned int> newIndices {
				bottomLeft, bottomRight, topLeft,
				bottomRight, topRight, topLeft
			};

			indices.insert(indices.end(), newIndices.begin(), newIndices.end());
		}
	}
	// adding the bottom vertex, in the origin (0,0,0)
	vertices.push_back(Vertex(glm::vec3(0.0f), { 0.0f, 0.0f }, {0.0f, -1.0f, 0.0f}));
	unsigned int bottomIndex = vertices.size() - 1;
	// create triangles for the bottom part
	// iterate through every sector and create a triangle with the neighbour vertex 
	for (unsigned int sector = 0; sector < sectors; ++sector) {
		vertices.push_back(Vertex(
			{ radius * glm::sin(sector * sectorStep), 0.0f, radius * glm::cos(sector * sectorStep) },
			{ 0.0f, 0.0f }, 
			{ 0.0f, -1.0f, 0.0f })
		);

		indices.push_back(bottomIndex);
		// special case for wrapping around
		if (sector != sectors - 1) {
			indices.push_back(bottomIndex + 2 + sector);
		}
		else {
			indices.push_back(bottomIndex + 1);
		}
		// current sector
		indices.push_back(bottomIndex + 1 + sector);
	}

	return new Mesh(vertices, indices);
}
