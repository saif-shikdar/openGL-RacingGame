#pragma once

#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"

// Class for generating a unit cube
class CTetrahedron
{
public:
	CTetrahedron();
	~CTetrahedron();
	void Create(string filename);
	void Render();
	void Release();
private:
	GLuint m_uiVAO;
	CVertexBufferObject m_VBO;
	CTexture m_tTexture;
};
