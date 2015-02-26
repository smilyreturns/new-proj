#include "Animation.h"
#include "GLM_Header.h"
#include "GL_Header.h"

#include "Gizmos.h"

#include "Vertex.h"
#include "Utility.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Animation::Animation(){}

bool Animation::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}

	//GUI:
	GUI::createNewBar("Camera");
	TwAddVarRW(GUI::getBar("Camera"), "Mouse sensitivity", TW_TYPE_FLOAT, &m_flyCamera.m_sensitivity, "min=0.1 max=100 step=0.5");
	TwAddVarRW(GUI::getBar("Camera"), "Fly speed", TW_TYPE_FLOAT, &m_flyCamera.m_moveSpeed, "min=0.1 max=100 step=0.5");

	TwAddVarRW(GUI::getBar("Camera"), "Animation Speed", TW_TYPE_FLOAT, &m_animationSpeed, "min=0.1 max=100 step=0.5");
	//TwAddVarRW(GUI::getBar("Advanced textures"), "Spec Power", TW_TYPE_FLOAT, &m_specularPower, "group=Light min=0.1 max=100 step=0.5");
	
	//GUI:
	GUI::createNewBar("Advanced textures");
	TwAddVarRW(GUI::getBar("Advanced textures"), "Light Direction", TW_TYPE_DIR3F, &m_lightDir, "group=Light");
	TwAddVarRW(GUI::getBar("Advanced textures"), "Light Color", TW_TYPE_COLOR4F, &m_lightColor, "group=Light");
	TwAddVarRW(GUI::getBar("Advanced textures"), "Spec Power", TW_TYPE_FLOAT, &m_specularPower, "group=Light min=0.1 max=100 step=0.5");

	//TwAddVarRW(GUI::getBar("Advanced textures"), "Spec Power", TW_TYPE_FLOAT, &m_specularPower, "group=Light min=0.1 max=100 step=0.5");
	
	Gizmos::create();

	m_flyCamera = FlyCamera(60.0f, m_windowWidth / m_windowHeight, 10.0f);
	m_flyCamera.m_fieldOfView = 60.0f;
	m_flyCamera.m_aspect = m_windowWidth / m_windowHeight;
	m_flyCamera.setMoveSpeed(100.0f);
	m_flyCamera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_flyCamera.setFOVSpeed(100.0f);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	m_fbxFile = new FBXFile();
	m_fbxFile->load("./models/characters/enemyelite/enemyelite.fbx");
	m_fbxFile->initialiseOpenGLTextures();

	loadTextures();
	loadShaders("./shaders/skinned_vertex.glsl", "./shaders/skinned_fragment.glsl", &m_program);

	generateGLMeshes(m_fbxFile);

	m_ambientLight = vec3(0.1f);
	m_lightDir = (vec3(-1, -1, 0));
	m_lightColor = vec3(0.7f);
	m_specularPower = 15;

	m_timer = 0;
	return true;
}

void Animation::shutdown()
{
	Application::shutdown();
}

bool Animation::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	Gizmos::clear();

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

	FBXSkeleton* skele = m_fbxFile->getSkeletonByIndex(0);
	FBXAnimation* anim = m_fbxFile->getAnimationByIndex(0);

	m_timer += m_animationSpeed * m_deltaTime;

	evaluateSkeleton(anim, skele, m_timer);

	for (unsigned int i = 0; i < skele->m_boneCount; ++i)
	{
		skele->m_nodes[i]->updateGlobalTransform();
		
		mat4 nodeGlobal = skele->m_nodes[i]->m_globalTransform;
		vec3 nodePos = nodeGlobal[3].xyz;
		
		Gizmos::addAABBFilled(nodePos, vec3(3.0f), vec4(1, 0, 0, 1), &nodeGlobal);

		if (skele->m_nodes[i]->m_parent != nullptr)
		{
			vec3 parentPos = skele->m_nodes[i]->m_parent->m_globalTransform[3].xyz;
			Gizmos::addLine(nodePos, parentPos, vec4(0, 1, 0, 1));
		}
	}

	m_flyCamera.m_windowWidth = m_windowWidth;
	m_flyCamera.m_windowHeight = m_windowHeight;
	m_flyCamera.update(m_deltaTime);
	return true;
}

