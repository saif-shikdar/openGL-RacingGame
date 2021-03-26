#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
	trackWidth = 60.0f;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3);

	return a + b * t + c * t2 + d * t3;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk

	m_controlPoints.push_back(glm::vec3(-100, 5, -282));
	m_controlPoints.push_back(glm::vec3(-100, 5, 0));
	m_controlPoints.push_back(glm::vec3(-71, 5, 71));
	m_controlPoints.push_back(glm::vec3(0, 5, 100));
	m_controlPoints.push_back(glm::vec3(71, 5, 71));
	m_controlPoints.push_back(glm::vec3(100, 10, 0));
	m_controlPoints.push_back(glm::vec3(71, 30, -71));
	m_controlPoints.push_back(glm::vec3(0, 50, -100));
	m_controlPoints.push_back(glm::vec3(-176, 50, -109));
	m_controlPoints.push_back(glm::vec3(-346, 50, -172));
	m_controlPoints.push_back(glm::vec3(-503, 50, -82));
	m_controlPoints.push_back(glm::vec3(-422, 50, 186));
	m_controlPoints.push_back(glm::vec3(243, 50, 99));
	m_controlPoints.push_back(glm::vec3(124, 5, -357));
	m_controlPoints.push_back(glm::vec3(217, 5, -459));
	m_controlPoints.push_back(glm::vec3(345, 5, -459));
	m_controlPoints.push_back(glm::vec3(409, 5, -364));
	m_controlPoints.push_back(glm::vec3(491, 5, -246));
	m_controlPoints.push_back(glm::vec3(491, 5, -85));
	m_controlPoints.push_back(glm::vec3(491, 5, 57));
	m_controlPoints.push_back(glm::vec3(491, 5, 170));
	m_controlPoints.push_back(glm::vec3(573, 30, 236));
	m_controlPoints.push_back(glm::vec3(735, 30, 170));
	m_controlPoints.push_back(glm::vec3(704, 70, 1));
	m_controlPoints.push_back(glm::vec3(533, 70, -109));
	m_controlPoints.push_back(glm::vec3(345, 70, -202));
	m_controlPoints.push_back(glm::vec3(60, 80, -344));
	m_controlPoints.push_back(glm::vec3(-500, 30, -540));
	m_controlPoints.push_back(glm::vec3(-20, 5, -771));

	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3& p, glm::vec3& up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size() - 1; i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}
}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points

	SetControlPoints();

	// Call UniformlySampleControlPoints with the number of samples required

	UniformlySampleControlPoints(500);

	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card

	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);

	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	
	glm::vec2 textcords(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (auto &centreLinePoint : m_centrelinePoints) {
		vbo.AddData(&centreLinePoint, sizeof(glm::vec3));
		vbo.AddData(&textcords, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}


void CCatmullRom::CreateOffsetCurves()
{
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively

	for (int i = 0; i < m_centrelinePoints.size() - 1; i++) {

		glm::vec3 p = m_centrelinePoints[i];
		glm::vec3 pNext = m_centrelinePoints[i + 1];
		
		glm::vec3 t = glm::normalize(pNext - p);
		glm::vec3 n = glm::normalize(glm::cross(t, glm::vec3(0, 1, 0)));
		glm::vec3 b = glm::normalize(glm::cross(n, t));

		glm::vec3 l = p - ((trackWidth / 2) * n);
		glm::vec3 r = p + ((trackWidth / 2) * n);

		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r);

	}

	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	// Note it is possible to only use one VAO / VBO with all the points instead.

	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);

	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (auto &leftOffsetPoint : m_leftOffsetPoints) {
		vbo.AddData(&leftOffsetPoint , sizeof(glm::vec3));
		vbo.AddData(&texCoord, sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);

	CVertexBufferObject vbo2;
	vbo2.Create();
	vbo2.Bind();

	for (auto & rightOffsetPoint : m_rightOffsetPoints) {
		vbo2.AddData(&rightOffsetPoint, sizeof(glm::vec3));
		vbo2.AddData(&texCoord, sizeof(glm::vec2));
		vbo2.AddData(&normal, sizeof(glm::vec3));
	}

	vbo2.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride2 = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride2, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride2, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride2,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

float CCatmullRom::GetWidth() 
{
	return trackWidth;
}


void CCatmullRom::CreateTrack()
{
	m_texture.Load("resources\\textures\\road.jpg"); // Downloaded from https://www.artstation.com/artwork/Bm1N2D on 26/03/21
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	vector<glm::vec2> textcords{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f,1.0f),
		
	};

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card

	for (int i = 0; i < m_centrelinePoints.size() - 1; i++) {
		m_triPoints.push_back(m_leftOffsetPoints[i]);
		m_triPoints.push_back(m_rightOffsetPoints[i]);
	}

	m_triPoints.push_back(m_leftOffsetPoints[0]);
	m_triPoints.push_back(m_rightOffsetPoints[0]);

	m_triPoints.push_back(m_leftOffsetPoints[1]);
	m_triPoints.push_back(m_rightOffsetPoints[1]);

	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);

	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < m_triPoints.size(); i++) {
		vbo.AddData(&m_triPoints[i], sizeof(glm::vec3));
		vbo.AddData(&textcords[i%4], sizeof(glm::vec2));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	m_vertexCount = m_triPoints.size();

	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	glGenVertexArrays(1, &m_vaoTrack2);
	glBindVertexArray(m_vaoTrack2);

	CVertexBufferObject vbo2;
	vbo2.Create();
	vbo2.Bind();

	for (int i = m_triPoints.size() - 1; i > 0; i--) {
		vbo2.AddData(&m_triPoints[i], sizeof(glm::vec3));
		vbo2.AddData(&textcords[i%4], sizeof(glm::vec2));
		vbo2.AddData(&normal, sizeof(glm::vec3));
	}

	vbo2.UploadDataToGPU(GL_STATIC_DRAW);

	m_vertexCount = m_triPoints.size();

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CCatmullRom::CreateTrackBarrier() {
	
	for (int i = 0; i < m_leftOffsetPoints.size(); i++) {
		m_leftBarrierPoints.push_back(m_leftOffsetPoints[i]);
		m_leftBarrierPoints.push_back(glm::vec3(m_leftOffsetPoints[i].x, m_leftOffsetPoints[i].y += 3, m_leftOffsetPoints[i].z));
	}

	m_leftBarrierPoints.push_back(m_leftOffsetPoints[0]);
	m_leftBarrierPoints.push_back(glm::vec3(m_leftOffsetPoints[0].x, m_leftOffsetPoints[0].y, m_leftOffsetPoints[0].z));

	m_leftBarrierPoints.push_back(m_leftOffsetPoints[1]);
	m_leftBarrierPoints.push_back(glm::vec3(m_leftOffsetPoints[1].x, m_leftOffsetPoints[1].y, m_leftOffsetPoints[1].z));

	m_leftBarrierPoints.push_back(m_leftOffsetPoints[2]);
	m_leftBarrierPoints.push_back(glm::vec3(m_leftOffsetPoints[2].x, m_leftOffsetPoints[2].y, m_leftOffsetPoints[2].z));

	for (int i = 0; i < m_rightOffsetPoints.size(); i++) {
		m_rightBarrierPoints.push_back(m_rightOffsetPoints[i]);
		m_rightBarrierPoints.push_back(glm::vec3(m_rightOffsetPoints[i].x, m_rightOffsetPoints[i].y += 3, m_rightOffsetPoints[i].z));
	}

	m_rightBarrierPoints.push_back(m_rightOffsetPoints[0]);
	m_rightBarrierPoints.push_back(glm::vec3(m_rightOffsetPoints[0].x, m_rightOffsetPoints[0].y, m_rightOffsetPoints[0].z));

	m_rightBarrierPoints.push_back(m_rightOffsetPoints[1]);
	m_rightBarrierPoints.push_back(glm::vec3(m_rightOffsetPoints[1].x, m_rightOffsetPoints[1].y, m_rightOffsetPoints[1].z));

	m_rightBarrierPoints.push_back(m_rightOffsetPoints[2]);
	m_rightBarrierPoints.push_back(glm::vec3(m_rightOffsetPoints[2].x, m_rightOffsetPoints[2].y, m_rightOffsetPoints[2].z));

	glm::vec3 normal(0.0f, 1.0f, 0.0f);

	glGenVertexArrays(1, &m_vaoLeftBarrier);
	glBindVertexArray(m_vaoLeftBarrier);

	CVertexBufferObject vbo;
	vbo.Create();
	vbo.Bind();

	for (int i = 0; i < m_leftBarrierPoints.size(); i++) {
		vbo.AddData(&m_leftBarrierPoints[i], sizeof(glm::vec3));
		vbo.AddData(&normal, sizeof(glm::vec3));
	}

	vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));

	glGenVertexArrays(1, &m_vaoRightBarrier);
	glBindVertexArray(m_vaoRightBarrier);

	CVertexBufferObject vbo2;
	vbo2.Create();
	vbo2.Bind();

	for (int i = 0; i < m_rightBarrierPoints.size(); i++) {
		vbo2.AddData(&m_rightBarrierPoints[i], sizeof(glm::vec3));
		vbo2.AddData(&normal, sizeof(glm::vec3));
	}

	vbo2.UploadDataToGPU(GL_STATIC_DRAW);

	// Vertex curve positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	// Vertex curve positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
}

void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it

	glLineWidth(1.0f);
	glBindVertexArray(m_vaoCentreline);
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size() - 1); 
}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it

	glLineWidth(1.0f);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glDrawArrays(GL_LINE_STRIP, 0, m_leftOffsetPoints.size() - 1);

	// Bind the VAO m_vaoRightOffsetCurve and render it

	glLineWidth(1.0f);
	glBindVertexArray(m_vaoRightOffsetCurve);
	glDrawArrays(GL_LINE_STRIP, 0, m_rightOffsetPoints.size() - 1);
}


void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it
	glBindVertexArray(m_vaoTrack);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glLineWidth(1.0f);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
	glBindVertexArray(m_vaoTrack2);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glLineWidth(1.0f);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
}

void CCatmullRom::RenderTrackBarrier() {

	glBindVertexArray(m_vaoLeftBarrier);
	glDrawArrays(GL_TRIANGLE_STRIP, 1, m_leftBarrierPoints.size() - 1);

	glBindVertexArray(m_vaoRightBarrier);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_rightBarrierPoints.size() - 1);
}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);