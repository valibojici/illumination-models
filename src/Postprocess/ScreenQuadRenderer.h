#pragma once
#include <Shader.h>
#include "VAO.h"
#include "Texture.h"

/// <summary>
/// Helper class to render a texture to screen
/// </summary>
class ScreenQuadRenderer
{
private:
	VAO m_vao;
	VBO m_vbo;
public:
	ScreenQuadRenderer() {
		// instead of a quad, draw 1 triangle that covers the screen
		float vertices[] = {
			-1.0f, -1.0f, 0.0f, 0.0f,
			3.0f, -1.0f, 2.0f, 0.0f,
			-1.0f, 3.0f, 0.0f, 2.0f,
		};
		m_vao.create();
		m_vao.bind();
		m_vbo.bufferData(vertices, sizeof(vertices));
		m_vao.addLayout(VAO::DataType::FLOAT, 2);
		m_vao.addLayout(VAO::DataType::FLOAT, 2);
		m_vao.linkVBO(m_vbo);
	}
	/// <summary>
	/// Render a texture to screen
	/// </summary>
	/// <param name="texture">: texture which should be drawn (will be bound to slot 0)</param>
	/// <param name="shader">: shader that reads the texture</param>
	void render(unsigned int texture, Shader& shader) {
		m_vao.bind();
		shader.bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		shader.setInt("u_texture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	/// <summary>
	/// Render cubemap
	/// </summary>
	/// <param name="texture">: cubemap texture</param>
	/// <param name="inverseViewProjMatrix">: (view * projection) ^ -1 == projection^-1 * view^-1</param>
	void renderCubemap(unsigned int texture, Shader& shader, const glm::mat4& inverseViewProjMatrix) {
		m_vao.bind();
		shader.bind();
		shader.setMat4("u_inverseViewProjMatrix", inverseViewProjMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		shader.setInt("u_texture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
};