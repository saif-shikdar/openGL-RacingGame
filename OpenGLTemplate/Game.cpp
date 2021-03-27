/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 4.0a 30/01/2016
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 
*/


#include "game.h"
#include <iostream>

// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "Cube.h"
#include "Tetrahedron.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_ship = NULL;
	m_asteroid = NULL;
	m_pad = NULL;
	m_pSphere = NULL;
	m_pCube = NULL;
	m_pTetrahedron = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDistance = 0.0f;
	m_cameraSpeed = 0.0f;
	ship_position = glm::vec3();
	ship_rotation = glm::mat4();
	lap_number = 1;
	pedalUp = false;
	camView = 0;
	m_rotY = 0.0f;
	camB = glm::vec3();
	camN = glm::vec3();
	camT = glm::vec3();
	p = glm::vec3();
	offsetPos = 0;
	boostActive = false;
	time = 0;
	hitClock1 = false;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_ship;
	delete m_asteroid;
	delete m_pad;
	delete m_pSphere;
	delete m_pCube;
	delete m_pTetrahedron;
	delete m_pAudio;
	delete m_pCatmullRom;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	m_ship = new COpenAssetImportMesh;
	m_asteroid = new COpenAssetImportMesh;
	m_pad = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pCube = new CCube;
	m_pTetrahedron = new CTetrahedron;
	m_pAudio = new CAudio;
	m_pCatmullRom = new CCatmullRom;

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("sphereShader.vert");
	sShaderFileNames.push_back("sphereShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	//pMainProgram->AddShaderToProgram(&shShaders[2]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from https://wallpaperaccess.com/full/296718.jpg on 23/03/21
	m_pSkybox->Create(2500.0f);

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	m_ship->Load("resources\\models\\Ship\\shipA_OBJ.obj"); // Downloaded from https://www.turbosquid.com/3d-models/space-fighter-3ds-free/820608 on 12/03/21
	m_asteroid->Load("resources\\models\\Asteroid\\asteroid.obj"); // Downloaded from https://www.turbosquid.com/3d-models/asteroid-space-planet-3ds-free/616773 on 25/03/21
	m_pad->Load("resources\\models\\Pad\\speedboost.obj"); // Downloaded from https://drive.google.com/file/d/0B9dy0wb-EfqPYTVLWWQyaHc0Yk0/view on 24/03/21

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pCube->Create("resources\\textures\\clock.jpg"); // Texture downloaded from https://www.dreamstime.com/cartoon-blue-alarm-clock-hand-drawn-vector-illustration-isolated-white-background-cartoon-blue-alarm-clock-image119167009 on 27/03/21

	m_pTetrahedron->Create("resources\\textures\\dirtpile01.jpg"); 

	glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	//m_pAudio->Initialise();
	//m_pAudio->LoadEventSound("Resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	//m_pAudio->LoadMusicStream("Resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	//m_pAudio->PlayMusicStream();

	/*glm::vec3 p0 = glm::vec3(-500, 10, -200);
	glm::vec3 p1 = glm::vec3(0, 10, -200);
	glm::vec3 p2 = glm::vec3(0, 10, 200);
	glm::vec3 p3 = glm::vec3(-500, 10, 200);
	m_pCatmullRom->CreatePath(p0, p1, p2, p3);*/

	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();
	m_pCatmullRom->CreateTrack();
	m_pCatmullRom->CreateTrackBarrier();
}

