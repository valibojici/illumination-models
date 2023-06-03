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
	std::string contents = readFile(path);
	contents = processInclude(processExtends(contents));
	const char* shaderCode = contents.c_str(); // get the whole shader as string
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
	bind();
	glUniform1f(getLocation(name), val);
}

void Shader::setInt(const std::string& name, int val)
{
	bind();
	glUniform1i(getLocation(name), val);
}

void Shader::setBool(const std::string& name, bool val)
{
	bind();
	setInt(name, val ? 1 : 0);
}

void Shader::setVec4(const std::string& name, const glm::vec4& val)
{
	bind();
	glUniform4fv(getLocation(name), 1, &val[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& val)
{
	bind();
	glUniform3fv(getLocation(name), 1, &val[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& val)
{
	bind();
	glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, &val[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& val)
{
	bind();
	glUniformMatrix3fv(getLocation(name), 1, GL_FALSE, &val[0][0]);
}

void Shader::setIntArray(const std::string& name, unsigned int count, int* data) {
	bind();
	glUniform1iv(getLocation(name), count, data);
}

Shader::TemplateInfo Shader::extractName(const std::string& str, size_t offset) {
	size_t begin = offset;
	while (str[begin] != '"') begin++; // find "
	size_t end = begin + 1; // go one past "
	while (str[end] != '"') end++; // find end "
	return { begin + 1, end - 1 }; // go back one
}

std::string Shader::readFile(const std::string& path) {
	std::stringstream p;
	p << "shaders/" << path;
	std::ifstream file(p.str());
	if (!file) {
		printf("Error file '%s' not loaded", p.str().c_str());
		exit(0); 
	}
	std::stringstream buffer;
	buffer << file.rdbuf(); // read whole file in buffer
	return buffer.str();
}

std::string Shader::processExtends(std::string shader) {
	size_t begin = shader.find("@extends");
	if (begin == std::string::npos) return shader;

	// extract the <file> from @extends "<file>"
	TemplateInfo file_pos = extractName(shader, 0);
	std::string file = shader.substr(file_pos.start, file_pos.end - file_pos.start + 1);
	// load base template shader
	std::string base_shader = readFile(file);

	std::unordered_map<std::string, std::string> sections;
	// extract all sections
	size_t section_pos = shader.find("@section");
	while (section_pos != std::string::npos) {
		// extract name of section
		file_pos = extractName(shader, section_pos);
		std::string section_name = shader.substr(file_pos.start, file_pos.end - file_pos.start + 1);
		size_t section_start = file_pos.end + 2; // skip end quote
		size_t section_end = shader.find("@endsection", section_start) - 1;
		// store section content
		sections[section_name] = shader.substr(section_start, section_end - section_start + 1);
		// find next section
		section_pos = shader.find("@section", section_end);
	}

	// replace sections from template
	size_t has_pos = base_shader.find("@has");
	while (has_pos != std::string::npos) {
		// get the section name
		file_pos = extractName(base_shader, has_pos);
		std::string section_name = base_shader.substr(file_pos.start, file_pos.end - file_pos.start + 1);

		// replace this placeholder with the section from the shader
		base_shader.replace(has_pos, file_pos.end - has_pos + 2, sections[section_name]);
		has_pos = base_shader.find("@has");
	}
	return base_shader;
}

std::string Shader::processInclude(std::string shader) {
	size_t pos = shader.find("@include");       // find the index where @include starts
	while (pos != std::string::npos) {            // while there are includes
		TemplateInfo file_pos = extractName(shader, pos);

		// extract the <file> from @include "<file>"
		std::string file = shader.substr(file_pos.start, file_pos.end - file_pos.start + 1);
		// replace <@include "file"> with content
		shader.replace(pos, file_pos.end - pos + 2, readFile(file));

		pos = shader.find("@include"); // search for another @include
	}
	return shader;
}