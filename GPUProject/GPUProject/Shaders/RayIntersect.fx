
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
	float4	color;
	int		ID;
	float3	pad;
};

struct MeshTriangle
{
	float4	pos0;
	float4	pos1;
	float4	pos2;
	float2	textureCoordinate0;
	float2	textureCoordinate1;
	float2	textureCoordinate2;
	float4	normal;
	int		ID;
	float4  b1;
	float	b2;
};

struct HitData
{
	float4	pos;
	float4	normal;
	float	distance;
	float4	color;
	int		ID;
};

float RaySphereIntersect(Ray p_ray, Sphere p_sphere, float p_dist)
{
	float l_t = -1.0f;

	float distanceDelta = 0.001f;
	float4 length = p_sphere.center - p_ray.origin;
	//s = Projection of length onto ray direction
	float s = dot(length, p_ray.direction);
	float lengthSquared = dot(length, length);
	float radiusSquared = p_sphere.radius * p_sphere.radius;

	if(s < 0 && lengthSquared > radiusSquared)
	{
		//miss
		return -1.0f;
	}
	//m = Squared distance from sphere center to projection
	float m = lengthSquared - (s*s);
	
	if(m > radiusSquared)
	{
		//miss
		return -1.0f;
	}
	//q = Squared distance
	float q = sqrt(radiusSquared - m);
	if(lengthSquared > radiusSquared)
	{
		l_t = s - q;
		if(l_t < p_dist || p_dist < 0.0f && l_t > distanceDelta)
		{
			return l_t;
		}
		else return -1.0f;
	}

	return -1.0f;
}

float RayTriangleIntersect(Ray p_ray, Triangle p_tri, float p_dist)
{
	float distanceDelta = 0.001f;
	float l_t;

	float4 e1 = p_tri.pos1 - p_tri.pos0;
	float4 e2 = p_tri.pos2 - p_tri.pos0;
	float3 q = cross(p_ray.direction.xyz, e2.xyz);
	float a = dot(e1.xyz, q);
	if(a > -0.00001f && a < 0.00001f)
	{
		//miss
		return -1.0f;
	}

	float f = 1/a;
	float4 s = p_ray.origin - p_tri.pos0;
	float u = f *(dot(s.xyz, q));
	if(u < 0.0f)
	{
		//miss
		return -1.0f;
	}
	float3 r = cross(s.xyz, e1.xyz);
	float v = f * (dot(p_ray.direction.xyz, r));
	if(v < 0.0f || (u + v) > 1.0f)
	{
		//miss
		return -1.0f;
	}
	l_t = f * (dot(e2.xyz, r));
	
	if((l_t < p_dist && l_t > 0.0f) || (p_dist < 0.0f && l_t > distanceDelta))
	{		
		return l_t;
	}
	return -1.0f;
}

float3 RayTriangleIntersects(Ray p_ray, MeshTriangle p_tri, float p_dist)
{
	float distanceDelta = 0.001f;
	float l_t;

	float4 e1 = p_tri.pos1 - p_tri.pos0;
	float4 e2 = p_tri.pos2 - p_tri.pos0;
	float3 q = cross(p_ray.direction.xyz, e2.xyz);
	float a = dot(e1.xyz, q);
	if(a > -0.00001f && a < 0.00001f)
	{
		//miss
		return float3(-1.0f, 0.0f, 0.0f);
	}

	float f = 1/a;
	float4 s = p_ray.origin - p_tri.pos0;
	float u = f *(dot(s.xyz, q));
	if(u < 0.0f)
	{
		//miss
		return float3(-1.0f, 0.0f, 0.0f);
	}
	float3 r = cross(s.xyz, e1.xyz);
	float v = f * (dot(p_ray.direction.xyz, r));
	if(v < 0.0f || (u + v) > 1.0f)
	{
		//miss
		return float3(-1.0f, 0.0f, 0.0f);
	}
	l_t = f * (dot(e2.xyz, r));
	
	if((l_t < p_dist && l_t > 0.0f) || (p_dist < 0.0f && l_t > distanceDelta))
	{	
		float2 index = ((1-u-v) * p_tri.textureCoordinate0) + (u * p_tri.textureCoordinate1) + (v * p_tri.textureCoordinate2);
		return float3(l_t, index);
	}
	return float3(-1.0f, 0.0f, 0.0f);
}