#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class CCube;
class CTetrahedron;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	COpenAssetImportMesh *m_ship;
	COpenAssetImportMesh* m_asteroid;
	COpenAssetImportMesh* m_pad;

	CSphere *m_pSphere;
	CCube* m_pCube;
	CTetrahedron* m_pTetrahedron;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CCatmullRom *m_pCatmullRom;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;
	glm::vec3 ship_position;
	glm::mat4 ship_rotation;
	int lap_number;
	


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

	glm::vec3 camT;
	glm::vec3 camB;
	glm::vec3 camN;
	glm::vec3 p;

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	void SwitchCamera();
	bool checkDistance(glm::vec3 c);
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;
	float m_currentDistance;
	float m_cameraSpeed;
	bool pedalUp;
	int camView;
	float m_rotY;
	float offsetPos;
	bool boostActive;
	bool raceFinished;
	float time;
	bool hitClock1;
	bool hitClock2;
	bool hitClock3;
	bool hitClock4;
	bool hitClock5;
	float camAngleX;
};