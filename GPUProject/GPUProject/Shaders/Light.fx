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
	float3 pad;
};


float3 PointLight(HitData hd, Light L, Ray r)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	//The vector from surface to the light
	float3 lightVec = L.pos.xyz - hd.pos.xyz;
	float3 paddy = L.pad;
	//the distance deom surface to light
	float d = length(lightVec);

	if(d > L.range)
		return float3(0.0f, 0.0f, 0.0f);

	//Normalize light vector
	lightVec /= d;

	//Add ambient light term
	litColor += hd.color.xyz * L.ambient.xyz; //hd.color = diffuse

	float diffuseFactor = dot(lightVec, hd.normal.xyz);
	
	[branch]
	if(diffuseFactor > 0.0f)
	{
		float specPower		= max(0.5, 1.0f);
		float3 toEye		= normalize(r.origin.xyz - hd.pos.xyz);
		float3 R			= reflect(-lightVec, hd.normal.xyz);
		float specFactor	= pow(max(dot(R, toEye), 0.0f), 50);

		litColor += diffuseFactor * hd.color.xyz * L.diffuse.xyz; //hd.color = diffuse
		litColor += specFactor *  hd.color.xyz *	L.spec.xyz; //hd.color = spec
	}

	return litColor / dot(L.att.xyz, float3(1.0f, d, d*d));
}

float3 PointLightR(HitData hd, Light L, Ray r)
{
	//return float4(1, 0, 1, 1) * 0.1f;
	float3 paddy = L.pad;
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	//The vector from surface to the light
	float3 lightVec = L.pos.xyz - hd.pos.xyz;
	float lightintensity;
	float3 lightDir;
	float3 reflection;
	float4 specular;
	//the distance deom surface to light
	float d = length(lightVec);
	float fade;
	if(d > L.range)
		return float3(0.0f, 0.0f, 0.0f);
	fade = 1 - (d/ L.range);
	//Normalize light vector
	lightVec /= d;

	//Add ambient light term
	litColor = L.ambient.xyz;

	//lightVec = float3(0, 0, 0);
	//hd.normal = float4(0, 0, 0, 1);
	lightintensity = saturate(dot(hd.normal.xyz, lightVec));
	litColor += L.diffuse.xyz * lightintensity;
	lightDir = -lightVec;
	if(lightintensity > 0.0f)
	{
	/*	litColor += L.diffuse.xyz * lightintensity;
		litColor	= saturate(litColor);
		reflection	= normalize(2 * lightintensity * hd.normal.xyz - lightDir);
		float3 toEye= normalize(r.origin.xyz - hd.pos.xyz);
		specular	= pow(saturate(dot(reflection, toEye)), 255);*/

		float shininess = 32;
		float3 viewDir = normalize(hd.pos.xyz - r.origin.xyz);
		float3 ref = reflect(-lightDir, normalize(hd.normal.xyz));
		float specFac = pow(max(dot(ref, viewDir), 0.0f), shininess);
		litColor += float3(1.0f, 1.0f, 1.0f) * specFac;
	}
	litColor = litColor * hd.color.xyz;
	//litColor = saturate(litColor + specular.xyz);

	return litColor*fade;
}