#include "AppClass.h"
void Application::InitVariables(void)
{
	//init the mesh
	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	m_pMesh->GenerateCube(1.0f, C_BLACK);
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	//
	//matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f,1.0f,1.0f));
	//static float value = 0.0f;
	//matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(0.0f, 0.0f, 0.0f));
	////value += 0.01f;
	//
	//matrix4 m4Model = m4Translate;
	////matrix4 m4Model = m4Scale * m4Translate;
	//
	//m_pMesh->Render(m4Projection, m4View, m4Model);

	//value to move the space invader over time
	static float value = 0.0f;

	//loop through the 2D array 
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			//if it equals 1 at a certain spot draw a box
			if (spaceArray[i][j] == 1)
			{
				//draw, move, and render the invader to the screen
				m_pMesh->Render(m4Projection, m4View, glm::translate(IDENTITY_M4, vector3((j - 15) + value, -i + 5, 0)));
			}
		}
	}
	
	value += 0.1;
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}