#pragma once
#include "GL/glew.h"
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include "glm/glm.hpp"

class Shader
{
private:
	static unsigned int s_currentBoundShader;
	unsigned int m_id = 0;
	std::unordered_map<std::string, int> m_uniformLocations;
	unsigned int load(const std::string& path, unsigned int type);
	unsigned int getLocation(const std::string& name);
public:
	Shader(const std::string& vertexPath,
		const std::string& fragmentPath,
		const std::string& geometryPath = "");
	Shader() = default;
	~Shader();

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

struct Position {
	size_t start;
	size_t end;
};
std::string readFile(const std::string& path);
std::string processExtends(std::string shader);
std::string processInclude(std::string shader);
Position extractName(const std::string& str, size_t offset);