// Render method runs repeatedly in a loop
void Game::Render() 
{
	
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("CubeMapTex", 1);
	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	// Set light and materials in main shader program
	glm::vec4 lightPosition1 = glm::vec4(-100, 100, -100, 1); // Position of light source *in world coordinates*
	glm::vec4 lightPosition2(50, 100, 50, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	/*// Switch to the sphere program
	CShaderProgram* pSphereProgram = (*m_pShaderPrograms)[1];
	pSphereProgram->UseProgram();

	// Set light and materials in sphere programme
	pSphereProgram->SetUniform("light1.position", viewMatrix * lightPosition1);
	pSphereProgram->SetUniform("light1.La", glm::vec3(1.0f, 0.0f, 1.0f));
	pSphereProgram->SetUniform("light1.Ld", glm::vec3(1.0f, 0.0f, 1.0f));
	pSphereProgram->SetUniform("light1.Ls", glm::vec3(1.0f, 0.0f, 1.0f));
	pSphereProgram->SetUniform("light1.direction", glm::normalize(viewNormalMatrix * glm::vec3(0, -1, 0)));
	pSphereProgram->SetUniform("light1.exponent", 20.0f);
	pSphereProgram->SetUniform("light1.cutoff", 30.0f);
	pSphereProgram->SetUniform("light2.position", viewMatrix * lightPosition2);
	pSphereProgram->SetUniform("light2.La", glm::vec3(1.0f, 1.0f, 0.0f));
	pSphereProgram->SetUniform("light2.Ld", glm::vec3(1.0f, 1.0f, 0.0f));
	pSphereProgram->SetUniform("light2.Ls", glm::vec3(1.0f, 1.0f, 0.0f));
	pSphereProgram->SetUniform("light2.direction", glm::normalize(viewNormalMatrix * glm::vec3(0, -1, 0)));
	pSphereProgram->SetUniform("light2.exponent", 20.0f);
	pSphereProgram->SetUniform("light2.cutoff", 30.0f);
	pSphereProgram->SetUniform("material1.shininess", 15.0f);
	pSphereProgram->SetUniform("material1.Ma", glm::vec3(0.0f, 0.0f, 0.2f));
	pSphereProgram->SetUniform("material1.Md", glm::vec3(0.0f, 0.0f, 1.0f));
	pSphereProgram->SetUniform("material1.Ms", glm::vec3(1.0f, 1.0f, 1.0f));*/

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render();
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	modelViewMatrixStack.Push(); {

		modelViewMatrixStack.Push(); {
			modelViewMatrixStack.Rotate(glm::vec3(0, 1, 0), m_rotY);
			modelViewMatrixStack.Translate(glm::vec3(1000, 30, 0));
			modelViewMatrixStack.Scale(100.0);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			m_pSphere->Render();
		} modelViewMatrixStack.Pop();

		modelViewMatrixStack.Push(); {
			modelViewMatrixStack.Rotate(glm::vec3(1, 0, 0), m_rotY);
			modelViewMatrixStack.Translate(glm::vec3(100, 30, 700));
			modelViewMatrixStack.Scale(30.0);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			m_pSphere->Render();
		} modelViewMatrixStack.Pop();

		// Render the asteroid
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), m_rotY);
		modelViewMatrixStack.Translate(glm::vec3(10.f, 5.0f, 0.0f));
		modelViewMatrixStack.Scale(1.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_asteroid->Render();
		modelViewMatrixStack.Pop();
	} modelViewMatrixStack.Pop();

	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	

	// Render the ship
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(ship_position);
	modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 80.1f);
	modelViewMatrixStack *= ship_rotation;
	modelViewMatrixStack.Scale(0.2f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_ship->Render();
	modelViewMatrixStack.Pop();

	// Render the tetrahedron
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 50.0f, 500.f));
	modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), 300);
	modelViewMatrixStack.Scale(100.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pTetrahedron->Render();
	modelViewMatrixStack.Pop();

	// Render the cube
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(50.0f, 10.0f, 95.0f));
	modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), m_rotY);
	modelViewMatrixStack.Scale(4.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	if (!hitClock1) {
		m_pCube->Render();
	}
	modelViewMatrixStack.Pop();

	// Render the cube2
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-282.f, 56.0f, -152.0f));
	modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), m_rotY);
	modelViewMatrixStack.Scale(4.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	if (!hitClock2) {
		m_pCube->Render();
	}
	modelViewMatrixStack.Pop();

	// Render the cube3
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(220.f, 35.0f, -107.0f));
	modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), m_rotY);
	modelViewMatrixStack.Scale(4.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	if (!hitClock3) {
		m_pCube->Render();
	}
	modelViewMatrixStack.Pop();

	// Render the cube4
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(458.f, 10.0f, -284.0f));
	modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), m_rotY);
	modelViewMatrixStack.Scale(4.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	if (!hitClock4) {
		m_pCube->Render();
	}
	modelViewMatrixStack.Pop();

	// Render the cube5
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(654.f, 78.0f, -33.0f));
	modelViewMatrixStack.Rotate(glm::vec3(1.0f, 0.0f, 0.0f), m_rotY);
	modelViewMatrixStack.Scale(4.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	if (!hitClock5) {
		m_pCube->Render();
	}
	modelViewMatrixStack.Pop();

	// Render the pad
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(-24, 2.5, -650));
	modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 3.f);
	modelViewMatrixStack.Scale(3.5f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pad->Render();
	modelViewMatrixStack.Pop();

	// Render the pad2
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(493, 70.0f, -115));
	modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 7.3f);
	modelViewMatrixStack.Scale(3.5f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pad->Render();
	modelViewMatrixStack.Pop();
		
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
	m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	

	// Render your object here
	modelViewMatrixStack.Pop();

	//m_pCatmullRom->RenderCentreline();
	//m_pCatmullRom->RenderOffsetCurves();
	m_pCatmullRom->RenderTrack();
	m_pCatmullRom->RenderTrackBarrier();

	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

}


