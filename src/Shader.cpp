#include "Shader.h"

unsigned int Shader::s_currentBoundShader = 0;

Shader::Shader(
	const std::string& vertexPath, 
	const std::string& fragmentPath, 
	const std::string& geometryPath)
{
	load(vertexPath, fragmentPath, geometryPath);
}

Shader::~Shader()
{
	glDeleteProgram(m_id);
	s_currentBoundShader = 0;
}

void Shader::load(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
{
	// create a new program
	m_id = glCreateProgram();

	unsigned int vertexShaderId = load(vertexPath, GL_VERTEX_SHADER);
	unsigned int fragmentShaderId = load(fragmentPath, GL_FRAGMENT_SHADER);
	// TODO: check for geometry shader 
	glAttachShader(m_id, vertexShaderId);
	glAttachShader(m_id, fragmentShaderId);

	// link and check for linking errors
	glLinkProgram(m_id);
	int ok;
	char log[1024];
	glGetProgramiv(m_id, GL_LINK_STATUS, &ok);
	if (!ok)
	{
		glGetProgramInfoLog(m_id, 1024, NULL, log);
		printf("ERROR Shader linking failed\n%s\n", log);
	}

	// delete shaders after linking 
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
}

unsigned int Shader::load(const std::string& path, unsigned int type)
{
	std::ifstream file(path);
	if (!file) {
		printf("Error file '%s' not loaded", path.c_str());
		exit(0);
	}
	std::stringstream buffer;
	buffer << file.rdbuf(); // read whole file in buffer
	std::string shaderCodeString = buffer.str();
	const char* shaderCode = shaderCodeString.c_str(); // get the whole shader as string

	unsigned int id = glCreateShader(type);

	// compile 
	glShaderSource(id, 1, &shaderCode, NULL);
	glCompileShader(id);
	
	// check errors
	int ok;
	char log[1024];
	glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
	if (!ok) {
		glGetShaderInfoLog(id, 512, NULL, log);
		const char* shaderType;
		// get string of the type of shader used
		switch (type)
		{
		case GL_VERTEX_SHADER:
			shaderType = "VERTEX";
			break;
		case GL_FRAGMENT_SHADER:
			shaderType = "FRAGMENT";
			break;
		case GL_GEOMETRY_SHADER:
			shaderType = "GEOMETRY";
			break;
		}

		printf("ERROR %s shader compilation failed\n%s\n", shaderType, log);
	}
	return id;
}

unsigned int Shader::getLocation(const std::string& name)
{
	if (m_uniformLocations.find(name) == m_uniformLocations.end()) {
		m_uniformLocations[name] = glGetUniformLocation(m_id, name.c_str());
	}
	return m_uniformLocations[name];
}

void Shader::bind() const
{
	if (m_id != s_currentBoundShader) {
		glUseProgram(m_id);
		s_currentBoundShader = m_id;
	}
}

void Shader::unbind() const
{
	if (s_currentBoundShader != 0) {
		glUseProgram(0);
		s_currentBoundShader = 0;
	}
}

void Shader::setFloat(const std::string& name, float val)
{
	glUniform1f(getLocation(name), val);
}

void Shader::setInt(const std::string& name, int val)
{
	glUniform1i(getLocation(name), val);
}

void Shader::setBool(const std::string& name, bool val)
{
	setInt(name, val ? 1 : 0);
}

void Shader::setVec4(const std::string& name, const glm::vec4& val)
{
	glUniform4fv(getLocation(name), 1, &val[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& val)
{
	glUniform3fv(getLocation(name), 1, &val[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& val)
{
	glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, &val[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& val)
{
	glUniformMatrix3fv(getLocation(name), 1, GL_FALSE, &val[0][0]);
}
