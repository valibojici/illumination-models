#pragma once
#include "GL/glew.h"
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include "glm/glm.hpp"

/// <summary>
/// Class for loading shaders and setting uniforms
/// </summary>
class Shader
{
private:
	// the id of the currently bound shader
	static unsigned int s_currentBoundShader;
	unsigned int m_id = 0;

	// cache uniforms locations
	std::unordered_map<std::string, int> m_uniformLocations;

	/// <summary>
	/// Load shader from filepath
	/// </summary>
	/// <param name="type">Type of shader, GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER</param>
	/// <returns>id of the shader</returns>
	unsigned int load(const std::string& path, unsigned int type);

	/// <summary>
	/// Get location of an uniform
	/// </summary>
	unsigned int getLocation(const std::string& name);


	struct TemplateInfo {
		size_t start;
		size_t end;
	};
	// read a file content in a string
	std::string readFile(const std::string& path);

	// process the "@extends", load main template and replace sections
	std::string processExtends(std::string shader);

	// replace the "@include" with the mentioned file
	std::string processInclude(std::string shader);

	// get the start / end of the first "name" strings starting from offset
	TemplateInfo extractName(const std::string& str, size_t offset);
public:
	Shader(const std::string& vertexPath,
		const std::string& fragmentPath,
		const std::string& geometryPath = "");
	Shader() = default;
	~Shader();

	/// <summary>
	/// Load shader program from file.
	/// </summary>
	/// <param name="vertexPath">: path to vertex shader</param>
	/// <param name="fragmentPath">: path to fragment shader</param>
	void load(const std::string& vertexPath,
		const std::string& fragmentPath,
		const std::string& geometryPath = "");
	void bind() const;
	void unbind() const;

	void setFloat(const std::string& name, float val);
	void setInt(const std::string& name, int val);
	void setBool(const std::string& name, bool val);
	void setVec4(const std::string& name, const glm::vec4& val);
	void setVec3(const std::string& name, const glm::vec3& val);
	void setMat4(const std::string& name, const glm::mat4& val);
	void setMat3(const std::string& name, const glm::mat3& val);
	void setIntArray(const std::string& name, unsigned int count, int* data);
};