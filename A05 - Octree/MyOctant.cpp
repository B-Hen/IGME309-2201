#include "MyOctant.h"
using namespace Simplex;

//myOctant
uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdeaEntityCount = 5;
uint MyOctant::GETOctantCount(void) { return m_uOctantCount; };

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdeaEntityCount)
{
	Init();

	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdeaEntityCount = a_nIdeaEntityCount;
	m_uID = m_uOctantCount;

	m_pRoot = this;
	m_lChild.clear();

	std::vector<vector3> lMinMax; // this will hold all of the Min and Max vectors of the Bounding Object

	uint nObjects = m_pEntityMngr->GetEntityCount();
	for (uint i = 0; i < nObjects; i++)
	{
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();
		lMinMax.push_back(pRigidBody->GetMinGlobal());
		lMinMax.push_back(pRigidBody->GetMaxGlobal());
	}

	MyRigidBody* pRigidBody = new MyRigidBody(lMinMax);

	vector3 vHalfWidth = pRigidBody->GetHalfWidth();
	float fMax = vHalfWidth.x;

	for (int i = 1; i < 3; i++)
	{
		if (fMax < vHalfWidth[i])
			fMax = vHalfWidth[i];
	}

	vector3 v3Center = pRigidBody->GetCenterLocal();
	lMinMax.clear();
	SafeDelete(pRigidBody);

	m_fSize = fMax * 2.0f;
	m_v3Center = v3Center;
	m_v3Min = m_v3Center - (vector3(fMax));
	m_v3Max = m_v3Center + (vector3(fMax));

	m_uOctantCount++;
	ConstructTree(m_uMaxLevel);
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}

Simplex::MyOctant::MyOctant(MyOctant const& other)
{
	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;

	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}

MyOctant& Simplex::MyOctant::operator=(MyOctant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}

	return *this;
}

Simplex::MyOctant::~MyOctant(void)
{
	Release();
}

void Simplex::MyOctant::Swap(MyOctant& other)
{
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);
	for (uint i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}

float Simplex::MyOctant::GetSize(void)
{
	return m_fSize;
}

vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool Simplex::MyOctant::IsCollidiing(uint a_uRBIndex)
{
	uint nObjectCount = m_pEntityMngr->GetEntityCount();

	//if the index given is larger than the number of elements in the bounding object there is no collision

	if (a_uRBIndex >= nObjectCount)
		return false;

	//as the octant will never rotate or scale this collsions is as easy as an AABB
	//get all vectors in global space 
	MyEntity* pEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRigidBody = pEntity->GetRigidBody();
	vector3 v3MinO = pRigidBody->GetMinGlobal();
	vector3 v3MaxO= pRigidBody->GetMaxGlobal();

	//check x
	if (m_v3Max.x < v3MinO.x)
		return false;
	if (m_v3Min.x > v3MaxO.x)
		return false;

	//check y
	if (m_v3Max.y < v3MinO.y)
		return false;
	if (m_v3Min.y > v3MaxO.y)
		return false;

	//check z 
	if (m_v3Max.z < v3MinO.z)
		return false;
	if (m_v3Min.z > v3MaxO.z)
		return false;

	return true;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (m_uID == a_nIndex)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->Display(a_nIndex);
	}
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->Display(a_v3Color);
	}

	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	uint nLeafs = m_lChild.size();
	for (uint nChild = 0; nChild < nLeafs; nChild++)
	{
		m_lChild[nChild]->DisplayLeafs(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

void Simplex::MyOctant::ClearEntityList(void)
{
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ClearEntityList();
	}
}

void Simplex::MyOctant::Subdivide(void)
{
	//if this node has reach the maximum depth return without chnages
	if (m_uLevel >= m_uMaxLevel)
		return;

	//if this node has been already subdivided return without changes
	if (m_uChildren != 0)
		return;

	//as this will have children it will not be a leaf
	m_uChildren = 8;

	float fSize = m_fSize / 4.0f;
	float fSizeD = fSize * 2.0f;
	vector3 v3Center;

	//MyOctant 0 Bottom Left Back
	v3Center = m_v3Center;
	v3Center.x -= fSize;
	v3Center.y -= fSize;
	v3Center.z -= fSize;

	m_pChild[0] = new MyOctant(v3Center, fSizeD);

	//bottom right back
	v3Center.x += fSizeD;
	m_pChild[1] = new MyOctant(v3Center, fSizeD);

	//bottom right front
	v3Center.z += fSizeD;
	m_pChild[2] = new MyOctant(v3Center, fSizeD);

	//bottom left front
	v3Center.x -= fSizeD;
	m_pChild[3] = new MyOctant(v3Center, fSizeD);

	//top left
	v3Center.y += fSizeD;
	m_pChild[4] = new MyOctant(v3Center, fSizeD);

	//top left back
	v3Center.z -= fSizeD;
	m_pChild[5] = new MyOctant(v3Center, fSizeD);

	//top right back
	v3Center.x += fSizeD;
	m_pChild[6] = new MyOctant(v3Center, fSizeD);

	//top right font
	v3Center.z += fSizeD;
	m_pChild[7] = new MyOctant(v3Center, fSizeD);

	for (uint nIndex = 0; nIndex < 8; nIndex++)
	{
		m_pChild[nIndex]->m_pRoot = m_pRoot;
		m_pChild[nIndex]->m_pParent = this;
		m_pChild[nIndex]->m_uLevel = m_uLevel + 1;
		if (m_pChild[nIndex]->ContainsMoreThan(m_uIdeaEntityCount))
		{
			m_pChild[nIndex]->Subdivide();
		}
	}
}

MyOctant* Simplex::MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild > 7) return nullptr;
	return m_pChild[a_nChild];
}

