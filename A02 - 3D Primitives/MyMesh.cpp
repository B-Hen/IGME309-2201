#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	vector3 center = vector3(0, 0, -a_fHeight/2);
	//get the angle for the base 
	float angle = (2 * PI) / a_nSubdivisions;

	//save the point before
	vector3 pointBefore = vector3(a_fRadius * cosf(-angle), a_fRadius * sinf(-angle), -a_fHeight / 2);

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//get the end points to complete a new triangle
		float pointX = a_fRadius * cosf(angle * i);
		float pointY = a_fRadius * sinf(angle * i);

		//make the triangle for the base
		AddTri(pointBefore, center, vector3(pointX, pointY, -a_fHeight / 2));

		//make triangle for the side
		AddTri(pointBefore, vector3(pointX, pointY, -a_fHeight / 2), vector3(0, 0, a_fHeight/2));

		//change the point before to the new point
		pointBefore = vector3(pointX, pointY, -a_fHeight / 2);
	}


	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//center vector for the base 
	vector3 center = vector3(0, 0, -a_fHeight / 2);
	vector3 centerTop = vector3(0, 0, a_fHeight/2);

	//get the angle of the subdivisions
	float angle = (2 * PI) / a_nSubdivisions;
	vector3 pointBefore = vector3(a_fRadius * cosf(-angle), a_fRadius * sinf(-angle), -a_fHeight / 2);

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//make the x and y points
		float pointX = a_fRadius * cosf(angle * i);
		float pointY = a_fRadius * sinf(angle * i);

		//make the base of the cylinder
		AddTri(pointBefore, center, vector3(pointX, pointY, -a_fHeight / 2));

		//make the sides of the cylinder
		AddQuad(pointBefore, vector3(pointX, pointY, -a_fHeight / 2), vector3(pointBefore.x, pointBefore.y, a_fHeight/2), vector3(pointX, pointY, a_fHeight/2));

		//make the top of the cylinder
		AddTri(vector3(pointBefore.x, pointBefore.y, a_fHeight/2), vector3(pointX, pointY, a_fHeight/2), centerTop);

		//make the pointBefore equal to the point found here
		pointBefore =  vector3(pointX, pointY, -a_fHeight / 2);
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//center vector for the base 
	vector3 center = vector3(0, 0, -a_fHeight / 2);
	vector3 centerTop = vector3(0, 0, a_fHeight/2);

	//get the angle of the subdivisions
	float angle = (2 * PI) / a_nSubdivisions;
	vector3 pointBefore = vector3(a_fOuterRadius * cosf(-angle), a_fOuterRadius * sinf(-angle), -a_fHeight / 2);
	vector3 pointBefore2 = vector3(a_fInnerRadius * cosf(-angle), a_fInnerRadius * sinf(-angle), -a_fHeight / 2);

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//make the x and y points
		float pointX = a_fOuterRadius * cosf(angle * i);
		float pointY = a_fOuterRadius * sinf(angle * i);

		float pointXInner = a_fInnerRadius * cosf(angle * i);
		float pointYInner = a_fInnerRadius * sinf(angle * i);

		//make the base of the cylinder and then add it so the center is empty
		AddTri(pointBefore, pointBefore2, vector3(pointX, pointY, -a_fHeight / 2));
		AddTri(vector3(pointXInner, pointYInner, -a_fHeight / 2), vector3(pointX, pointY, -a_fHeight / 2), pointBefore2);

		//make the sides of the cylinder
		AddQuad(pointBefore, vector3(pointX, pointY, -a_fHeight / 2), vector3(pointBefore.x, pointBefore.y, a_fHeight/2), vector3(pointX, pointY, a_fHeight/2));

		//make the inside sides of the cylinder 
		AddQuad(vector3(pointXInner, pointYInner, -a_fHeight / 2), pointBefore2, vector3(pointXInner, pointYInner, a_fHeight/2), vector3(pointBefore2.x, pointBefore2.y, a_fHeight/2));

		//make the top of the cylinder and then add it so the center is empty 
		AddTri(vector3(pointBefore.x, pointBefore.y, a_fHeight/2), vector3(pointX, pointY, a_fHeight/2), vector3(pointBefore2.x, pointBefore2.y, a_fHeight/2));
		AddTri(vector3(pointXInner, pointYInner, a_fHeight/2), vector3(pointBefore2.x, pointBefore2.y, a_fHeight/2), vector3(pointX, pointY, a_fHeight/2));

		//make the pointBefore equal to the point found here
		pointBefore = vector3(pointX, pointY, -a_fHeight / 2);
		pointBefore2 = vector3(pointXInner, pointYInner, -a_fHeight / 2);
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	//vectors to hold points 
	vector3 pointUno = vector3(0, 0, 0);
	vector3 pointZwei = vector3(0, 0, 0);
	vector3 pointAlteUno = vector3(0, 0, 0);
	vector3 pointAlteZwei = vector3(0, 0, 0);

	//loop through for the vertical points
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		//loop through for the horzantiluy points 
		for (int j = 0; j <= a_nSubdivisionsB; j++)
		{
			//k will reset when j moves on to its next iteration the k is swtiching between 0 and 1 so only save the first 2 points
			for (int k = 1; k >= 0; k--)
			{
				float s = (i + k) % a_nSubdivisionsA + 0.5;
				float t = j % a_nSubdivisionsB;
	
				float x = (a_fOuterRadius + a_fInnerRadius * cosf(s * (2 * PI) / a_nSubdivisionsA)) * cosf(t * (2 * PI) / a_nSubdivisionsB);
				float y = (a_fOuterRadius + a_fInnerRadius * cosf(s * (2 * PI) / a_nSubdivisionsA)) * sinf(t * (2 * PI) / a_nSubdivisionsB);
				float z = a_fInnerRadius * sinf(s * (2 * PI) / a_nSubdivisionsA);

				if (k == 1)
				{
					pointUno = vector3(x, y, z);
				}
				if (k == 0)
				{
					pointZwei = vector3(x, y, z);
				}

				//if its more than 0 draw the tures 
				if (j != 0)
				{
					AddTri(pointAlteUno, pointAlteZwei, pointUno);
					AddTri(pointZwei, pointUno, pointAlteZwei);
				}

				//make the old points equal to the new points 
				pointAlteUno = pointUno;
				pointAlteZwei = pointZwei;
			}
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	//a list to hold the vertices
	std::vector<vector3> vertices = std::vector<vector3>();
	a_nSubdivisions *= 3;

	//floats to hold the vetecies xy is the change in poisition
	float x, y, z, xy;
	
	//float to hold the place around the circle
	float stepHorizontal = (2 * PI) / a_nSubdivisions;
	float stepVertical = PI / a_nSubdivisions;

	//floats to hold the angles for each circle
	float angle90 = 0;
	float angle360 = 0;

	//loop through 0 to 360  horzonital
	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		//get the angle from 0 to 360
		angle360 = (PI / 2) - i * stepVertical;
		xy = a_fRadius * cosf(angle360);
		z = a_fRadius * sinf(angle360);

		//loop through to get the points from 90 to -90
		for (int j = 0; j <= a_nSubdivisions; j++)
		{
			//get the angle for the vertically
			angle90 = j * stepHorizontal;

			x = xy * cosf(angle90);
			y = xy * sinf(angle90);

			vertices.push_back(vector3(x, y, z));
		}
	}

	//floats to hold the current vertices
	int vertex1 = 0;
	int vertex2 = 0;
	
	//now that all the point are in a list draw it out 
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//get the first vertices and then the next one in the list
		vertex1 = i * (a_nSubdivisions + 1);
		vertex2 = vertex1 + (a_nSubdivisions + 1);
	
		//loop through again to connect and draw
		for (int j = 0; j < a_nSubdivisions; j++)
		{
			AddTri(vertices[vertex1 + 1], vertices[vertex2], vertices[vertex1]);
			AddTri(vertices[vertex2 + 1], vertices[vertex2], vertices[vertex1 + 1]);

			//move on to a new point in the list
			vertex1++;
			vertex2++;
		}
	}

    //Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}