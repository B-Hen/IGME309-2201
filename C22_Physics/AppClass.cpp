#include "AppClass.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

using namespace Simplex;
void Application::InitVariables(void)
{
	//random seed to make the floor swiss cheese
	srand(time(NULL));

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(5.0f, 15.0f, 15.0f), //Position
		vector3(5.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	startList.push_back(vector3(6.0f, 1.0f, 8.0f));
	startList.push_back(vector3(2.0f, 1.0f, 2.0f));

	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve");
	m_pEntityMngr->SetPosition(vector3(5.0f, 1.0f, 3.0f), "Steve");
	m_stevePosition = vector3(5.0f, -1.1f, 3.0f);
	m_pEntityMngr->UsePhysicsSolver();

	m_pEntityMngr->AddEntity("Minecraft\\Cow.obj", "Cow");
	m_pEntityMngr->SetPosition(vector3(7.5f, 1.0f, 6.5f), "Cow");
	m_cowPosition = vector3(8.0f, -1.1f, 7.0f);
	m_pEntityMngr->UsePhysicsSolver();

	m_pEntityMngr->AddEntity("Minecraft\\Pig.obj", "Pig");
	m_pEntityMngr->SetPosition(vector3(0.5f, 1.0f, 4.5f), "Pig");
	m_pigPosition = vector3(1.0f, -1.1f, 5.0f);
	m_pEntityMngr->UsePhysicsSolver();

	//set the start cell index and the end cell index
	startIndexI = m_stevePosition.z;
	startIndexJ = m_stevePosition.x;
	targetIndexI = m_cowPosition.z;
	targetIndexJ = m_cowPosition.x;
	
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
				arrayOfCells[i][j]->setClosed(true);
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
			if (i == targetIndexI && j == targetIndexJ)
			{
				arrayOfCells[i][j]->setHeuisticsCost(0.0f);
			}

			//check if it is the start point and make its G value equal to 0
			if (i == startIndexI && j == startIndexJ)
			{
				arrayOfCells[i][j]->setMovementCost(0.0f);
			}
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

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);

	//if aStar is not done run the code
	if (aStarContinue)
	{
		aStar();
	}

	//loop through the close list of points and draw a path using planes
	for (int i = 0; i < closeList.size(); i++)
	{
		matrix4 m4Rotation = glm::rotate(IDENTITY_M4, glm::radians(-90.0f), vector3(1.0f, 0.0f, 0.0f));
		matrix4 planePosition = glm::translate(vector3(closeList[i]->getID()[1] - 0.5f, -0.01f, closeList[i]->getID()[0] - 0.5f)) * m4Rotation; //adjust by 0.5 to get in the cube perfectly
		m_pMeshMngr->AddPlaneToRenderList(planePosition, C_BLUE, 1);
	}

	//check to see if a new position needs to be made for steve and the cow once  they are colliding
	if (newPosition)
	{
		//-----------------------------------------------------------------------------------------------------------
		//ALL THIS CODE WAS TRYING TO GET IT TO WORK WITH A RANDOM START AND TARGET POSITION BUT IT WOULD BREAK
		//AND I COULD NOT FIRGUE OUT WHY

		//get random new start and taget places
		/*m_stevePosition = vector3((float)((rand() % 8) + 1), -1.1f, (float)((rand() % 8) + 1));
		m_pEntityMngr->SetPosition(vector3(m_stevePosition.x, 1.0f, m_stevePosition.z), "Steve");

		m_cowPosition = vector3((float)((rand() % 8) + 1), -1.1f, (float)((rand() % 8) + 1));
		m_pEntityMngr->SetPosition(vector3(m_cowPosition.x, 1.0f, m_cowPosition.z), "Cow");*/
		//-------------------------------------------------------------------------------------------------------------


		//if elses too show that a star works with different start and end points
		if (collision == 0)
		{
			m_pEntityMngr->SetPosition(vector3(2.0f, 1.0f, 5.0f), "Steve");
			m_stevePosition = vector3(2.0f, -1.1f, 5.0f);

			//set the new positon of the cow
			m_pEntityMngr->SetPosition(vector3(8.0f, 1.0f, 9.0f), "Cow");
			m_cowPosition = vector3(8.0f, -1.1f, 9.0f);

			collision++;
		}
		else if(collision == 1)
		{
			m_pEntityMngr->SetPosition(vector3(6.0f, 1.0f, 8.0f), "Steve");
			m_stevePosition = vector3(6.0f, -1.1f, 8.0f);

			//set the new positon of the cow
			m_pEntityMngr->SetPosition(vector3(3.0f, 1.0f, 2.0f), "Cow");
			m_cowPosition = vector3(3.0f, -1.1f, 2.0f);

			collision++;
		}
		else if (collision == 2)
		{
			m_pEntityMngr->SetPosition(vector3(1.0f, 1.0f, 7.0f), "Steve");
			m_stevePosition = vector3(1.0f, -1.1f, 7.0f);

			//set the new positon of the cow
			m_pEntityMngr->SetPosition(vector3(5.0f, 1.0f, 6.0f), "Cow");
			m_cowPosition = vector3(5.0f, -1.1f, 6.0f);

			collision++;
		}
		else if (collision == 3)
		{
			m_pEntityMngr->SetPosition(vector3(2.0f, 1.0f, 5.0f), "Steve");
			m_stevePosition = vector3(2.0f, -1.1f, 5.0f);

			//set the new positon of the cow
			m_pEntityMngr->SetPosition(vector3(3.0f, 1.0f, 6.0f), "Cow");
			m_cowPosition = vector3(3.0f, -1.1f, 6.0f);

			//reset the collision so that it loops through start and target points
			collision = 0;
		}

		std::cout << "Steve start position: (" << m_stevePosition.x << ", " << m_stevePosition.z << ")" << std::endl;
		std::cout << "Cow target position: (" << m_cowPosition.x << ", " << m_cowPosition.z << ")" << std::endl;

		//set the index for the start and target indecies
		startIndexI = m_stevePosition.z;
		startIndexJ = m_stevePosition.x;
		targetIndexI = m_cowPosition.z;
		targetIndexJ = m_cowPosition.x;

		//reset all the cells so a star can start again
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				//make it so random piece of the floor are missing aka different obstacles and weights
				if (arrayOfCells[i][j]->getHole())
				{
					continue;
				}

				//get and set the Heuistic Cost of each cell for A*
				arrayOfCells[i][j]->setHeuisticsCost(HCost(i, j, m_cowPosition));

				//set the moveent cost of each cell, each one will be a cost of 1.0f 
				arrayOfCells[i][j]->setMovementCost(1.0f);

				//reset parent
				arrayOfCells[i][j]->setParentCell(nullptr);

				//reset closed
				arrayOfCells[i][j]->setClosed(false);

				//check if it is the end point and make its H value equal to 0
				if (i == targetIndexI && j == targetIndexJ)
				{
					arrayOfCells[i][j]->setHeuisticsCost(0.0f);
				}

				//check if it is the start point and make its G value equal to 0
				if (i == startIndexI && j == startIndexJ)
				{
					arrayOfCells[i][j]->setMovementCost(0.0f);
				}
			}
		}

		//make astar true and run the method to get the new optimal path
		aStarContinue = true;
		aStar();

		//make new positon false so it only runs once
		newPosition = false;
	}
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//if the a star algorthim is done them move steve using lerp
	if (closeList.size() > 0)
	{
		//get a timer
		static float fTimer = 0;	//store the new timer
		static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
		fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

		//get the current position
		vector3 currentPosition; // = vector3(closeList[0]->getID[1], -1.1f, closeList[0]->getID[0]);

		//get the total time for steve to walk to a point in the path
		float animationTime = 2.0f;

		//get the start position and the end position
		vector3 startPosition;
		vector3 endPosition;

		//int to hold the place of stop
		static uint place = 0;

		if (place < closeList.size() - 1)
		{
			startPosition = vector3(closeList[place]->getID()[1] - 0.5f, 1.0, closeList[place]->getID()[0]);
			endPosition = vector3(closeList[place + 1]->getID()[1] - 0.5f, 1.0, closeList[place + 1]->getID()[0]);
		}
		else
		{
			//check if steve and the cow are colliding
			if (m_pEntityMngr->GetRigidBody("Steve")->IsColliding(m_pEntityMngr->GetRigidBody("Cow")))
			{
				//clear the list and then make the new position true
				closeList.clear();
				openList.clear();
				newPosition = true;
			}
		}

		//realtionship between the animation time adn the cuurent time
		float totalTime = MapValue(fTimer, 0.0f, animationTime, 0.0f, 1.0f);

		//change the postion based on time
		currentPosition = glm::lerp(startPosition, endPosition, totalTime);

		// reset tht total time and the place number
		if (totalTime >= 1.0f)
		{
			place++;
			fTimer = 0;
			place %= closeList.size();
		}

		matrix4 m4Model = glm::translate(currentPosition);

		m_pEntityMngr->SetPosition(currentPosition, "Steve");
	}

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
float Application::HCost(int x, int z, vector3 target)
{
	//use the distance formula to get the H cost 
	float Xs = x - target.x;
	float Zs = z - target.z;

	Xs = Xs * Xs;
	Zs = Zs * Zs;

	float h = sqrt(Xs + Zs);

	return h;
}

