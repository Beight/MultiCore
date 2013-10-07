
struct Ray
{
	float4 origin;
	float4 direction;
};

struct Sphere
{
	float4 center;
	float radius;
	float4 color;
};

struct Triangle
{
	float4 pos0;
	float4 pos1;
	float4 pos2;
	float4 color;
};

struct HitData
{
	float4 pos;
	float4 normal;
	float distance;
	float4 color;
};

HitData RaySphereIntersect(Ray p_ray, Sphere p_sphere, HitData h)
{
	HitData hd;
	hd.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	hd.distance = -1.0f;
	float4 length = p_sphere.center - p_ray.origin;
	//s = Projection of length onto ray direction
	float s = dot(length, p_ray.direction);
	float lengthSquared = dot(length, length);
	float radiusSquared = p_sphere.radius * p_sphere.radius;

	if(s < 0 && lengthSquared > radiusSquared)
	{
		//miss
		return h;
	}
	//m = Squared distance from sphere center to projection
	float m = lengthSquared - (s*s);
	
	if(m > radiusSquared)
	{
		//miss
		return h;
	}
	//q = Squared distance
	float q = sqrt(radiusSquared - m);
	if(lengthSquared > radiusSquared)
	{
		hd.distance = s - q;
		if(hd.distance < h.distance)
		{
			hd.pos = p_ray.origin + p_ray.direction * hd.distance;
			hd.normal = normalize(hd.pos - p_sphere.center);
			hd.color = p_sphere.color;
			return hd;
		}
		else return h;
	}

	return h;
}

HitData RayTriangleIntersect(Ray p_ray, Triangle p_tri, HitData h)
{
	HitData hd;
	hd.distance = -1;
	hd.color = float4(0.0f, 0.0f, 0.0f, 0.0f);


	float4 e1 = p_tri.pos1 - p_tri.pos0;
	float4 e2 = p_tri.pos2 - p_tri.pos0;
	float3 q = cross(p_ray.direction.xyz, e2.xyz);
	float a = dot(e1.xyz, q);
	if(a > -0.00001f && a < 0.00001f)
	{
		//miss
		return h;
	}

	float f = 1/a;
	float4 s = p_ray.origin - p_tri.pos0;
	float u = f *(dot(s.xyz, q));
	if(u < 0.0f)
	{
		//miss
		return h;
	}
	float3 r = cross(s.xyz, e1.xyz);
	float v = f * (dot(p_ray.direction.xyz, r));
	if(v < 0.0f || (u + v) > 1.0f)
	{
		//miss
		return h;
	}
	hd.distance = f * (dot(e2.xyz, r));
	if(hd.distance < h.distance)
	{
		hd.pos = p_ray.origin + p_ray.direction * hd.distance;
		float3 temp = cross(e1.xyz, e2.xyz);
		hd.normal = normalize(float4(temp, 1.0f));
		hd.color = p_tri.color;
		return hd;
	}
	else return h;
}