// Update method runs repeatedly with the Render method
void Game::Update()
{
	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	//m_pCamera->Set(glm::vec3(0, 300, 0), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
	m_pCamera->Update(m_dt);

	if (!raceFinished) {
		time += m_dt;
	}

	m_rotY += 0.0005f * m_dt;

	// If up button is not held it will decelerate back to 0. If boost activated it will slowly decrease back to top speed.

	if (!pedalUp) {
		if (m_cameraSpeed >= 0) {
			m_cameraSpeed -= 0.0001 * m_dt;
		}
	}
	else {
		if (m_cameraSpeed < 0.195) {
			boostActive = false;
			m_cameraSpeed += 0.00005 * m_dt;
		}
		else if (m_cameraSpeed > 0.2) {
			boostActive = true;
			m_cameraSpeed -= 0.00005 * m_dt;
		}
	}

	// If speed goes below zero it corrects itself back to zero

	if (m_cameraSpeed < 0) {
		m_cameraSpeed = 0;
	}

	// Checks collision for boost pads. If collision occurs it will activate boost.

	if (checkDistance(glm::vec3(-24, 2.5, -650))) {
		if (pedalUp) {
			m_cameraSpeed = 0.3;
		}	
	}

	if (checkDistance(glm::vec3(493, 70.0f, -115))) {
		if (pedalUp) {
			m_cameraSpeed = 0.3;
		}
	}

	// Checks collision for clock powerups. If collision occurs it will decrease time by 5 secs.

	if (checkDistance(glm::vec3(50.0f, 10.0f, 95.0f))) {
		hitClock1 = true;
		time -= 50.0f;
	}

	if (checkDistance(glm::vec3(-282.f, 56.0f, -152.0f))) {
		hitClock2 = true;
		time -= 50.0f;
	}

	if (checkDistance(glm::vec3(220.f, 35.0f, -107.0f))) {
		hitClock3 = true;
		time -= 50.0f;
	}

	if (checkDistance(glm::vec3(458.f, 10.0f, -284.0f))) {
		hitClock4 = true;
		time -= 50.0f;
	}

	if (checkDistance(glm::vec3(654.f, 78.0f, -33.0f))) {
		hitClock5 = true;
		time -= 50.0f;
	}

	m_dt * 0.1f;
	m_currentDistance += m_cameraSpeed;

	m_pCatmullRom->Sample(m_currentDistance, p);

	glm::vec3 pNext;
	m_pCatmullRom->Sample(m_currentDistance + 1.0f, pNext);

	camT = glm::normalize(pNext - p);
	camN = glm::normalize(glm::cross(camT, glm::vec3(0,1,0)));
	camB = glm::normalize(glm::cross(camN, camT));

	lap_number = m_pCatmullRom->CurrentLap(m_currentDistance) + 1;

	ship_position = ((p - (15.0f * camT) + glm::vec3(0, 3, 0)) + offsetPos * camN);

	ship_rotation = glm::mat4(glm::mat3(camT, camB, camN));

	SwitchCamera();

	m_pAudio->Update();
}

