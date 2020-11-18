#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(5.0f, 15.0f, 15.0f), //Position
		vector3(5.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve");
	m_pEntityMngr->SetPosition(vector3(5.0f, 1.0f, 3.0f), "Steve");
	m_stevePosition = vector3(5.0f, -1.1f, 3.0f);
	m_pEntityMngr->UsePhysicsSolver();

	m_pEntityMngr->AddEntity("Minecraft\\Cow.obj", "Cow");
	m_pEntityMngr->SetPosition(vector3(8.0f, 1.0f, 7.0f), "Cow");
	m_cowPosition = vector3(8.0f, -1.1f, 7.0f);
	m_pEntityMngr->UsePhysicsSolver();

	targetX = m_cowPosition.x;
	targetZ = m_cowPosition.z;

	startX = m_stevePosition.x;
    startZ = m_stevePosition.z;
	
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			arrayOfCells[i][j] = new Cell();
			arrayOfCells[i][j]->setID(i, j);

			//make it so random piece of the floor are missing aka different obstacles and weights
			if (rand() % 10 < 1)
			{
				arrayOfCells[i][j]->setHole(true);
				continue;
			}

			//draw the floor out of minecraft blocks
			m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube_" + std::to_string(i));
			vector3 v3Position = vector3((i * 1), -1.1, (j * 1));
			matrix4 m4Position = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Position * glm::scale(vector3(1.0f)));

			//get and set the Heuistic Cost of each cell for A*
			arrayOfCells[i][j]->setHeuisticsCost(HCost(i, j, m_cowPosition));

			//set the moveent cost of each cell, each one will be a cost of 1.0f 
			arrayOfCells[i][j]->setMovementCost(1.0f);

			//check if it is the end point and make its H value equal to 0
			if (i == targetX && j == targetZ)
			{
				arrayOfCells[i][j]->setHeuisticsCost(0.0f);
			}

			//check if it is the start point and make its G value equal to 0
			if (i == startX && j == startZ)
			{
				arrayOfCells[i][j]->setMovementCost(0.0f);
			}

			//std::cout << v3Position.x << "" << v3Position.y << "" << v3Position.z << std::endl;
			//m_pEntityMngr->UsePhysicsSolver();
			//std::cout << arrayOfCells[i][j]->getmovementCost() << std::endl;
		}
	}
	//std::cout << m_cowPosition.x << ", " << m_cowPosition.y << ", " << m_cowPosition.z << std::endl;
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Set the model matrix for the main object
	//m_pEntityMngr->SetModelMatrix(m_m4Steve, "Steve");

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	//m_pEntityMngr->AddEntityToRenderList(-1, true);

	aStar();

	matrix4 m4Rotation = glm::rotate(IDENTITY_M4, glm::radians(-90.0f), vector3(1.0f, 0.0f, 0.0f));
	vector3 plane = vector3(10.0f, 4.1f, 10.0f);
	matrix4 planePosition = glm::translate(vector3(4.5f, -0.011f, 2.5f)) * m4Rotation; //adjust by 0.5 to get in the cube perfectly
	m_pMeshMngr->AddPlaneToRenderList(planePosition, C_RED, 1);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

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

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			SafeDelete(arrayOfCells[i][j]);
		}
	}

	//release GUI
	ShutdownGUI();
}

//method to get the heuristic value
float Application::HCost(int x, int y, vector3 target)
{
	//use the distance formula to get the H cost 
	float Xs = x - target.x;
	float Ys = y - target.y;

	Xs = Xs * Xs;
	Ys = Ys * Ys;

	float h = sqrt(Xs + Ys);
	return h;
}

//method to do a star
void Application::aStar()
{
	//start by making the start node the current node
	Cell* current = arrayOfCells[startX][startZ];  //safe delete this later

	//add the start to the open list
	openList.push_back(current);

	//loop through and check all the possible cells around current
	while (!aStaarContinue)
	{
		//first check if North cell is a valid cell
		if (isValid(startX, startZ))
		{
			std::cout << "working" << std::endl;
			break;
		}
	}
}

//check if the cells is valid to do a star
bool Application::isValid(int x, int z)
{
	//check to see if the cell is in bounds
	if (x > -1 && x < 10 && z > -1 && z < 10)
	{
		return true;
	}
}