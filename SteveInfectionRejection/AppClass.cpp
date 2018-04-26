#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 5.0f, -10.0f), //Position
		vector3(0.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	m_pEntityMngr->AddEntity("Minecraft\\Zombie.obj", "Main");
	m_pEntityMngr->UsePhysicsSolver();
	
	// spawn enemies that chase you
	for (int i = 0; i < 10; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve_Angry_" + std::to_string(i));
		vector3 v3Position = vector3(glm::sphericalRand(12.0f));
		v3Position.y = 0.0f;
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position);
		m_pEntityMngr->UsePhysicsSolver();
		//m_pEntityMngr->SetMass(i+1);
	}
	
	// spawn targets that run away from you
	for (int i = 0; i < 20; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve_Flee_" + std::to_string(i));
		vector3 v3Position = vector3(glm::sphericalRand(12.0f));
		v3Position.y = 0.0f;
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position);
		m_pEntityMngr->UsePhysicsSolver();
		//m_pEntityMngr->SetMass(i+1);
	}

#pragma region Audio
	// Directory to audio
	String sRoute = m_pSystem->m_pFolder->GetFolderData();
	sRoute += m_pSystem->m_pFolder->GetFolderAudio();

	// Background music
	m_soundBGM.openFromFile(sRoute + "infection_rejection_04.wav");
#pragma endregion

#pragma region Octree
	m_uOctantLevels = 0;
	m_pRoot = new MyOctant(m_uOctantLevels, 5);
#pragma endregion
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	
	static float fTimer = 0;
	static uint uClock = m_pSystem->GenClock();		//generate a new clock for that timer
	fTimer = m_pSystem->GetDeltaTime(uClock);		//get the delta time for that timer
	int time = static_cast<int>(fTimer);


	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();


	if ((time % 60) == 0)
	{
		SafeDelete(m_pRoot);
		m_pRoot = new MyOctant(m_uOctantLevels, 5);
	}

	//Update Entity Manager
	m_pEntityMngr->Update(fTimer);

	//Set the model matrix for the main object
	//m_pEntityMngr->SetModelMatrix(m_m4Steve, "Steve");

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	//m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//display octree
	if (!hideOctree) m_pRoot->Display(m_uOctantID);

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();

	//release GUI
	ShutdownGUI();
}