//method to do a star
void Application::aStar()
{
	//first start by getting the start cell and adding it to the close list 
	closeList.push_back(arrayOfCells[startIndexI][startIndexJ]);

	//loop through each cell until aStarContinue is true
	while (aStarContinue)
	{
		//start by looking at the cell to the North
		//check if the cell is valid and it isn't a hole
		if (isValid(startIndexI - 1, startIndexJ))
		{
			//if this cell is the target cell you have finished getting the optimal path
			if (startIndexI - 1 == targetIndexI && startIndexJ == targetIndexJ)
			{
				//set current cell as target cell parent
				arrayOfCells[startIndexI - 1][startIndexJ]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ]->setWeight(arrayOfCells[startIndexI - 1][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ]->getHeuisticsCost());

				//add it to the close list
				closeList.push_back(arrayOfCells[startIndexI - 1][startIndexJ]);

				//set aStarContinue to true
				aStarContinue = false;

				openList.clear();

				return;
			}
			//next check if there isn't a parent and the cell isn't a hole
			else if (arrayOfCells[startIndexI - 1][startIndexJ]->getParentCell() == nullptr && !arrayOfCells[startIndexI - 1][startIndexJ]->getHole() && !arrayOfCells[startIndexI - 1][startIndexJ]->getClosed())
			{
				//add this cell to the open list
				openList.push_back(arrayOfCells[startIndexI - 1][startIndexJ]);

				//assign the current cell as the parent
				arrayOfCells[startIndexI - 1][startIndexJ]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ]->setWeight(arrayOfCells[startIndexI - 1][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ]->getHeuisticsCost());
			}
			//condition for if the cell has a parent and is already on the open list
			else if (arrayOfCells[startIndexI - 1][startIndexJ]->getParentCell() != nullptr && !arrayOfCells[startIndexI - 1][startIndexJ]->getHole() && !arrayOfCells[startIndexI - 1][startIndexJ]->getClosed())
			{
				//check if the movement cost is lower with this current cell as a parent
				if (arrayOfCells[startIndexI - 1][startIndexJ]->getParentCell()->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ]->getmovementCost() >
					arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ]->getmovementCost())
				{
					//reparent this cell
					arrayOfCells[startIndexI - 1][startIndexJ]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

					//recalculate the movement cost
					arrayOfCells[startIndexI - 1][startIndexJ]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ]->getmovementCost());

					//recalculate the f cost 
					arrayOfCells[startIndexI - 1][startIndexJ]->setWeight(arrayOfCells[startIndexI - 1][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ]->getHeuisticsCost());
				}
			}
		}

		//check the cell that is northeast
		//check if the cell is valid and it isn't a hole
		if (isValid(startIndexI - 1, startIndexJ + 1))
		{
			//if this cell is the target cell you have finished getting the optimal path
			if (startIndexI - 1 == targetIndexI && startIndexJ + 1 == targetIndexJ)
			{
				//set current cell as target cell parent
				arrayOfCells[startIndexI - 1][startIndexJ + 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ + 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ + 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ + 1]->setWeight(arrayOfCells[startIndexI - 1][startIndexJ + 1]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ + 1]->getHeuisticsCost());

				//add it to the close list
				closeList.push_back(arrayOfCells[startIndexI - 1][startIndexJ + 1]);

				//set aStarContinue to true
				aStarContinue = false;

				openList.clear();

				return;
			}
			//next check if there isn't a parent and the cell isn't a hole
			else if (arrayOfCells[startIndexI - 1][startIndexJ + 1]->getParentCell() == nullptr && !arrayOfCells[startIndexI - 1][startIndexJ + 1]->getHole() && !arrayOfCells[startIndexI - 1][startIndexJ + 1]->getClosed())
			{
				//add this cell to the open list
				openList.push_back(arrayOfCells[startIndexI - 1][startIndexJ + 1]);

				//assign the current cell as the parent
				arrayOfCells[startIndexI - 1][startIndexJ + 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ + 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ + 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ + 1]->setWeight(arrayOfCells[startIndexI - 1][startIndexJ + 1]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ + 1]->getHeuisticsCost());
			}
			//condition for if the cell has a parent and is already on the open list
			else if (arrayOfCells[startIndexI - 1][startIndexJ + 1]->getParentCell() != nullptr && !arrayOfCells[startIndexI - 1][startIndexJ + 1]->getHole() && !arrayOfCells[startIndexI - 1][startIndexJ + 1]->getClosed())
			{
				//check if the movement cost is lower with this current cell as a parent
				if (arrayOfCells[startIndexI - 1][startIndexJ + 1]->getParentCell()->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ + 1]->getmovementCost() >
					arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ + 1]->getmovementCost())
				{
					//reparent this cell
					arrayOfCells[startIndexI - 1][startIndexJ + 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

					//recalculate the movement cost
					arrayOfCells[startIndexI - 1][startIndexJ + 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ + 1]->getmovementCost());

					//recalculate the f cost 
					arrayOfCells[startIndexI - 1][startIndexJ + 1]->setWeight(arrayOfCells[startIndexI - 1][startIndexJ + 1]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ + 1]->getHeuisticsCost());
				}
			}
		}

		//check the northwest cell next 
		//check if the cell is valid and it isn't a hole
		if (isValid(startIndexI - 1, startIndexJ - 1))
		{
			//if this cell is the target cell you have finished getting the optimal path
			if (startIndexI - 1 == targetIndexI && startIndexJ - 1 == targetIndexJ)
			{
				//set current cell as target cell parent
				arrayOfCells[startIndexI - 1][startIndexJ - 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ - 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ - 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ - 1]->setWeight(arrayOfCells[startIndexI - 1][startIndexJ - 1]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ - 1]->getHeuisticsCost());

				//add it to the close list
				closeList.push_back(arrayOfCells[startIndexI - 1][startIndexJ - 1]);

				//set aStarContinue to true
				aStarContinue = false;

				openList.clear();

				return;
			}
			//next check if there isn't a parent and the cell isn't a hole
			else if (arrayOfCells[startIndexI - 1][startIndexJ - 1]->getParentCell() == nullptr && !arrayOfCells[startIndexI - 1][startIndexJ - 1]->getHole() && !arrayOfCells[startIndexI - 1][startIndexJ - 1]->getClosed())
			{
				//add this cell to the open list
				openList.push_back(arrayOfCells[startIndexI - 1][startIndexJ - 1]);

				//assign the current cell as the parent
				arrayOfCells[startIndexI - 1][startIndexJ - 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ - 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ - 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI - 1][startIndexJ - 1]->setWeight(arrayOfCells[startIndexI - 1][startIndexJ - 1]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ - 1]->getHeuisticsCost());
			}
			//condition for if the cell has a parent and is already on the open list
			else if (arrayOfCells[startIndexI - 1][startIndexJ - 1]->getParentCell() != nullptr && !arrayOfCells[startIndexI - 1][startIndexJ - 1]->getHole() && !arrayOfCells[startIndexI - 1][startIndexJ - 1]->getClosed())
			{
				//check if the movement cost is lower with this current cell as a parent
				if (arrayOfCells[startIndexI - 1][startIndexJ - 1]->getParentCell()->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ + 1]->getmovementCost() >
					arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ - 1]->getmovementCost())
				{
					//reparent this cell
					arrayOfCells[startIndexI - 1][startIndexJ - 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

					//recalculate the movement cost
					arrayOfCells[startIndexI - 1][startIndexJ - 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ - 1]->getmovementCost());

					//recalculate the f cost 
					arrayOfCells[startIndexI - 1][startIndexJ - 1]->setWeight(arrayOfCells[startIndexI - 1][startIndexJ - 1]->getmovementCost() + arrayOfCells[startIndexI - 1][startIndexJ - 1]->getHeuisticsCost());
				}
			}
		}

		//check the south cell
		//check if the cell is valid and it isn't a hole
		if (isValid(startIndexI + 1, startIndexJ))
		{
			//if this cell is the target cell you have finished getting the optimal path
			if (startIndexI + 1 == targetIndexI && startIndexJ == targetIndexJ)
			{
				//set current cell as target cell parent
				arrayOfCells[startIndexI + 1][startIndexJ]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ]->setWeight(arrayOfCells[startIndexI + 1][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ]->getHeuisticsCost());

				//add it to the close list
				closeList.push_back(arrayOfCells[startIndexI + 1][startIndexJ]);

				//set aStarContinue to true
				aStarContinue = false;

				openList.clear();

				return;
			}
			//next check if there isn't a parent and the cell isn't a hole
			else if (arrayOfCells[startIndexI + 1][startIndexJ]->getParentCell() == nullptr && !arrayOfCells[startIndexI + 1][startIndexJ]->getHole() && !arrayOfCells[startIndexI + 1][startIndexJ]->getClosed())
			{
				//add this cell to the open list
				openList.push_back(arrayOfCells[startIndexI + 1][startIndexJ]);

				//assign the current cell as the parent
				arrayOfCells[startIndexI + 1][startIndexJ]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ]->setWeight(arrayOfCells[startIndexI + 1][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ]->getHeuisticsCost());
			}
			//condition for if the cell has a parent and is already on the open list
			else if (arrayOfCells[startIndexI + 1][startIndexJ]->getParentCell() != nullptr && !arrayOfCells[startIndexI + 1][startIndexJ]->getHole() && !arrayOfCells[startIndexI + 1][startIndexJ]->getClosed())
			{
				//check if the movement cost is lower with this current cell as a parent
				if (arrayOfCells[startIndexI + 1][startIndexJ]->getParentCell()->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ]->getmovementCost() >
					arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ]->getmovementCost())
				{
					//reparent this cell
					arrayOfCells[startIndexI + 1][startIndexJ]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

					//recalculate the movement cost
					arrayOfCells[startIndexI + 1][startIndexJ]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ]->getmovementCost());

					//recalculate the f cost 
					arrayOfCells[startIndexI + 1][startIndexJ]->setWeight(arrayOfCells[startIndexI + 1][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ]->getHeuisticsCost());
				}
			}
		}

		//check the south east cell
		//check if the cell is valid and it isn't a hole
		if (isValid(startIndexI + 1, startIndexJ + 1))
		{
			//if this cell is the target cell you have finished getting the optimal path
			if (startIndexI + 1 == targetIndexI && startIndexJ + 1 == targetIndexJ)
			{
				//set current cell as target cell parent
				arrayOfCells[startIndexI + 1][startIndexJ + 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ + 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ + 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ + 1]->setWeight(arrayOfCells[startIndexI + 1][startIndexJ + 1]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ + 1]->getHeuisticsCost());

				//add it to the close list
				closeList.push_back(arrayOfCells[startIndexI + 1][startIndexJ + 1]);

				//set aStarContinue to true
				aStarContinue = false;

				openList.clear();

				return;
			}
			//next check if there isn't a parent and the cell isn't a hole
			else if (arrayOfCells[startIndexI + 1][startIndexJ + 1]->getParentCell() == nullptr && !arrayOfCells[startIndexI + 1][startIndexJ + 1]->getHole() && !arrayOfCells[startIndexI + 1][startIndexJ + 1]->getClosed())
			{
				//add this cell to the open list
				openList.push_back(arrayOfCells[startIndexI + 1][startIndexJ + 1]);

				//assign the current cell as the parent
				arrayOfCells[startIndexI + 1][startIndexJ + 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ + 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ + 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ + 1]->setWeight(arrayOfCells[startIndexI + 1][startIndexJ + 1]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ + 1]->getHeuisticsCost());
			}
			//condition for if the cell has a parent and is already on the open list
			else if (arrayOfCells[startIndexI + 1][startIndexJ + 1]->getParentCell() != nullptr && !arrayOfCells[startIndexI + 1][startIndexJ + 1]->getHole() && !arrayOfCells[startIndexI + 1][startIndexJ + 1]->getClosed())
			{
				//check if the movement cost is lower with this current cell as a parent
				if (arrayOfCells[startIndexI + 1][startIndexJ + 1]->getParentCell()->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ + 1]->getmovementCost() >
					arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ + 1]->getmovementCost())
				{
					//reparent this cell
					arrayOfCells[startIndexI + 1][startIndexJ + 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

					//recalculate the movement cost
					arrayOfCells[startIndexI + 1][startIndexJ + 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ + 1]->getmovementCost());

					//recalculate the f cost 
					arrayOfCells[startIndexI + 1][startIndexJ + 1]->setWeight(arrayOfCells[startIndexI + 1][startIndexJ + 1]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ + 1]->getHeuisticsCost());
				}
			}
		}

		//check the south west cell
		//check if the cell is valid and it isn't a hole
		if (isValid(startIndexI + 1, startIndexJ - 1))
		{
			//if this cell is the target cell you have finished getting the optimal path
			if (startIndexI + 1 == targetIndexI && startIndexJ - 1 == targetIndexJ)
			{
				//set current cell as target cell parent
				arrayOfCells[startIndexI + 1][startIndexJ - 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ - 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ - 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ - 1]->setWeight(arrayOfCells[startIndexI + 1][startIndexJ - 1]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ - 1]->getHeuisticsCost());

				//add it to the close list
				closeList.push_back(arrayOfCells[startIndexI + 1][startIndexJ - 1]);

				//set aStarContinue to true
				aStarContinue = false;

				openList.clear();

				return;
			}
			//next check if there isn't a parent and the cell isn't a hole
			else if (arrayOfCells[startIndexI + 1][startIndexJ - 1]->getParentCell() == nullptr && !arrayOfCells[startIndexI + 1][startIndexJ - 1]->getHole() && !arrayOfCells[startIndexI + 1][startIndexJ - 1]->getClosed())
			{
				//add this cell to the open list
				openList.push_back(arrayOfCells[startIndexI + 1][startIndexJ - 1]);

				//assign the current cell as the parent
				arrayOfCells[startIndexI + 1][startIndexJ - 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ - 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ - 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI + 1][startIndexJ - 1]->setWeight(arrayOfCells[startIndexI + 1][startIndexJ - 1]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ - 1]->getHeuisticsCost());
			}
			//condition for if the cell has a parent and is already on the open list
			else if (arrayOfCells[startIndexI + 1][startIndexJ - 1]->getParentCell() != nullptr && !arrayOfCells[startIndexI + 1][startIndexJ - 1]->getHole() && !arrayOfCells[startIndexI + 1][startIndexJ - 1]->getClosed())
			{
				//check if the movement cost is lower with this current cell as a parent
				if (arrayOfCells[startIndexI + 1][startIndexJ - 1]->getParentCell()->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ - 1]->getmovementCost() >
					arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ - 1]->getmovementCost())
				{
					//reparent this cell
					arrayOfCells[startIndexI + 1][startIndexJ - 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

					//recalculate the movement cost
					arrayOfCells[startIndexI + 1][startIndexJ - 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ - 1]->getmovementCost());

					//recalculate the f cost 
					arrayOfCells[startIndexI + 1][startIndexJ - 1]->setWeight(arrayOfCells[startIndexI + 1][startIndexJ - 1]->getmovementCost() + arrayOfCells[startIndexI + 1][startIndexJ - 1]->getHeuisticsCost());
				}
			}
		}

		//check the east cell
		//check if the cell is valid and it isn't a hole
		if (isValid(startIndexI, startIndexJ + 1))
		{
			//if this cell is the target cell you have finished getting the optimal path
			if (startIndexI == targetIndexI && startIndexJ + 1 == targetIndexJ)
			{
				//set current cell as target cell parent
				arrayOfCells[startIndexI][startIndexJ + 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI][startIndexJ + 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ + 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI][startIndexJ + 1]->setWeight(arrayOfCells[startIndexI][startIndexJ + 1]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ + 1]->getHeuisticsCost());

				//add it to the close list
				closeList.push_back(arrayOfCells[startIndexI][startIndexJ + 1]);

				//set aStarContinue to true
				aStarContinue = false;

				openList.clear();

				return;
			}
			//next check if there isn't a parent and the cell isn't a hole
			else if (arrayOfCells[startIndexI][startIndexJ + 1]->getParentCell() == nullptr && !arrayOfCells[startIndexI][startIndexJ + 1]->getHole() && !arrayOfCells[startIndexI][startIndexJ + 1]->getClosed())
			{
				//add this cell to the open list
				openList.push_back(arrayOfCells[startIndexI][startIndexJ + 1]);

				//assign the current cell as the parent
				arrayOfCells[startIndexI][startIndexJ + 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI][startIndexJ + 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ + 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI][startIndexJ + 1]->setWeight(arrayOfCells[startIndexI][startIndexJ + 1]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ + 1]->getHeuisticsCost());
			}
			//condition for if the cell has a parent and is already on the open list
			else if (arrayOfCells[startIndexI][startIndexJ + 1]->getParentCell() != nullptr && !arrayOfCells[startIndexI][startIndexJ + 1]->getHole() && !arrayOfCells[startIndexI][startIndexJ + 1]->getClosed())
			{
				//check if the movement cost is lower with this current cell as a parent
				if (arrayOfCells[startIndexI][startIndexJ + 1]->getParentCell()->getmovementCost() + arrayOfCells[startIndexI][startIndexJ + 1]->getmovementCost() >
					arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ + 1]->getmovementCost())
				{
					//reparent this cell
					arrayOfCells[startIndexI][startIndexJ + 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

					//recalculate the movement cost
					arrayOfCells[startIndexI][startIndexJ + 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ + 1]->getmovementCost());

					//recalculate the f cost 
					arrayOfCells[startIndexI][startIndexJ + 1]->setWeight(arrayOfCells[startIndexI][startIndexJ + 1]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ + 1]->getHeuisticsCost());
				}
			}
		}

		//check the west cell
		if (isValid(startIndexI, startIndexJ - 1))
		{
			//if this cell is the target cell you have finished getting the optimal path
			if (startIndexI == targetIndexI && startIndexJ - 1 == targetIndexJ)
			{
				//set current cell as target cell parent
				arrayOfCells[startIndexI][startIndexJ - 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI][startIndexJ - 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ - 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI][startIndexJ - 1]->setWeight(arrayOfCells[startIndexI][startIndexJ - 1]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ - 1]->getHeuisticsCost());

				//add it to the close list
				closeList.push_back(arrayOfCells[startIndexI][startIndexJ - 1]);

				//set aStarContinue to true
				aStarContinue = false;

				openList.clear();

				return;
			}
			//next check if there isn't a parent and the cell isn't a hole
			else if (arrayOfCells[startIndexI][startIndexJ - 1]->getParentCell() == nullptr && !arrayOfCells[startIndexI][startIndexJ - 1]->getHole() && !arrayOfCells[startIndexI][startIndexJ - 1]->getClosed())
			{
				//add this cell to the open list
				openList.push_back(arrayOfCells[startIndexI][startIndexJ - 1]);

				//assign the current cell as the parent
				arrayOfCells[startIndexI][startIndexJ - 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

				//get the movement cost of the cell
				arrayOfCells[startIndexI][startIndexJ - 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ - 1]->getmovementCost());

				//get the f cost of the cell
				arrayOfCells[startIndexI][startIndexJ - 1]->setWeight(arrayOfCells[startIndexI][startIndexJ - 1]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ - 1]->getHeuisticsCost());
			}
			//condition for if the cell has a parent and is already on the open list
			else if (arrayOfCells[startIndexI][startIndexJ - 1]->getParentCell() != nullptr && !arrayOfCells[startIndexI][startIndexJ - 1]->getHole() && !arrayOfCells[startIndexI][startIndexJ - 1]->getClosed())
			{
				//check if the movement cost is lower with this current cell as a parent
				if (arrayOfCells[startIndexI][startIndexJ - 1]->getParentCell()->getmovementCost() + arrayOfCells[startIndexI][startIndexJ - 1]->getmovementCost() >
					arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ - 1]->getmovementCost())
				{
					//reparent this cell
					arrayOfCells[startIndexI][startIndexJ - 1]->setParentCell(arrayOfCells[startIndexI][startIndexJ]);

					//recalculate the movement cost
					arrayOfCells[startIndexI][startIndexJ - 1]->setMovementCost(arrayOfCells[startIndexI][startIndexJ]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ - 1]->getmovementCost());

					//recalculate the f cost 
					arrayOfCells[startIndexI][startIndexJ - 1]->setWeight(arrayOfCells[startIndexI][startIndexJ - 1]->getmovementCost() + arrayOfCells[startIndexI][startIndexJ - 1]->getHeuisticsCost());
				}
			}
		}

		//float to hold the largest f value
		float smallest = FLT_MAX;
		int index = 0;

		//lloop through the open list for the cell with the lowest f cost
		for (int i = 0; i < openList.size(); i++)
		{
			//check to see if the f value is smaller than the smallest
			if (openList[i]->getWeight() < smallest)
			{
				//make a new startIndexI and startIndexJ
				//indecies = openList[i]->getID();

				//set a new smallest
				smallest = openList[i]->getWeight();

				//make the new indecies for the current cell
				startIndexI = openList[i]->getID()[0]; //indecies[0];
				startIndexJ = openList[i]->getID()[1]; //indecies[1];

				index = i;
			}
		}

		//remove the cell from the list
		openList.erase(openList.begin() + index);

		//openList.clear();

		//add this new cell to the closed list
		closeList.push_back(arrayOfCells[startIndexI][startIndexJ]);

		//openList.clear();

		//make the new cell closed
		arrayOfCells[startIndexI][startIndexJ]->setClosed(true);
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

	return false;
}

