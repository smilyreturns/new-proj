#include "Lighting.h"

#include "GLM_Header.h"
#include "GL_Header.h"

#include "Gizmos.h"

#include "Vertex.h"
#include "Utility.h"

#include "stb_image.h"

bool Lighting::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	//GUI:
	GUI::createNewBar("Lighting");


	Gizmos::create();

	loadShaders("./shaders/lighting_vertex.glsl", "./shaders/lighting_fragment.glsl", &m_program);

	m_flyCamera = FlyCamera(60.0f, m_windowWidth / m_windowHeight, 10.0f);
	m_flyCamera.m_fieldOfView = 60.0f;
	m_flyCamera.m_aspect = m_windowWidth / m_windowHeight;
	m_flyCamera.setMoveSpeed(10.0f);
	m_flyCamera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_flyCamera.setFOVSpeed(100.0f);
	m_flyCamera.m_sensitivity = 1.0f;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err = tinyobj::LoadObj(shapes, materials, "./models/stanford/bunny.obj");

	createOpenGLBuffers(shapes);

	m_ambientLight = vec3(0.1f);
	m_lightDir = vec3(0, -1, 0);
	
	m_lightColor = vec3(0.6f, 0, 0);
	m_materialColor = vec3(1);

	m_specularPower = 15;

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	return true;
}

void Lighting::reloadShader()
{
	glDeleteProgram(m_program);
	loadShaders("./shaders/lighting_vertex.glsl", "./shaders/lighting_fragment.glsl", &m_program);
}

void Lighting::shutdown()
{
	cleanupOpenGLBuffers();

	Application::shutdown();
}

bool Lighting::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float deltaTime = (float)glfwGetTime();
	glfwSetTime(0.0f);

	if ( glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS )
	{
		reloadShader();
	}

	Gizmos::addTransform(mat4(1));

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 red(1, 0, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 blue(0, 0, 1, 1);

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? blue : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? red : black);
	}

	m_flyCamera.m_windowWidth = m_windowWidth;
	m_flyCamera.m_windowHeight = m_windowHeight;
	m_flyCamera.update(deltaTime);

	return true;
}

void Lighting::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program);
	unsigned int projViewHandle = glGetUniformLocation(m_program, "projView");
	if (projViewHandle >= 0)
	{
		glUniformMatrix4fv(projViewHandle, 1, GL_FALSE, (float*)&m_flyCamera.m_projView);
		
		int ambientUniform = 
			glGetUniformLocation(m_program, "ambientLight");
		int lightDirUniform = 
			glGetUniformLocation(m_program, "lightDir");
		int lightColorUniform = 
			glGetUniformLocation(m_program, "lightColor");
		int materialColorUniform = 
			glGetUniformLocation(m_program, "materialColor");
		//Specular variables
		int eyePosUniform =
			glGetUniformLocation(m_program, "eyePos");
		int specularPowerUniform =
			glGetUniformLocation(m_program, "specularPower");

		glUniform3fv(ambientUniform, 1, (float*)&m_ambientLight);
		glUniform3fv(lightDirUniform, 1, (float*)&m_lightDir);
		glUniform3fv(lightColorUniform, 1, (float*)&m_lightColor);
		glUniform3fv(materialColorUniform, 1, (float*)&m_materialColor);

		vec3 eyePos = m_flyCamera.m_world[3].xyz;
		glUniform3fv(eyePosUniform, 1, (float*)&eyePos);
		glUniform1f(specularPowerUniform, m_specularPower);
	}

	for (unsigned int shapeIndex = 0; shapeIndex < m_glData.size(); ++shapeIndex)
	{

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBindVertexArray(m_glData[shapeIndex].m_VAO);
		glDrawElements(GL_TRIANGLES, m_glData[shapeIndex].m_indexCount, GL_UNSIGNED_INT, 0);
	}

	Gizmos::draw(m_flyCamera.m_projView);

	Application::draw();
}

void Lighting::createOpenGLBuffers(std::vector<tinyobj::shape_t>& a_shapes)
{
	m_glData.resize(a_shapes.size());

	for (unsigned int shapeIndex = 0; shapeIndex < a_shapes.size(); ++shapeIndex)
	{
		std::vector<float> vertexData;

		unsigned int floatCount = a_shapes[shapeIndex].mesh.positions.size();
		floatCount += a_shapes[shapeIndex].mesh.normals.size();

		vertexData.reserve(floatCount);

		vertexData.insert(vertexData.end(),
			a_shapes[shapeIndex].mesh.positions.begin(),
			a_shapes[shapeIndex].mesh.positions.end());

		vertexData.insert(vertexData.end(),
			a_shapes[shapeIndex].mesh.normals.begin(),
			a_shapes[shapeIndex].mesh.normals.end());

		m_glData[shapeIndex].m_indexCount = a_shapes[shapeIndex].mesh.indices.size();

		glGenVertexArrays(1, &m_glData[shapeIndex].m_VAO);
		glGenBuffers(1, &m_glData[shapeIndex].m_VBO);
		glGenBuffers(1, &m_glData[shapeIndex].m_IBO);

		glBindVertexArray(m_glData[shapeIndex].m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_glData[shapeIndex].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* floatCount, 
									  vertexData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glData[shapeIndex].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_shapes[shapeIndex].mesh.indices.size() * sizeof(unsigned int),
											  a_shapes[shapeIndex].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
							  (void*)(sizeof(float)*a_shapes[shapeIndex].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Lighting::cleanupOpenGLBuffers()
{
	for (unsigned int shapeIndex = 0; shapeIndex < m_glData.size(); ++shapeIndex)
	{
		glDeleteVertexArrays(1, &m_glData[shapeIndex].m_VAO);
		glDeleteBuffers(1, &m_glData[shapeIndex].m_VBO);
		glDeleteBuffers(1, &m_glData[shapeIndex].m_IBO);
	}
}