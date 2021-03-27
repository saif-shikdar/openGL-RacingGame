#include "Tetrahedron.h"

CTetrahedron::CTetrahedron()
{}

CTetrahedron::~CTetrahedron()
{
	Release();
}

void CTetrahedron::Create(string filename)
{
	m_tTexture.Load(filename);
	m_tTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_tTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_tTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_tTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenVertexArrays(1, &m_uiVAO);
	glBindVertexArray(m_uiVAO);
	m_VBO.Create();
	m_VBO.Bind();

	// Write the code to add interleaved point, texture coord, and normal of the cube

	glm::vec3 v0 = glm::vec3(0, 0, 1); // A
	glm::vec3 v1 = glm::vec3(1, -1, -1); // B
	glm::vec3 v2 = glm::vec3(1, 1, -1); // C
	glm::vec3 v3 = glm::vec3(-1, 1, -1); // D
	glm::vec3 v4 = glm::vec3(-1, -1, -1); // E

	// Normals

	glm::vec3 frontFace = glm::vec3(-1, 0, -1);
	glm::vec3 backFace = glm::vec3(1, 0, 1);
	glm::vec3 leftFace = glm::vec3(0, -1, -1);
	glm::vec3 rightFace = glm::vec3(0, -1, 1);
	glm::vec3 bottomFace = glm::vec3(0, 0, -1);

	// Texture Coords

	glm::vec2 t0 = glm::vec2(0, 0);
	glm::vec2 t1 = glm::vec2(0, 1);
	glm::vec2 t2 = glm::vec2(1, 0);
	glm::vec2 t3 = glm::vec2(1, 1);

	// FrontFace

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&frontFace, sizeof(glm::vec3));
	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&frontFace, sizeof(glm::vec3));
	m_VBO.AddData(&v2, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&frontFace, sizeof(glm::vec3));

	// rightFace

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&rightFace, sizeof(glm::vec3));
	m_VBO.AddData(&v2, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&rightFace, sizeof(glm::vec3));
	m_VBO.AddData(&v3, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&rightFace, sizeof(glm::vec3));

	// BackFace

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&rightFace, sizeof(glm::vec3));
	m_VBO.AddData(&v3, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&rightFace, sizeof(glm::vec3));
	m_VBO.AddData(&v4, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&rightFace, sizeof(glm::vec3));

	// LeftFace

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&leftFace, sizeof(glm::vec3));
	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&leftFace, sizeof(glm::vec3));
	m_VBO.AddData(&v4, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&leftFace, sizeof(glm::vec3));

	// BottomFace

	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&bottomFace, sizeof(glm::vec3));
	m_VBO.AddData(&v2, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&bottomFace, sizeof(glm::vec3));
	m_VBO.AddData(&v3, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&bottomFace, sizeof(glm::vec3));
	m_VBO.AddData(&v4, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&bottomFace, sizeof(glm::vec3));

	// Upload data to GPU
	m_VBO.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CTetrahedron::Render()
{
	glBindVertexArray(m_uiVAO);
	m_tTexture.Bind();
	// Call glDrawArrays to render the side
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 3, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 6, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 9, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
}

void CTetrahedron::Release()
{
	m_tTexture.Release();
	glDeleteVertexArrays(1, &m_uiVAO);
	m_VBO.Release();
}
