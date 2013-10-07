#include "RayIntersect.fx"

struct Light
{
	float4 pos;
	float4 dir;
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float4 att;
	float range;
};


float3 PointLight(HitData hd, Light L, Ray r)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	//The vector from surface to the light
	float3 lightVec = L.pos.xyz - hd.pos.xyz;

	//the distance deom surface to light
	float d = length(lightVec);

	//if(d > L.range)
		//return float3(0.0f, 0.0f, 0.0f);

	//Normalize light vector
	lightVec /= d;

	//Add ambient light term
	litColor += hd.color.xyz * L.ambient.xyz; //hd.color = diffuse

	float diffuseFactor = dot(lightVec, hd.normal.xyz);
	
	[branch]
	if(diffuseFactor > 0.0f)
	{
		float specPower		= max(hd.color.a, 1.0f);
		float3 toEye		= normalize(r.origin.xyz - hd.pos.xyz);
		float3 R			= reflect(-lightVec, hd.normal.xyz);
		float specFactor	= pow(max(dot(R, toEye), 0.0f), 5);

		litColor += diffuseFactor * hd.color.xyz * L.diffuse.xyz; //hd.color = diffuse
		//litColor += specFactor * float3(1,1,1);// * hd.color.xyz *	L.spec.xyz; //hd.color = spec
	}

	return litColor / dot(L.att.xyz, float3(1.0f, d, d*d));
}