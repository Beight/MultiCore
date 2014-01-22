//--------------------------------------------------------------------------------------
// BasicCompute.fx
// Direct3D 11 Shader Model 5.0 Demo
//--------------------------------------------------------------------------------------
#include "Light.fx"
static const float WIDTH = 800.0f;
static const float HEIGHT= 800.0f;
#define NROFTRIANGLES 10
#define NROFLIGHTS 10

RWTexture2D<float4> output : register(u0);
StructuredBuffer<MeshTriangle> input : register(t0);
Texture2D meshTexture : register(t1);
StructuredBuffer<Material> material : register (t2);



float3 LightStage(HitData hd, Ray r);

cbuffer ConstBuffer
{
	float4 cameraPos;
	float4x4 IP;
	float4x4 IV;
	Sphere sphere;
	Sphere spherel0;
	Triangle triangles[NROFTRIANGLES];
	Light lightList[NROFLIGHTS];
	int nrOfFaces;
};

[numthreads(32, 32, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
	///////////////////////////////////////////////
	//Primary rays stage
	///////////////////////////////////////////////
	HitData hd;
	float hit;
	hd.distance = -1.0;
	hd.ID = -1;
	Ray r;
	float norm_X, norm_Y;
	norm_X = ((threadID.x / WIDTH)*2) - 1.0f;
	norm_Y = ((1.0f-(threadID.y / HEIGHT))*2) - 1.0f;

	float4 rayDir = float4(norm_X, norm_Y, 1, 1);

	rayDir = mul(rayDir, IP);

	rayDir = rayDir / rayDir.w;

	rayDir = mul(rayDir, IV);

	rayDir = rayDir - cameraPos;
	rayDir = normalize(rayDir);
	r.origin = cameraPos;
	r.direction = rayDir;

	///////////////////////////////////////////////
	//Intersection stage
	///////////////////////////////////////////////

	float3 finalColor = float3(0.0f, 0.0f, 0.0f);

	float2 paddy = sphere.pad;
	// ## SPHERE ## //
	hit = RaySphereIntersect(r, sphere, hd.distance);
	if(hit > -1.0f)
	{
		hd.pos = r.origin + r.direction * hit;
		hd.normal = normalize(hd.pos - sphere.center);
		hd.color = sphere.color;
		hd.distance = hit;
		hd.ID = sphere.ID;
		hd.materialID = -1;
	}
	hit = RaySphereIntersect(r, spherel0, hd.distance);
	if(hit > -1.0f)
	{
		hd.pos = r.origin + r.direction * hit;
		hd.normal = normalize(hd.pos - spherel0.center);
		hd.color = spherel0.color;
		hd.distance = hit;
		hd.ID = spherel0.ID;
		hd.materialID = -1;
	}

	//					
	// ## CUBE ## // 
	for(int i = 0; i < NROFTRIANGLES; i++)
	{
			//padding
			float3 paddy = triangles[i].pad;
			hit = RayTriangleIntersect(r, triangles[i], hd.distance);
			if(hit > -1.0f)
			{
				hd.pos = r.origin + r.direction * hit.x;
				hd.normal = triangles[i].normal;
				hd.color = triangles[i].color;					
				hd.ID = triangles[i].ID;
				hd.distance = hit;
				hd.materialID = -1;
			}
	}

	
	// ## Single Mesh Triangle ## //
	//if(hd.ID != 72)
	//{
	//	float3 temp;
	//	temp = RayTriangleIntersects(r, input[0], hd.distance);
	//	hit = temp.x;
	//	if(hit > -1.0f)
	//	{
	//		hd.pos = r.origin + r.direction * hit;
	//		float4 e1 = input[0].pos1 - input[0].pos0;
	//		float4 e2 = input[0].pos2 - input[0].pos0;
	//		float3 temp = cross(e1.xyz, e2.xyz);
	//		hd.normal = normalize(float4(temp, 1.0f));//normalize(input[0].normal);
	//		hd.color = float4(0.f, 0.f, 1.f, 0.f);//meshTexture[temp.yz*256.0f];
	//		hd.ID = input[0].ID;
	//		hd.distance = hit;
	//	}
	//}
	

	// ## MESH ## // ptriangle = (1-u-v)A+uB+vC
	for(int j = 0; j < nrOfFaces; j++)
	{
		//padding
		float paddy = input[j].pad;
		float3 temp;
		temp = RayTriangleIntersects(r, input[j], hd.distance);
		hit = temp.x;
		if(hit > -1.0f)
		{
			hd.pos = r.origin + r.direction * hit;
			hd.normal = normalize(input[j].normal);
			//hd.color = float4(0.f, 0.f, 1.f, 0.f);
			hd.color = meshTexture[temp.yz*512.f];
			hd.ID = input[j].ID;
			hd.distance = hit;
			hd.materialID = 1;
		}
	}
	if(hd.distance >  0.0f)
	{

		///////////////////////////////////////////////
		//First Light Ray
		///////////////////////////////////////////////
		finalColor += LightStage(hd, r);
		///////////////////////////////////////////////
		//Bounce Rays
		///////////////////////////////////////////////
		Ray bounceRay;
		bounceRay = r;
		HitData bounceHit;
		bounceHit.distance = -1.0f;
		bounceHit.pos = hd.pos;
		bounceHit.color = hd.color;
		bounceHit.normal = hd.normal;
		bounceHit.ID = hd.ID;
		int tempID = -1;
		///////////////////////////////////////////////
		//Number of Bounces
		///////////////////////////////////////////////
		for(int j = 0; j < 0; j++)
		{
			float bHit = -1.0f;
			bounceHit.distance = -1.0f;
			float4 bounceDir = reflect(bounceRay.direction, bounceHit.normal);
			bounceDir = normalize(bounceDir);
			bounceRay.origin = bounceHit.pos;
			bounceRay.direction = bounceDir;
			// ## SPHERE ## //
			if(bounceHit.ID != -2)
			{
				bHit = RaySphereIntersect(bounceRay, sphere, bounceHit.distance);
				if(bHit > -1.0f)
				{
					bounceHit.pos = bounceRay.origin + bounceRay.direction * bHit;
					bounceHit.normal = normalize(bounceHit.pos - sphere.center);
					bounceHit.color = sphere.color;
					bounceHit.distance = bHit;
					tempID = sphere.ID;
					bounceHit.materialID = -1;
				}
			}
			// ## CUBE ## //
			for(int i = 0; i < NROFTRIANGLES; i++)
			{
				float3 paddy = triangles[i].pad;
				if(bounceHit.ID != i)
				{
					bHit = RayTriangleIntersect(bounceRay, triangles[i], bounceHit.distance);
					if(bHit > -1.0f)
					{
						bounceHit.pos = bounceRay.origin + bounceRay.direction * bHit;
						bounceHit.normal = triangles[i].normal;
						bounceHit.color = triangles[i].color;
						tempID = triangles[i].ID;
						hd.distance = bHit;
						bounceHit.materialID = -1;
					}
				}
			}

			// ## Single Mesh Triangle ## //
			//if(bounceHit.ID != input[0].ID)
			//{
			//	float3 temp;
			//	temp = RayTriangleIntersects(bounceRay, input[0], bounceHit.distance);
			//	hit = temp.x;
			//	if(hit > -1.0f)
			//	{
			//		bounceHit.pos = r.origin + r.direction * hit; ///////////////////////////////////////////////###### OBS!!! r
			//		float4 e1 = input[0].pos1 - input[0].pos0;
			//		float4 e2 = input[0].pos2 - input[0].pos0;
			//		float3 temp = cross(e1.xyz, e2.xyz);
			//		bounceHit.normal = normalize(float4(temp, 1.0f)); //normalize(input[0].normal);
			//		bounceHit.color = float4(0.f, 0.f, 1.f, 0.f);//meshTexture[temp.yz*256.0f];
			//		bounceHit.ID = input[0].ID;
			//		bounceHit.distance = hit;
			//	}
			//}
			// ## MESH ## //
			for(int j = 0; j < nrOfFaces; j++)
			{
				if(bounceHit.ID != input[j].ID)
				{
					//padding
					float paddy = input[j].pad;
					float3 temp = RayTriangleIntersects(bounceRay, input[j], bounceHit.distance);
					if(temp.x > -1.0f)
					{
						bounceHit.pos = bounceRay.origin + bounceRay.direction * hit;
						bounceHit.normal = normalize(input[j].normal);
						//bounceHit.color = float4(0.f, 0.f, 1.f, 0.f);
						bounceHit.color = meshTexture[temp.yz*512.f];
						tempID = input[j].ID;
						bounceHit.distance = temp.x;
						bounceHit.materialID = 1;
					}
				}
			}
			if(tempID != -1)
				bounceHit.ID = tempID;
			///////////////////////////////////////////////
			//Bounce Light
			///////////////////////////////////////////////
			finalColor += LightStage(bounceHit, bounceRay) * 1.0f;
		}
	}

	output[threadID.xy] = /*hd.normal;//*/ float4(finalColor, 1.0f);
}

float3 LightStage(HitData hd, Ray r)
{
		
		float3 final = float3(0.f,0.f,0.f);
		float3 paddy = lightList[0].pad;

		/// ## NUMBER OF LIGHTS ## //
		for(int i = 0; i < 1; i++)
		{	
			float3 color = float3(0.0f, 0.0f, 0.0f);
			Ray lightRay;
			float hit = -1.0f;
			HitData lightHit;
			lightHit.distance = -1.0f;
			lightRay.origin = hd.pos;
			lightRay.direction = normalize(lightList[i].pos - hd.pos);
			float lightLength = length(lightList[i].pos.xyz - hd.pos.xyz);
			// ## SPHERE ## //
			if(hd.ID != -2)
			{
				hit = RaySphereIntersect(lightRay, sphere, hd.distance);
				if(hit > -1.0f)
				{
					lightHit.distance = hit;
				}
			}
			// ## CUBE ## //
			for(int j = 0; j < NROFTRIANGLES; j++)
			{
				if(hd.ID != triangles[j].ID)
				{
					hit = RayTriangleIntersect(r, triangles[j], hd.distance);
					if(hit > -1.0f)
					{
						lightHit.distance = hit;
					}
				}
			}


			// ## Single Mesh Triangle ## //
			//if(hd.ID != input[0].ID)
			//{
			//	float3 temp;
			//	temp = RayTriangleIntersects(lightRay, input[0], hd.distance);
			//	hit = temp.x;
			//	if(hit > -1.0f)
			//	{
			//		lightHit.pos = lightRay.origin + lightRay.direction * hit; ///////////////////////////////////////////////###### OBS!!! r
			//		float4 e1 = input[0].pos1 - input[0].pos0;
			//		float4 e2 = input[0].pos2 - input[0].pos0;
			//		float3 temp = cross(e1.xyz, e2.xyz);
			//		lightHit.normal = normalize(float4(temp, 1.0f));//input[0].normal);
			//		lightHit.color = float4(0.f, 0.f, 1.f, 0.f);//meshTexture[temp.yz*256.0f];
			//		lightHit.ID = input[0].ID;
			//		lightHit.distance = hit;
			//	}
			//}

			// ## MESH ## //
			for(int k = 0; k < nrOfFaces; k++)
			{
				if(hd.ID != input[k].ID)
				{
					float3 temp = RayTriangleIntersects(lightRay, input[k], hd.distance);
					if(temp.x > -1.0f)
					{
						lightHit.distance = temp.x;
					}
				}
			}
			if(lightHit.distance > 0.0001f && lightLength > lightHit.distance)
			{
				color = (PointLightR(hd.pos, hd.normal, hd.color, hd.materialID, lightList[i], r, material[0])*0.5f);
				
			}	
			else 
				color = PointLightR(hd.pos, hd.normal, hd.color, hd.materialID, lightList[i], r, material[0]);
		

			final += color;
		}
		return final;
}