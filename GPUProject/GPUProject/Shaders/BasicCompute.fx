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



float3 LightStage(HitData hd, Ray r);

cbuffer ConstBuffer
{
	float4 cameraPos;
	float4x4 IP;
	float4x4 IV;
	Sphere sphere;
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
	}
	//					
	// ## CUBE ## // 
	for(int i = 0; i < NROFTRIANGLES; i++)
	{
			float3 paddy = triangles[i].pad;
			hit = RayTriangleIntersect(r, triangles[i], hd.distance);
			if(hit > -1.0f)
			{
				hd.pos = r.origin + r.direction * hit.x;
				float4 e1 = triangles[i].pos1 - triangles[i].pos0;
				float4 e2 = triangles[i].pos2 - triangles[i].pos0;
				float3 temp = cross(e1.xyz, e2.xyz);
				hd.normal = normalize(float4(temp, 1.0f));
				hd.color = triangles[i].color;
				hd.ID = triangles[i].ID;
				hd.distance = hit;
			}
	}

	
	// ## Single Mesh Triangle ## //
	if(hd.ID != 72)
	{
		float3 temp;
		temp = RayTriangleIntersects(r, input[0], hd.distance);
		hit = temp.x;
		if(hit > -1.0f)
		{
			hd.pos = r.origin + r.direction * hit;
			/*float4 e1 = input[j].pos1 - input[j].pos0;
			float4 e2 = input[j].pos2 - input[j].pos0;
			float3 temp = cross(e1.xyz, e2.xyz);*/
			hd.normal = normalize(input[0].normal);
			hd.color = float4(0.f, 0.f, 1.f, 0.f);//meshTexture[temp.yz*256.0f];
			hd.ID = input[0].ID;
			hd.distance = hit;
		}
	}
	

	// ## MESH ## // ptriangle = (1-u-v)A+uB+vC
	//for(int j = 0; j < nrOfFaces; j++)
	//{
	//		float3 temp;
	//		temp = RayTriangleIntersects(r, input[j], hd.distance);
	//		hit = temp.x;
	//		if(hit > -1.0f)
	//		{
	//			hd.pos = r.origin + r.direction * hit;
	//			float4 e1 = input[j].pos1 - input[j].pos0;
	//			float4 e2 = input[j].pos2 - input[j].pos0;
	//			float3 temp = cross(e1.xyz, e2.xyz);
	//			hd.normal = normalize(float4(temp, 1.0f));
	//			hd.color = meshTexture[temp.yz*256.0f];
	//			hd.ID = input[j].ID;
	//			hd.distance = hit;
	//		}
	//}
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
						float4 e1 = triangles[i].pos1 - triangles[i].pos0;
						float4 e2 = triangles[i].pos2 - triangles[i].pos0;
						float3 temp = cross(e1.xyz, e2.xyz);
						bounceHit.normal = normalize(float4(temp, 1.0f));
						bounceHit.color = triangles[i].color;
						tempID = triangles[i].ID;
						bounceHit.distance = bHit;
					}
				}
			}

			// ## Single Mesh Triangle ## //
			if(bounceHit.ID != input[0].ID)
			{
				float3 temp;
				temp = RayTriangleIntersects(bounceRay, input[0], bounceHit.distance);
				hit = temp.x;
				if(hit > -1.0f)
				{
					bounceHit.pos = bounceRay.origin + bounceRay.direction * hit; ///////////////////////////////////////////////###### OBS!!! r
					/*float4 e1 = input[j].pos1 - input[j].pos0;
					float4 e2 = input[j].pos2 - input[j].pos0;
					float3 temp = cross(e1.xyz, e2.xyz);*/
					bounceHit.normal = normalize(input[0].normal);
					bounceHit.color = float4(0.f, 0.f, 1.f, 0.f);//meshTexture[temp.yz*256.0f];
					bounceHit.ID = input[0].ID;
					bounceHit.distance = hit;
				}
			}
			// ## MESH ## //
			//for(int j = 0; j < nrOfFaces; j++)
			//{
			//		float3 temp;
			//		temp = RayTriangleIntersects(bounceRay, input[j], bounceHit.distance);
			//		bHit = temp.x;
			//		if(bHit > -1.0f)
			//		{
			//			bounceHit.pos = r.origin + r.direction * hit;
			//			float4 e1 = input[j].pos1 - input[j].pos0;
			//			float4 e2 = input[j].pos2 - input[j].pos0;
			//			float3 temp = cross(e1.xyz, e2.xyz);
			//			bounceHit.normal = normalize(float4(temp, 1.0f));
			//			bounceHit.color = meshTexture[temp.yz*256.0f];
			//			tempID = input[j].ID;
			//			bounceHit.distance = hit;
			//		}
			//}
			if(bounceHit.ID != -1)
				bounceHit.ID = tempID;
			///////////////////////////////////////////////
			//Bounce Light
			///////////////////////////////////////////////
			finalColor += LightStage(bounceHit, bounceRay) * 1.0f;
		}
	}

	output[threadID.xy] = /*hd.normal;*/ float4(finalColor, 1.0f);
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
			float4 lightDir = lightList[i].pos - hd.pos;
			lightDir = normalize(lightDir);
			lightRay.origin = hd.pos;
			lightRay.direction = lightDir;
			float lightLength = length(lightList[i].pos.xyz - hd.pos.xyz);
			// ## SPHERE ## //
			if(hd.ID != -2)
			{
				hit = RaySphereIntersect(lightRay, sphere, hd.distance);
				if(hit > -1.0f)
				{
					lightHit.pos = lightRay.origin + lightRay.direction * hit;
					lightHit.normal = normalize(lightHit.pos - sphere.center);
					lightHit.color = sphere.color;
					lightHit.distance = hit;
					lightHit.ID	= sphere.ID;
				}
			}
			// ## CUBE ## //
			for(int j = 0; j < NROFTRIANGLES; j++)
			{
				if(hd.ID-j)
				{
					hit = RayTriangleIntersect(lightRay, triangles[j], hd.distance);
					if(hit > -1.0f)
					{
						lightHit.pos = lightRay.origin + lightRay.direction * hit;  ///////////////////////////////////////////////###### OBS!!! r
						float4 e1 = triangles[j].pos1 - triangles[j].pos0;
						float4 e2 = triangles[j].pos2 - triangles[j].pos0;
						float3 temp = cross(e1.xyz, e2.xyz);
						lightHit.normal = normalize(float4(temp, 1.0f));
						lightHit.color = triangles[j].color;
						lightHit.ID = triangles[j].ID;
						lightHit.distance = hit;
					}
				}
			}


			// ## Single Mesh Triangle ## //
			if(hd.ID != input[0].ID)
			{
				float3 temp;
				temp = RayTriangleIntersects(lightRay, input[0], hd.distance);
				hit = temp.x;
				if(hit > -1.0f)
				{
					lightHit.pos = lightRay.origin + lightRay.direction * hit; ///////////////////////////////////////////////###### OBS!!! r
					/*float4 e1 = input[j].pos1 - input[j].pos0;
					float4 e2 = input[j].pos2 - input[j].pos0;
					float3 temp = cross(e1.xyz, e2.xyz);*/
					lightHit.normal = normalize(input[0].normal);
					lightHit.color = float4(0.f, 0.f, 1.f, 0.f);//meshTexture[temp.yz*256.0f];
					lightHit.ID = input[0].ID;
					lightHit.distance = hit;
				}
			}

			// ## MESH ## //
			//for(int k = 0; k < nrOfFaces; k++)
			//{
			//	if(hd.ID != k)
			//	{
			//		float3 temp;
			//		temp = RayTriangleIntersects(r, input[k], hd.distance);
			//		hit = temp.x;
			//		if(hit > -1.0f)
			//		{
			//			lightHit.pos = r.origin + r.direction * hit;
			//			float4 e1 = input[k].pos1 - input[k].pos0;
			//			float4 e2 = input[k].pos2 - input[k].pos0;
			//			float3 temp = cross(e1.xyz, e2.xyz);
			//			lightHit.normal = normalize(float4(temp, 1.0f));
			//			lightHit.color = meshTexture[temp.yz*256.0f];
			//			lightHit.ID = input[k].ID;
			//			hd.distance = hit;
			//		}
			//	}
			//}
			if(lightHit.distance > 0.0f && lightLength > lightHit.distance)
			{
				color = (PointLightR(hd.pos, hd.normal, hd.color, lightList[i], r)*0.5f);
				
			}	
			else 
				color = PointLightR(hd.pos, hd.normal, hd.color, lightList[i], r);
		

			final += color;
		}
		return final;
}