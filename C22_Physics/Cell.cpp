#include "Cell.h"
#include "MyEntityManager.h"
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;
using namespace Simplex;

Simplex::Cell::Cell()
{
	//set the properties when the cell is made
	IDSize = 2;
	ID = new int[IDSize]();
	parent = nullptr;
	g = FLT_MAX;
	h = FLT_MAX;
	f = FLT_MAX;
	isHole = false;
	closed = false;
}

Simplex::Cell::Cell(const Cell& otherCell)
{
	//now copy the elements of th ID of the other cell
	IDSize = otherCell.IDSize;
	ID = new int[IDSize]();
	copy(otherCell.ID, otherCell.ID + otherCell.IDSize, ID);
	parent = nullptr;
	g = otherCell.g;
	h = otherCell.h;
	f = otherCell.f;
	isHole = otherCell.isHole;
	closed = otherCell.closed;
}


Cell& Simplex::Cell::operator=(Cell const& otherCell)
{
	//check to see if it is the same cell
	if (this == &otherCell)
	{
		return *this;
	}

	//if it isn't the same get the ID
	if (ID != nullptr)
	{
		delete[] ID;
		ID = nullptr;
		IDSize = 2;
	}

	//now copy the elements of th ID of the other cell
	IDSize = otherCell.IDSize;
	ID = new int[IDSize]();
	copy(otherCell.ID, otherCell.ID + otherCell.IDSize, ID);

	parent = nullptr;
	g = otherCell.g;
	h = otherCell.h;
	f = otherCell.f;
	isHole = otherCell.isHole;
	closed = otherCell.closed;

	return *this;
}

Simplex::Cell::~Cell()
{
	//set the arrary for ID to nullptr
	delete[] ID;
	ID = nullptr;
	IDSize = 2;

	parent = nullptr;

	//reset the other values
	g = FLT_MAX;
	h = FLT_MAX;
	f = FLT_MAX;
	isHole = false;
}

int* Simplex::Cell::getID()
{
	return ID;
}

void Simplex::Cell::setID(int x, int z)
{
	ID[0] = x;
	ID[1] = z;
}

float Simplex::Cell::getmovementCost()
{
	return g;
}

void Simplex::Cell::setMovementCost(float g)
{
	this->g = g;
}

float Simplex::Cell::getHeuisticsCost()
{
	return h;
}

void Simplex::Cell::setHeuisticsCost(float h)
{
	this->h = h;
}

float Simplex::Cell::getWeight()
{
	return f;
}

void Simplex::Cell::setWeight(float f)
{
	this->f = f;
}

bool Simplex::Cell::getHole()
{
	return isHole;
}

void Simplex::Cell::setHole(bool isHole)
{
	this->isHole = isHole;
}

Cell* Simplex::Cell::getParentCell()
{
	return parent;
}

void Simplex::Cell::setParentCell(Cell* parent)
{
	this->parent = parent;
}

bool Simplex::Cell::getClosed()
{
	return closed;
}

void Simplex::Cell::setClosed(bool closed)
{
	this->closed = closed;
}
