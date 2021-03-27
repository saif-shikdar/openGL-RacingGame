#include "Cube.h"

CCube::CCube()
{}

CCube::~CCube()
{
	Release();
}

void CCube::Create(string filename)
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

	glm::vec3 v0 = glm::vec3(-1, -1, -1);
	glm::vec3 v1 = glm::vec3(1, -1, -1);
	glm::vec3 v2 = glm::vec3(-1, 1, -1);
	glm::vec3 v3 = glm::vec3(1, 1, -1);
	glm::vec3 v4 = glm::vec3(-1, -1, 1);
	glm::vec3 v5 = glm::vec3(1, -1, 1);
	glm::vec3 v6 = glm::vec3(-1, 1, 1);
	glm::vec3 v7 = glm::vec3(1, 1, 1);

	// Normals

	glm::vec3 a = glm::vec3(0, 0, 1);
	glm::vec3 b = glm::vec3(0, 0, -1);
	glm::vec3 c = glm::vec3(-1, 0, 0);
	glm::vec3 d = glm::vec3(1, 0, 0);
	glm::vec3 e = glm::vec3(0, 1, 0);
	glm::vec3 f = glm::vec3(0, 1, 0);

	// Texture Coords

	glm::vec2 t0 = glm::vec2(0, 0);
	glm::vec2 t1 = glm::vec2(0, 1);
	glm::vec2 t2 = glm::vec2(1, 0);
	glm::vec2 t3 = glm::vec2(1, 1);
	glm::vec2 t4 = glm::vec2(0, 0);
	glm::vec2 t5 = glm::vec2(1, 0);
	glm::vec2 t6 = glm::vec2(0, 1);
	glm::vec2 t7 = glm::vec2(1, 1);

	// A

	m_VBO.AddData(&v4, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&a, sizeof(glm::vec3));
	m_VBO.AddData(&v5, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&a, sizeof(glm::vec3));
	m_VBO.AddData(&v6, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&a, sizeof(glm::vec3));
	m_VBO.AddData(&v7, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&a, sizeof(glm::vec3));

	// B

	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&b, sizeof(glm::vec3));
	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&b, sizeof(glm::vec3));
	m_VBO.AddData(&v3, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&b, sizeof(glm::vec3));
	m_VBO.AddData(&v2, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&b, sizeof(glm::vec3));

	// C

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&c, sizeof(glm::vec3));
	m_VBO.AddData(&v4, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&c, sizeof(glm::vec3));
	m_VBO.AddData(&v2, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&c, sizeof(glm::vec3));
	m_VBO.AddData(&v6, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&c, sizeof(glm::vec3));

	// D

	m_VBO.AddData(&v5, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&d, sizeof(glm::vec3));
	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&d, sizeof(glm::vec3));
	m_VBO.AddData(&v7, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&d, sizeof(glm::vec3));
	m_VBO.AddData(&v3, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&d, sizeof(glm::vec3));

	// E

	m_VBO.AddData(&v6, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&e, sizeof(glm::vec3));
	m_VBO.AddData(&v7, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&e, sizeof(glm::vec3));
	m_VBO.AddData(&v2, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&e, sizeof(glm::vec3));
	m_VBO.AddData(&v3, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&e, sizeof(glm::vec3));

	// F

	m_VBO.AddData(&v0, sizeof(glm::vec3));
	m_VBO.AddData(&t1, sizeof(glm::vec2));
	m_VBO.AddData(&f, sizeof(glm::vec3));
	m_VBO.AddData(&v1, sizeof(glm::vec3));
	m_VBO.AddData(&t0, sizeof(glm::vec2));
	m_VBO.AddData(&f, sizeof(glm::vec3));
	m_VBO.AddData(&v4, sizeof(glm::vec3));
	m_VBO.AddData(&t3, sizeof(glm::vec2));
	m_VBO.AddData(&f, sizeof(glm::vec3));
	m_VBO.AddData(&v5, sizeof(glm::vec3));
	m_VBO.AddData(&t2, sizeof(glm::vec2));
	m_VBO.AddData(&f, sizeof(glm::vec3));

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

void CCube::Render()
{
	glBindVertexArray(m_uiVAO);
	m_tTexture.Bind();
	// Call glDrawArrays to render the side
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
}

void CCube::Release()
{
	m_tTexture.Release();
	glDeleteVertexArrays(1, &m_uiVAO);
	m_VBO.Release();
}