void Animation::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program);
	unsigned int projViewHandle = glGetUniformLocation(m_program, "projView");
	if (projViewHandle >= 0)
	{
		glUniformMatrix4fv(projViewHandle, 1, GL_FALSE, (float*)&m_flyCamera.m_projView);
	}

	unsigned int diffuseUniform = glGetUniformLocation(m_program, "diffuseTexture");
	glUniform1i(diffuseUniform, 0);
	
	int ambientUniform =
		glGetUniformLocation(m_program, "ambientLight");
	int lightDirUniform =
		glGetUniformLocation(m_program, "lightDir");
	int lightColorUniform =
		glGetUniformLocation(m_program, "lightColor");
	//Specular variables
	int eyePosUniform =
		glGetUniformLocation(m_program, "eyePos");
	int specularPowerUniform =
		glGetUniformLocation(m_program, "specularPower");

	glUniform3fv(ambientUniform, 1, (float*)&m_ambientLight);
	glUniform3fv(lightDirUniform, 1, (float*)&m_lightDir);
	glUniform3fv(lightColorUniform, 1, (float*)&m_lightColor);

	vec3 eyePos = m_flyCamera.m_world[3].xyz;
	glUniform3fv(eyePosUniform, 1, (float*)&eyePos);
	glUniform1f(specularPowerUniform, m_specularPower);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_specularTexture);

	int diffuseLocation = glGetUniformLocation(m_program, "diffuseTexture");
	int normalLocation = glGetUniformLocation(m_program, "normalTexture");
	int specularLocation = glGetUniformLocation(m_program, "specularTexture");

	glUniform1i(diffuseLocation, 0);
	glUniform1i(normalLocation, 1);
	glUniform1i(specularLocation, 2);
	

	//Load bones into shaders
	FBXSkeleton *skeleton = m_fbxFile->getSkeletonByIndex(0);
	updateBones(skeleton);
	unsigned int bonesUniform = glGetUniformLocation(m_program, "bones");
	glUniformMatrix4fv(bonesUniform, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);
	for (unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		FBXMeshNode* currMesh = m_fbxFile->getMeshByIndex(i);
		mat4 worldTransform = m_fbxFile->getMeshByIndex(i)->m_globalTransform;
		
		FBXMaterial* meshMaterial = currMesh->m_material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, meshMaterial->textures[FBXMaterial::DiffuseTexture]->handle);		
		
		unsigned int worldUniform = glGetUniformLocation(m_program, "world");
		glUniformMatrix4fv(worldUniform, 1, GL_FALSE, (float*)&worldTransform);

		glBindVertexArray(m_meshes[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_meshes[i].m_indexCount, GL_UNSIGNED_INT, 0);
	}

	Gizmos::draw(m_flyCamera.m_projView);

	Application::draw();
}

