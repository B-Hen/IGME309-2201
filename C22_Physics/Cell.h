#pragma once

//---------------------------------------------------------
//Programmer: Breanna Henriquez 
//Date: 11/17/2020
//----------------------------------------------------------

#ifndef _CELL_H_
#define _CELL_H_

namespace Simplex
{
class Cell
{
public:
	//constuctor
	Cell();

	//constructor for point
	Cell(const Cell& otherCell);

	//copy constructor
	Cell& operator=(Cell const& otherCell);

	//destructor
	~Cell();

	//method to return a pointer to the cell id
	int* getID();

	//method to set the id of the cell
	void setID(int x, int z);

	//method to get the movement cost
	float getmovementCost();

	//method to set the movement cost
	void setMovementCost(float g);

	//method to get the H cost
	float getHeuisticsCost();

	//method to set the H cost
	void setHeuisticsCost(float h);

	//method to get the weight
	float getWeight();

	//method to set the wieght
	void setWeight(float f);

	//method to get if there is a hole
	bool getHole();

	//method to set if there is a hole 
	void setHole(bool isHole);

	//method to get the parent
	int* getParentID();

	//method to set the arent
	void setParentID();

private:
	int* ID;			  //cell number
	int IDSize;     
	float g;              //movement cost
	float h;			  //Heuistics Cost
	float f;			  //The weigth of the cell
	bool isHole;          //Check is there is a hole in the wall
	int* parentID;          //Array to hold the parent array
};
}

#endif //CELL_H_

