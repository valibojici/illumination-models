#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex> &vertices, 
	const std::vector<unsigned int>& indices,
	const std::vector<Texture*>& textures
)
	: m_vertices(vertices), m_indices(indices), m_textures(textures)
{
	// create vao and bind it
	m_vao = new VAO();
	m_vao->create();
	m_vao->bind();

	// set the layout of the VBO
	m_vao->addLayout(VAO::DataType::FLOAT, 3); // position
	m_vao->addLayout(VAO::DataType::FLOAT, 2); // texCoord
	m_vao->addLayout(VAO::DataType::FLOAT, 3); // normal

	// create and link VBO
	m_vbo = new VBO(vertices);
	m_vao->linkVBO(*m_vbo);
	// create EBO
	m_ebo = new EBO(indices);
	m_ebo->bind();
}

Mesh::~Mesh()
{
	// must delete VBO before VAO
	delete m_vbo;
	delete m_ebo;
	delete m_vao;
}

void Mesh::draw(Shader &shader)
{
	for (unsigned int i = 0; i < m_textures.size(); ++i) {
		// bind this texture
		m_textures[i]->bind(i);
		std::string name; // check which type it has
		switch (m_textures[i]->getType())
		{
		case Texture::Type::DIFFUSE:
			name = "u_DiffuseTex";
			break;
		case Texture::Type::SPECULAR:
			name = "u_SpecularTex";
			break;
		case Texture::Type::NORMAL:
			name = "u_NormalTex";
			break;
		}
		// set the sampler in shader
		shader.setInt(name, i);
	}
	m_vao->bind();
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}

Mesh *Mesh::getCube(float width, float height, float depth)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// 1 half < 0 and 1 half > 0 so when added => the initial value
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

	// iterate through each face
	for (int i = 0; i < 6; ++i)
	{
		// iterate through this face's indices
		for (int j = 0; j < 4; ++j)
		{
			// get the index of this vertex which is part of the current face
			int vertexIndex = faces[i][j];
			vertices.push_back(Vertex(positions[vertexIndex], texCoords[j], normalsPerFace[i]));
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
	const std::vector<glm::vec3> normals = {
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
	};

	// create vertices using the data above
	std::vector<Vertex> vertices;
	for (int i = 0; i < 4; ++i)
	{
		vertices.push_back(Vertex(positions[i], texCoords[i], normals[i]));
	}

	std::vector<unsigned int> indices = {
		0, 1, 2,
		2, 3, 0};

	return new Mesh(vertices, indices);
}

Mesh *Mesh::getSphere(float radius, int smoothness)
{
	// create a sphere using parametric equation
	const float PI = 3.14159f;
	const float UMIN = 0;
	const float UMAX = 2 * PI;
	const float VMIN = -PI / 2;
	const float VMAX = PI / 2;
	const int MERID = smoothness;
	const int PARR = smoothness;
	const float uStep = (UMAX - UMIN) / MERID;
	const float vStep = (VMAX - VMIN) / PARR;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// for a given langitude and latitude get the sphere coords
	auto getCoords = [radius](float u, float v)
	{
		return glm::vec3{
			radius * glm::cos(v) * glm::cos(u),
			radius * glm::sin(v),
			-radius * glm::cos(v) * glm::sin(u)};
	};

	int currentIndex = 0; // index of the current point on the sphere

	// for every longitude, go from "south pole" to "north pole"
	for (int j = 0; j < MERID; ++j)
	{
		for (int i = 0; i < PARR + 1; ++i)
		{
			float u = UMIN + uStep * j;
			float v = VMIN + vStep * i;
			glm::vec3 coords = getCoords(u, v);

			vertices.push_back(Vertex(coords, {0.0f, 0.0f}, coords));

			if (i == PARR)
			{
				// special case "north pole", ignore and continue
				currentIndex++;
				continue;
			}

			// special case "last meridian", need to wrap around to the first
			if (j == MERID - 1)
			{
				indices.push_back(currentIndex);
				indices.push_back(i);
				indices.push_back(i + 1);

				indices.push_back(i + 1);
				indices.push_back(currentIndex + 1);
				indices.push_back(currentIndex);
			}
			else
			{
				/*
					index + 1 ---- index + MERID + 2
						|				 |
						|				 |
					  index   ---- index + MERID + 1
				*/

				// first triangle
				indices.push_back(currentIndex);
				indices.push_back(currentIndex + MERID + 1);
				indices.push_back(currentIndex + MERID + 2);
				// second triangle
				indices.push_back(currentIndex + MERID + 2);
				indices.push_back(currentIndex + 1);
				indices.push_back(currentIndex);
			}
			currentIndex++;
		}
	}
	return new Mesh(vertices, indices);
}
