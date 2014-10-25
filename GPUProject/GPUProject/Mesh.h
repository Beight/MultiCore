#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <DirectXMath.h>
#include "Structs.h"

using namespace DirectX;

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
	XMFLOAT4 kd;
	XMFLOAT4 ka;
	XMFLOAT4 tf;

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
	Material* getMaterial();
	int getFaces();
	Triangle* getTriangles();
	Material2 getMaterial2();
private:
	int m_nrOfFaces;
	void loadMaterial(std::string filename);
	Material m_material;
	Material2 m_material2;
	std::vector<Triangle> m_meshTriangles;

};
