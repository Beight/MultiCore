#include "RayIntersect.fx"

struct Light
{
	float3 pos;
	float3 dir;
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 att;
	float range;
};


float3 PointLight(HitData hd, Light L, Ray r)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	//The vector from surface to the light
	float3 lightVec = L.pos - hd.pos;

	//the distance deom surface to light
	float d = length(lightVec);

	if(d > L.range)
		return float3(0.0f, 0.0f, 0.0f);

	//Normalize light vector
	lightVec /= d;

	//Add ambient light term
	litColor += hd.color * L.ambient; //hd.color = diffuse

	float diffuseFactor = dot(lightVec, hd.normal);
	[branch]
	if(diffuseFactor > 0.0f)
	{
		float specPower		= max(hd.color.a, 1.0f);
		float3 toEye		= normalize(r.origin - hd.pos);
		float3 R			= reflect(-lightVec, hd.normal);
		float specFactor	= pow(max(dot(R, toEye), 0.0f), specPower);

		litColor += diffuseFactor * hd.color * L.diffuse; //hd.color = diffuse
		litColor += specFactor * hd.color *	L.spec; //hd.color = spec
	}

	return litColor / dot(L.att, float3(1.0f, d, d*d));
}