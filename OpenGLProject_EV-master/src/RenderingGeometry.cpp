#include "RenderingGeometry.h"
#include "GLM_Header.h"
#include "GL_Header.h"

#include "Gizmos.h"

#include "Vertex.h"
#include "Utility.h"

#include "stb_image.h"

bool RenderingGeometry::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	//GUI:
	GUI::createNewBar("Rendering Geo bar");
	TwAddVarRW(GUI::getBar("Rendering Geo bar"), "Wave Speed", TW_TYPE_FLOAT, &m_waveSpeed, "");
	TwAddVarRW(GUI::getBar("Rendering Geo bar"), "Height", TW_TYPE_FLOAT, &m_height, "min=1 max=50");
	TwAddVarRW(GUI::getBar("Rendering Geo bar"), "Wireframe", TW_TYPE_BOOL32, &m_wireframe, "");

	loadTexture("./textures/pirateflag.png");
	loadShaders("./shaders/textured_vertex.glsl", "./shaders/textured_fragment.glsl", &m_programID);
	generateGrid(100, 100);

	Gizmos::create();

	m_flyCamera = FlyCamera(60.0f, m_windowWidth / m_windowHeight, 10.0f);
	m_flyCamera.m_fieldOfView = 60.0f;
	m_flyCamera.m_aspect = m_windowWidth / m_windowHeight;
	m_flyCamera.setMoveSpeed(100.0f);
	m_flyCamera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_flyCamera.setFOVSpeed(100.0f);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	m_time = 0;

	m_waveSpeed = 10.0f;
	m_height = 1;

	m_wireframe = false;

	return true;
}

void RenderingGeometry::shutdown()
{


	Application::shutdown();
}

bool RenderingGeometry::update()
{
	if (Application::update() == false)
	{
		return false;
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
	m_time += m_waveSpeed * m_deltaTime;

	m_flyCamera.m_windowWidth = m_windowWidth;
	m_flyCamera.m_windowHeight = m_windowHeight;
	m_flyCamera.update(m_deltaTime);
	return true;
}

void RenderingGeometry::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	}
	
	glBindVertexArray(m_VAO);

	glUseProgram(m_programID);

	unsigned int projViewHandle = glGetUniformLocation(m_programID, "projView");
	unsigned int timeHandle = glGetUniformLocation(m_programID, "time");
	unsigned int heightHandle = glGetUniformLocation(m_programID, "height");
	if (projViewHandle >= 0)
	{
		glUniformMatrix4fv(projViewHandle, 1, GL_FALSE, (float*)&m_flyCamera.m_projView);
		glUniform1f(timeHandle, m_time);
		glUniform1f(heightHandle, m_height);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	Gizmos::draw(m_flyCamera.m_projView);

	Application::draw();
}

void RenderingGeometry::generateGrid(unsigned int a_rows, unsigned int a_cols)
{
	//Sets up Vertexes
	VertexTexCoord* vertexArray = new VertexTexCoord[(a_rows + 1) * (a_cols + 1)];
	for (unsigned int row = 0; row < a_rows + 1; ++row)
	{
		for (unsigned int col = 0; col < a_cols + 1; ++col)
		{
			vec4 position = vec4((float)col, 0, (float)row, 1);
			vertexArray[col + row * (a_cols + 1)].position = position;

			//vec4 color = vec4(sinf((row / (float)(a_rows + 1))), sinf((col / (float)(a_cols + 1))), 1.0f, 1.0f);
			//vertexArray[col + row * (a_cols + 1)].color = color;

			vertexArray[col + row * (a_cols + 1)].texCoord = vec2((float)col / (float)a_cols
																, (float)row / (float)a_rows);
		}
	}

	m_indexCount = a_rows * a_cols * 6;

	//Sets up Indicies
	unsigned int* indexArray = new unsigned int[m_indexCount];
	int indexLocation = 0;
	for (unsigned int row = 0; row < a_rows; ++row)
	{
		for (unsigned int col = 0; col < a_cols; ++col)
		{
			//Triangle 1
			indexArray[indexLocation + 0] = col + row * (a_cols + 1);
			indexArray[indexLocation + 1] = col + (row + 1) * (a_cols + 1);
			indexArray[indexLocation + 2] = (col + 1) + row * (a_cols + 1);

			//Triangle 2
			indexArray[indexLocation + 3] = (col + 1) + row * (a_cols + 1);
			indexArray[indexLocation + 4] = col + (row + 1) * (a_cols + 1);
			indexArray[indexLocation + 5] = (col + 1) + (row + 1) * (a_cols + 1);

			indexLocation += 6;
		}
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (a_cols + 1)*(a_rows + 1)*sizeof(VertexTexCoord), vertexArray, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), (void*)sizeof(vec4));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vertexArray;
	delete[] indexArray;
}


bool RenderingGeometry::loadTexture(const char* a_filename)
{
	int width;
	int height;
	int channels;

	unsigned char *data = stbi_load(a_filename, &width, &height, &channels, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	GLenum glChannel;
	switch (channels)
	{
	case 1:
		glChannel = GL_RED;
		break;
	case 2:
		glChannel = GL_RG;
		break;
	case 3:
		glChannel = GL_RGB;
		break;
	case 4:
		glChannel = GL_RGBA;
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, glChannel, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return true;
}
