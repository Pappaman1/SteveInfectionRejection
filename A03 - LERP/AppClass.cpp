#include "AppClass.h"
#define _USE_MATH_DEFINES
#include <math.h>
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Zachary Winnewisser - zmw7710@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits
	float fRadius = 0.95f;
	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		currentPosList.push_back(0);
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
	fSize = 1.0f;
	for (uint i = 0; i < m_uOrbits; i++) { //loop through the # of orbits
		std::vector<vector3> stopList; //create a vector to hold all of the stops
		for (int j = 0; j < 3 + i; j++)
		{
			double angle = (2 * M_PI) * j / (3 + i); //this loop adds another stop the list
			double s = sin(angle);
			double c = cos(angle);
			stopList.push_back(vector3(fSize * c, 0.0f, fSize * s));
		}
		sidesList.push_back(stopList); //push the stops onto the list of sides
		fSize += 0.5f;
	}


}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	//keep all of the mapValue stuff outside of the loop in order to prevent it from effecting only the first orbit
	static DWORD startTime = GetTickCount(); //start timer
	DWORD currentTime = GetTickCount(); //current time
	float maxTime = 2.0f;
	float timeSinceStart = (currentTime - startTime) / 1000.0f; //self explanatory
	float mapValue = MapValue(timeSinceStart, 0.0f, maxTime, 0.0f, 1.0f);

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));

		//calculate the current position
		vector3 v3CurrentPos = ZERO_V3;

		v3CurrentPos = glm::lerp(sidesList[i][currentPosList[i]], sidesList[i][(currentPosList[i] + 1) % sidesList[i].size()], mapValue); //lerp to the correct locations using currentPosList

		v3CurrentPos = glm::rotate(v3CurrentPos, 90.0f, AXIS_X); //rotate v3CurrentPos 90 degrees to prevent the spheres from lerping in the z direction and to make it look correct

		if (mapValue >= 1)
		{
			startTime = GetTickCount();
			currentPosList[i] = (currentPosList[i] + 1) % sidesList[i].size();
		}

		if (timeSinceStart > maxTime)
		{
			startTime = GetTickCount();
		}

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);


		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}