MyOctant* Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	return m_uChildren == 0;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	uint nCount = 0;
	uint nObjectCount = m_pEntityMngr->GetEntityCount();
	for (uint n = 0; n < nObjectCount; n++)
	{
		if (IsCollidiing(n))
			nCount++;
		if (nCount > a_nEntities)
			return true;
	}

	return false;
}

void Simplex::MyOctant::KillBranxhes(void)
{
	//tranvers tthe whole tree until it reaches a node with no children nd once it returns from it to its parent it till kill all of its children
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->KillBranxhes();
		delete m_pChild[nIndex];
		m_pChild[nIndex] = nullptr;
	}

	m_uChildren = 0;
}

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//If this method is tried to be applied to something else
    //other than the root, don't
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = a_nMaxLevel;

	m_uOctantCount = 1;

	//ClearEntityList();
	m_EntityList.clear();

	//clear the tree
	KillBranxhes();
	m_lChild.clear();

	//If the base tree
	if (ContainsMoreThan(m_uIdeaEntityCount))
	{
		Subdivide();
	}

	//Add octant ID to Entities
	AssignIDtoEntity();

	//constuct the list of objects
	ConstructList();
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	//tranvres until you reach leaf
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->AssignIDtoEntity();
	}
	if (m_uChildren == 0) // if this is a leaf
	{
		uint nEnities = m_pEntityMngr->GetEntityCount();
		for (uint nIndex = 0; nIndex < nEnities; nIndex++)
		{
			if (IsCollidiing(nIndex))
			{
				m_EntityList.push_back(nIndex);
				m_pEntityMngr->AddDimension(nIndex, m_uID);
			}
		}
	}
}

void Simplex::MyOctant::Release(void)
{
	//special release will only happen for the root
	if (m_uLevel == 0)
	{
		KillBranxhes();
	}

	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}

void Simplex::MyOctant::Init(void)
{
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_uID = m_uOctantCount;
	m_uLevel = 0;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (uint n = 0; n < 8; n++)
	{
		m_pChild[n] = nullptr;
	}
}

void Simplex::MyOctant::ConstructList(void)
{
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ConstructList();
	}

	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}
