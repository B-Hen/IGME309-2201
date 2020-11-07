#pragma once
//-------------------------------------------------------
//Programmer: Breanna Henriquez
//Date 11/5/2020
//-------------------------------------------------------

#ifndef _MYOCTANTCLASS_H_
#define _MYOCTANTCLASS_H_

#include "MyEntityManager.h"

namespace Simplex
{


//system Class
class MyOctant
{
	static uint m_uOctantCount; //will store the number of octants instantiated
	static uint m_uMaxLevel;    //will store the maximum level an octant can go to
	static uint m_uIdeaEntityCount; //will tell how many idea Entities this objct will contain

	uint m_uID = 0;		   //will sotre the current ID for this octant
	uint m_uLevel = 0;	   //will store the curent level of the octant
	uint m_uChildren = 0;  //Number of children on the octant (either 0 or 8)

	float m_fSize = 0.0f;  //Size of the octant

	MeshManager* m_pMeshMngr = nullptr; //Mesh manager singleton
	MyEntityManager* m_pEntityMngr = nullptr; //Entity Manager Singleton

	vector3 m_v3Center = vector3(0.0f); // Will store the center point of th octant
	vector3 m_v3Min = vector3(0.0f);   //Will store the mimimum vector of the ofctant
	vector3 m_v3Max = vector3(0.0f);    //Will store the maximum vector of the octant

	MyOctant* m_pParent = nullptr;  //Will store the parent of current octant
	MyOctant* m_pChild[8]; //Will store the children of the current octant

	std::vector<uint> m_EntityList; //List of Entites under this octant (Index in Entity Manager)

	MyOctant* m_pRoot = nullptr; //Root octant
	std::vector<MyOctant*> m_lChild; //List of nodes that contains objects (this will be applied to root only)

public:
	/*
	USAGE: Constructor, will create an octant containing all MagnaEntities INstances in the Mesh
	manager currently contains
	ARGUMENTS:
	-uint a_nMaxKevek = 2 - >sets the maximum level of subdivisions
	-uitn nIdealEntityCount = 5-> Sets the idea level of objects per octant
	OUTPUT: class object
	*/
	MyOctant(uint a_nMaxLevel = 2, uint a_nIdeaEntityCount = 5);
	/*
	USAGE: Constructor
	ARGUMENTS:
	-vector3 a_v3Center ->Center of the octant in gloabal space
	-float a_fSize - > Size of each side of the octant volume
	OutPut: class object
	*/
	MyOctant(vector3 a_v3Center, float a_fSize);
	/*
	USAGE: Copy Constructor
	ARGYMENTS: class object to copy
	OUTPUT: class object instance
	*/
	MyOctant(MyOctant const& other);
	/*
	USAGE:Copr Asignment Operator
	ARGUMENTS: class object to copy'
	OUTPUT: ---
	*/
	MyOctant& operator=(MyOctant const& other);
	/*
	USAGE: Destructor
	ARGUMENTS:---
	OUTPUT:----
	*/
	~MyOctant(void);
	/*
	USAGE: Changes object contents for other objects's
	ARGUMENTS:
	-MyOctant& other -> object to swap content from
	OUTPUT: ----
	*/
	void Swap(MyOctant& other);
	/*
	USAGE: Gets this octant's size
	ARGUMENTS: ----
	OUTPUT: size of octant
	*/
	float GetSize(void);
	/*
	USAGE: gets the center of the octant in global space
	ARGUMENTS: ---
	OUTPUT: Center of the octant in global space
	*/
	vector3 GetCenterGlobal(void);
	/*
	USAGE: Gets the min center of the octant in global space
	ARGUMENTS: --- 
	OUTPUT: Minimum in global space
	*/
	vector3 GetMinGlobal(void);
	/*
	USAGE: gets the max cocrner of the octant in global space
	ARGUMENTS: ---
	OUTPUT: Maximum in global space
	*/
	vector3 GetMaxGlobal(void);
	/*
	USAGE: Ask if there is a collision with the Entity specified by index from the Bounding Object Manager
	ARGUMENTS:
	-int a_uRBIndex -> Index of the Enitity in the Entity Manager
	OUTPUT: check of the collision
	*/
	bool IsCollidiing(uint a_uRBIndex);
	/*
	USAGE: Displays the MyOctant volume specified by index including the objects underneath
	ARGUMENTS: - vector3 a_v3Color = CYELLO - > Color of the volume to display
	OUTPUT: ---
	*/
	void Display(uint a_nIndex, vector3 a_v3Color = C_YELLOW);
	/*
	USAGE: Displays the MyOctant volume in the color specified
	ARGUMENTS:
	-vector3 a_v3Color = CYELLOW - > color of the volume to display
	OTPUT: ---
	*/
	void Display(vector3 a_v3Color = C_YELLOW);
	/*
	USAGE: Display the non empty leafs in the octree
	ARGUMENTS: - vector3 a v_3Colors = CYELLOW -> Color of the volume to display.
	OUTPUT: ----
	*/
	void DisplayLeafs(vector3 a_v3Color = C_YELLOW);
	/*
	USAGE: Clears the Entity list for each node
	ARGUMENTS: ----
	OUTPUT: ----
	*/
	void ClearEntityList(void);
	/*
	USAGE: allocates & smaller octants in the child pointers
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Subdivide(void);
	/*
	USAGE: returns the child specified in the index
	ARGUMENTS: uint a_nCHild -> Indez of the child (from 0  to 7)
	OUTPUT: My Octant object (child in index)
	*/
	MyOctant* GetChild(uint a_nChild);
	/*
	USAGE: returns the parent of the octant
	ARGUMENTS: ---
	OUTPUT: MyOctant object (parent)
	*/
	MyOctant* GetParent(void);
	/*
	USAGE: Ask the MYOctant if it does not contain any children (its a leaf)
	ARUGUMENTS: ---
	OUtPUT: IT contains no children
	*/
	bool IsLeaf(void);
	/*
	USAGE: Ask the MyOctant if it contains more than this many Bounding Objects
	ARGUMENTS: - uint a_nEntities -> Numebr of Entites to query
	OUTPUT: It contains at least this manu ENtities
	*/
	bool ContainsMoreThan(uint a_nEntities);
	/*
	USAGE: Deletes all children and the children of their children
	ARGUMENT: ----
	OUTPUT: ----
	*/
	void KillBranxhes(void);
	/*
	USAGE: create a tree using subdivisions, the max number of objects and levels
	ARGYMEENTS: -uint a_nMaxLevel = 3 -> Sets the maximum level of the tree while constructing it
	OUTPUT:  --- 
	*/
	void ConstructTree(uint a_nMaxLevel = 3);
	/*
	USAGE: Traverse the tree up to the leafs and set the objects in them to the index
	ARGUMENTS: ---
	OUTPUT:---
	*/
	void AssignIDtoEntity(void);
	/*
	USAGE: Gets the total number of octants in the worls
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	uint GETOctantCount(void);

private:
	/*
	USAGE: Deallocates member fields
	ARGUMENTS: ----
	OUTPUT: ----
	*/
	void Release(void);
	/*
	USAGE: Allocates memeber fields
	ARGUMETS: ---
	OUTPUT: ---
	*/
	void Init(void);
	/*
	USAGE: creates the list f all leafs of all leafs the contains objects
	ARGUMENTS: ----
	OUTPUT: ----
	*/
	void ConstructList(void);

};

}

#endif //_MYOCTANTCLASS_H_