// Camera switch method controlled using number keys.

void Game::SwitchCamera() {

	switch (camView)
	{
	case 0:
		m_pCamera->Set(p + (15.f * camB) - (45.f * camT), p - 15.0f * camT, glm::vec3(0, 1, 0));
		break;
	case 1:
		m_pCamera->Set(p + (50.f * camB) - (45.f * camT), p - 15.0f * camT, glm::vec3(0, 1, 0));
		break;
	case 2:
		m_pCamera->Set(p + (15.f * camB) + (0.f * camT) + (20.f * camN), p - 15.0f * camT, glm::vec3(0, 1, 0));
	case 3:
		m_pCamera->SetViewByMouse();
	default:
		break;
	}
}

bool Game::checkDistance(glm::vec3 c)
{
	glm::vec3 d = ship_position - c;
	if (glm::length(d) < 12.0f)
		return true;
	else
		return false;
}

void Game::DisplayFrameRate()
{


	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 40, 25, "FPS: %d", m_framesPerSecond);
		m_pFtFont->Render(20, height - 100, 25, "LAP: %d / 3", lap_number);
		m_pFtFont->Render(width - 400, height - 40, 25, "POSITION: %f %f", ship_position.x, ship_position.z);
		m_pFtFont->Render(width - 161, height - 510, 25, "SPEED: %f KPH", m_cameraSpeed * 1000);
		m_pFtFont->Render(20, height - 510, 25, "Time Elapsed: %f", time / 1000);
		if (lap_number > 3) {
			m_pFtFont->Render(width - 600, height - 200, 50, "RACE COMPLETE");
			pedalUp = false;
			raceFinished = true;
			m_pFtFont->Render(width - 650, height - 250, 50, "TIME TAKEN : %f", time / 1000);
		}
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{

	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch (LOWORD(w_param))
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			m_appActive = true;
			m_pHighResolutionTimer->Start();
			break;
		case WA_INACTIVE:
			m_appActive = false;
			break;
		}
		break;
	}

	case WM_SIZE:
		RECT dimensions;
		GetClientRect(window, &dimensions);
		m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch (w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			m_pAudio->PlayEventSound();
			break;
		case '2':
			camView = 0;
			break;
		case '3':
			camView = 1;
			break;
		case '4':
			camView = 2;
			break;
		case '5':
			camView = 3;
			break;
		case VK_UP:
			if (!raceFinished) {
				pedalUp = true;
			}
			break;
		case VK_DOWN:
			if (!raceFinished) {
				if (m_cameraSpeed >= 0.005) {
					m_cameraSpeed -= 0.005 * m_dt;
				}
			}
			break;
		case VK_LEFT:
			if (!raceFinished) {
				if (offsetPos > -20) {
					offsetPos -= 1.0f * m_dt;
				}
			}
			break;
		case VK_RIGHT:
			if (!raceFinished) {
				if (offsetPos < 20) {
					offsetPos += 1.0f * m_dt;
				}
			}
			break;
		}
		break;

	case WM_KEYUP:
		switch (w_param) {
		case VK_UP:
			pedalUp = false;
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
