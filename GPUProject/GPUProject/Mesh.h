#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <DirectXMath.h>

using namespace DirectX;

struct MeshTriangle
{
	XMVECTOR	pos0;
	XMVECTOR	pos1;
	XMVECTOR	pos2;
	XMFLOAT2	textureCoordinate0;
	XMFLOAT2	textureCoordinate1;
	XMFLOAT2	textureCoordinate2;
	int			ID;
	float		pad;
	XMVECTOR	normal;
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
	std::string map_Ka;
	std::string map_Kd;

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


struct Material2
{
	int	illum;
	float   ni;
	XMFLOAT2  pad;
	XMVECTOR kd;
	XMVECTOR ka;
	XMVECTOR tf;

	Material2()
	{
		pad = XMFLOAT2(0.f, 0.f);
	}
};


class Mesh
{
public:
	Mesh();
	~Mesh();
	void init();
	void loadObj(char* textFile);
	std::vector<MeshTriangle>* getTriangles();
	Material* getMaterial();
	int getFaces();
	MeshTriangle* getTriangles2();
	Material2 getMaterial2();
private:
	int m_nrOfFaces;
	void loadMaterial(std::string filename);
	Material m_material;
	Material2 m_material2;
	std::vector<MeshTriangle> m_meshTriangles;

};

#endif //