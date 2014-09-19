#pragma once
#define NROFTRIANGLES 12
#define NROFLIGHTS 10
#define CUBESIZE 16
#include "stdafx.h"

struct Ray
{
	XMFLOAT4 origin;
	XMFLOAT4 direction;
};

struct HitData
{
	XMFLOAT4	pos;
	XMFLOAT4	normal;
	XMFLOAT4	color;
	float		distance;
	int			ID;
	int			materialID;
	int			pad;
};

struct Light
{
	XMFLOAT4 pos;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	float range;
	XMFLOAT3 pad;
};

struct Sphere
{
	XMFLOAT4 center;
	XMFLOAT4 color;
	float radius;
	int		ID;
	XMFLOAT2 pad;
};

struct Triangle
{
	XMFLOAT4	pos0;
	XMFLOAT4	pos1;
	XMFLOAT4	pos2;
	XMFLOAT2	textureCoordinate0;
	XMFLOAT2	textureCoordinate1;
	XMFLOAT2	textureCoordinate2;
	int			ID;
	float		pad;
	XMFLOAT4	color;
	XMFLOAT4	normal;
};

//Constant Buffers
struct PrimaryConstBuffer
{
	XMFLOAT4	cameraPos;
	XMFLOAT4X4	IP;
	XMFLOAT4X4	IV;
};

struct IntersectionConstBuffer
{
	Sphere sphere;
	Triangle triangles[NROFTRIANGLES];
	int nrOfFaces;
	XMFLOAT3 pad;
};

struct ColorConstBuffer
{
	Sphere sphere;
	Triangle triangles[NROFTRIANGLES];
	Light lightList[NROFLIGHTS];
	int nrOfFaces;
	XMFLOAT3 pad;
};

struct FirstPassConstBuffer
{
	//bool firstPass;
	XMFLOAT4 firstPass;
};

//old Constant buffer
struct ConstBuffer
{
	XMFLOAT4 cameraPos;
	XMFLOAT4X4 IP;
	XMFLOAT4X4 IV;
	Sphere sphere;
	Sphere spherel0;
	Triangle triangles[NROFTRIANGLES];
	Light lightList[NROFLIGHTS];
	int nrOfFaces;
	XMFLOAT3 pad;
};
