#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

struct MeshTriangle
{
	XMVECTOR	pos0;
	XMVECTOR	pos1;
	XMVECTOR	pos2;
	XMFLOAT2	textureCoordinate0;
	XMFLOAT2	textureCoordinate1;
	XMFLOAT2	textureCoordinate2;
	XMVECTOR	normal;
	int			ID;
	XMFLOAT3	pad;
};

struct Material
{
	float Ns;
	float Ni;
	float d;
	float Tr;
	XMFLOAT3 Tf;
	int illum;
	XMFLOAT3 Ka;
	XMFLOAT3 Kd;
	XMFLOAT3 Ks;
	XMFLOAT3 Ke;
	string map_Ka;
	string map_Kd;

	Material()
	{
		Ns = 0.0f;
		Ni = 0.0f;
		d = 0.0f;
		Tr = 0.0f;
		Tf = XMFLOAT3(0.0f, 0.0f, 0.0f);
		illum = 0;
		Ka = XMFLOAT3(0.0f, 0.0f, 0.0f);
		Kd = XMFLOAT3(0.0f, 0.0f, 0.0f);
		Ks = XMFLOAT3(0.0f, 0.0f, 0.0f);
		Ke = XMFLOAT3(0.0f, 0.0f, 0.0f);
		map_Ka = "";
		map_Kd = "";
	}
};

class Mesh
{
public:
	Mesh();
	~Mesh();
	void init();
	void loadObj(char* textFile);
	vector<MeshTriangle>* getTriangles();
	Material* getMaterial();
	int getFaces();
private:
	int m_nrOfFaces;
	void loadMaterial(string filename);
	Material m_material;
	
	vector<MeshTriangle> m_meshTriangles;

};

#endif //