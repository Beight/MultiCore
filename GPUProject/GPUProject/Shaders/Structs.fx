//--------------------------------------------------------------------------------------
// Structs.fx
// Direct3D 11 Shader Model 5.0
//--------------------------------------------------------------------------------------
#ifndef STRUCTS
#define STRUCTS

#include "..\Constants.h"



struct Ray
{
	float4 origin;
	float4 direction;
};

struct Sphere
{
	float4 center;
	float4 color;
	float radius;
	int		ID;
	float2 pad;
};

struct Triangle
{
	float4	pos0;
	float4	pos1;
	float4	pos2;
	float2	textureCoordinate0;
	float2	textureCoordinate1;
	float2	textureCoordinate2;
	int		ID;
	float	pad;
	float4	color;
	float4	normal;
	
};

struct Material
{
	int		illum;
	float	ni;
	float2	pad;
	float4	kd;
	float4	ka;
	float4	tf;
};

struct HitData
{
	float4	pos;
	float4	normal;
	float4	color;
	float	distance;
	int		ID;
	int		materialID;
	float	rayPower;
};

#endif