void Animation::generateGLMeshes(FBXFile* a_fbxFile)
{
	unsigned int meshCount = a_fbxFile->getMeshCount();

	m_meshes.resize(meshCount);

	for (unsigned int meshIndex = 0; meshIndex < meshCount; ++meshIndex)
	{
		FBXMeshNode* currMesh = a_fbxFile->getMeshByIndex(meshIndex);

		m_meshes[meshIndex].m_indexCount = currMesh->m_indices.size();

		glGenBuffers(1, &m_meshes[meshIndex].m_VBO);
		glGenBuffers(1, &m_meshes[meshIndex].m_IBO);
		glGenVertexArrays(1, &m_meshes[meshIndex].m_VAO);

		glBindVertexArray(m_meshes[meshIndex].m_VAO);

		for (unsigned int vertIndex = 0; vertIndex < currMesh->m_vertices.size(); ++vertIndex)
		{
			currMesh->m_vertices[vertIndex].normal = vec4(0, 1, 0, 0);
			currMesh->m_vertices[vertIndex].tangent = vec4(1, 0, 0, 0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_meshes[meshIndex].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex)* currMesh->m_vertices.size(),
			currMesh->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshes[meshIndex].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* currMesh->m_indices.size(),
			currMesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //pos
		glEnableVertexAttribArray(1); //texcoord
		glEnableVertexAttribArray(2); //bone indices
		glEnableVertexAttribArray(3); //bone weights
		glEnableVertexAttribArray(4); //normal
		glEnableVertexAttribArray(5); //tangent

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TangentOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Animation::evaluateSkeleton(FBXAnimation* a_anim, FBXSkeleton* a_skele, float a_timer)
{
	float fps = 24.0f;

	int currentFrame = (int)(a_timer * fps);

	//loop through all the tracks.
	for (unsigned int trackIndex = 0; trackIndex < a_anim->m_trackCount; ++trackIndex)
	{
		int trackFrameCount = a_anim->m_tracks[trackIndex].m_keyframeCount;
		int trackFrame = currentFrame % trackFrameCount;

		//find what keyframes are currently effecting the bones
		FBXKeyFrame currFrame =
			a_anim->m_tracks[trackIndex].m_keyframes[trackFrame];
		FBXKeyFrame nextFrame =
			a_anim->m_tracks[trackIndex].m_keyframes[(trackFrame + 1) % trackFrameCount];

		//interpolate between those keyframes to generate the matrix for the current pose
		float timeSinceFrameFlip = a_timer - (currentFrame / fps);
		float t = timeSinceFrameFlip * fps;

		vec3 newPos = glm::mix(currFrame.m_translation, nextFrame.m_translation, t);
		glm::quat newRot = glm::slerp(currFrame.m_rotation, nextFrame.m_rotation, t);
		vec3 newScale = glm::mix(currFrame.m_scale, nextFrame.m_scale, t);

		mat4 localTransform = glm::translate(newPos) *
			glm::toMat4(newRot) *
			glm::scale(newScale);

		//get the right track for the given bone
		int boneIndex = a_anim->m_tracks[trackIndex].m_boneIndex;

		//set the FBXNode's local transforms to match
		if (boneIndex < a_skele->m_boneCount) //Could of wrote wrapped entire function body in this
		{
			a_skele->m_nodes[boneIndex]->m_localTransform = localTransform;
		}
	}
}

void Animation::updateBones(FBXSkeleton* a_skele)
{
	//loop through the nodes in the skeleton
	for (unsigned int boneIndex = 0; boneIndex < a_skele->m_boneCount; ++boneIndex)
	{
		//generate their global transforms
		int parentIndex = a_skele->m_parentIndex[boneIndex];
	
		if (parentIndex == -1)
		{
			a_skele->m_bones[boneIndex] = a_skele->m_nodes[boneIndex]->m_localTransform;
		}
		else
		{
			a_skele->m_bones[boneIndex] = a_skele->m_bones[parentIndex] * 
				a_skele->m_nodes[boneIndex]->m_localTransform;
		}
	}

	for (unsigned int boneIndex = 0; boneIndex < a_skele->m_boneCount; ++boneIndex)
	{
		//multiply the global transform by the inverse bind pose
		a_skele->m_bones[boneIndex] = a_skele->m_bones[boneIndex] * a_skele->m_bindPoses[boneIndex];
	}
}

void Animation::loadTextures()
{
	int width, height, channels;

	unsigned char *data = stbi_load("./models/characters/enemyelite/EnemyElite1_D.tga",
		&width, &height, &channels, STBI_default);

	glGenTextures(1, &m_diffuseTexture);
	glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load("./models/characters/enemyelite/EnemyElite_N.tga",
		&width, &height, &channels, STBI_default);

	glGenTextures(1, &m_normalTexture);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load("./models/characters/enemyelite/EnemyElite_S.tga",
		&width, &height, &channels, STBI_default);

	glGenTextures(1, &m_specularTexture);
	glBindTexture(GL_TEXTURE_2D, m_specularTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}


/*
//FIXED CODE//
	float fps = 24.0f;

	int currentFrame = (int)(a_timer * fps);
	
	//loop through all the tracks.
	for (unsigned int trackIndex = 0; trackIndex < a_anim->m_trackCount; ++trackIndex)
	{		
		int trackFrameCount = a_anim->m_tracks[trackIndex].m_keyframeCount;
		int trackFrame = currentFrame % trackFrameCount;

		//find what keyframes are currently effecting the bones
		FBXKeyFrame currFrame = 
			a_anim->m_tracks[trackIndex].m_keyframes[trackFrame];
		FBXKeyFrame nextFrame = 
			a_anim->m_tracks[trackIndex].m_keyframes[(trackFrame + 1) % trackFrameCount];
		
		//interpolate between those keyframes to generate the matrix for the current pose
		float timeSinceFrameFlip = a_timer - (currentFrame / fps);
		float t = timeSinceFrameFlip * fps;
		
		vec3 newPos = glm::mix(currFrame.m_translation, nextFrame.m_translation, t);
		glm::quat newRot = glm::slerp(currFrame.m_rotation, nextFrame.m_rotation, t);
		vec3 newScale = glm::mix(currFrame.m_scale, nextFrame.m_scale, t);
		
		mat4 localTransform = glm::translate(newPos) * 
							  glm::toMat4(newRot) * 
							  glm::scale(newScale);

		//get the right track for the given bone
		int boneIndex = a_anim->m_tracks[trackIndex].m_boneIndex;

		//set the FBXNode's local transforms to match
		if (boneIndex < a_skele->m_boneCount) //Could of wrote wrapped entire function body in this
		{
			a_skele->m_nodes[boneIndex]->m_localTransform = localTransform;
		}
	}
*/

/*
//FUNNY AS HECK CODE//
	float fps = 1.0f / 24.0f;

	int currentFrame = (int)(a_timer / fps);
	
	//loop through all the tracks.
	for (unsigned int trackIndex = 0; trackIndex < a_anim->m_trackCount; ++trackIndex)
	{		
		int trackFrameCount = a_anim->m_tracks[trackIndex].m_keyframeCount;

		int trackFrame = currentFrame % trackFrameCount;

		//find what keyframes are currently effecting the bones
		FBXKeyFrame currFrame = 
			a_anim->m_tracks[trackIndex].m_keyframes[trackFrame];
		FBXKeyFrame nextFrame = 
			a_anim->m_tracks[trackIndex].m_keyframes[(trackFrame + 1) % trackFrameCount];
		
		//interpolate between those keyframes to generate the matrix for the current pose
		float timeSinceFrameFlip = a_timer - currentFrame;
		float t = timeSinceFrameFlip * fps;
		
		vec3 newPos = glm::mix(currFrame.m_translation, nextFrame.m_translation, t);
		glm::quat newRot = glm::slerp(currFrame.m_rotation, nextFrame.m_rotation, t);
		vec3 newScale = glm::mix(currFrame.m_scale, nextFrame.m_scale, t);
		
		mat4 localTransform = glm::translate(newPos) * 
							  glm::toMat4(newRot) * 
							  glm::scale(newScale);

		//get the right track for the given bone
		int boneIndex = a_anim->m_tracks[trackIndex].m_boneIndex;

		//set the FBXNode's local transforms to match
		a_skele->m_nodes[boneIndex]->m_localTransform = localTransform;
	}
*/