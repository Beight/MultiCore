
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
	float4	pos0;
	float4	pos1;
	float4	pos2;
	float4	color;
	int		ID;
};

struct HitData
{
	float4	pos;
	float4	normal;
	float	distance;
	float4	color;
	int		ID;
};

HitData RaySphereIntersect(Ray p_ray, Sphere p_sphere, HitData p_hd)
{
	HitData l_hd;
	l_hd.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	l_hd.distance = -1.0f;
	float distanceDelta = 0.001f;
	float4 length = p_sphere.center - p_ray.origin;
	//s = Projection of length onto ray direction
	float s = dot(length, p_ray.direction);
	float lengthSquared = dot(length, length);
	float radiusSquared = p_sphere.radius * p_sphere.radius;

	if(s < 0 && lengthSquared > radiusSquared)
	{
		//miss
		return p_hd;
	}
	//m = Squared distance from sphere center to projection
	float m = lengthSquared - (s*s);
	
	if(m > radiusSquared)
	{
		//miss
		return p_hd;
	}
	//q = Squared distance
	float q = sqrt(radiusSquared - m);
	if(lengthSquared > radiusSquared)
	{
		l_hd.distance = s - q;
		if(l_hd.distance < p_hd.distance || p_hd.distance < 0.0f && l_hd.distance > distanceDelta)
		{
			l_hd.pos = p_ray.origin + p_ray.direction * l_hd.distance;
			l_hd.normal = normalize(l_hd.pos - p_sphere.center);
			l_hd.color = p_sphere.color;
			return l_hd;
		}
		else return p_hd;
	}

	return p_hd;
}

HitData RayTriangleIntersect(Ray p_ray, Triangle p_tri, HitData p_hd)
{
	/*if(p_hd.ID != p_tri.ID)
	{*/
	HitData l_hd;
	float distanceDelta = 0.001f;
	l_hd.distance = -1.0f;
	l_hd.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	l_hd.ID = -1;


	float4 e1 = p_tri.pos1 - p_tri.pos0;
	float4 e2 = p_tri.pos2 - p_tri.pos0;
	float3 q = cross(p_ray.direction.xyz, e2.xyz);
	float a = dot(e1.xyz, q);
	if(a > -0.00001f && a < 0.00001f)
	{
		//miss
		return p_hd;
	}

	float f = 1/a;
	float4 s = p_ray.origin - p_tri.pos0;
	float u = f *(dot(s.xyz, q));
	if(u < 0.0f)
	{
		//miss
		return p_hd;
	}
	float3 r = cross(s.xyz, e1.xyz);
	float v = f * (dot(p_ray.direction.xyz, r));
	if(v < 0.0f || (u + v) > 1.0f)
	{
		//miss
		return p_hd;
	}
	l_hd.distance = f * (dot(e2.xyz, r));
	
	if((l_hd.distance < p_hd.distance && l_hd.distance > 0.0f) || (p_hd.distance < 0.0f && l_hd.distance > distanceDelta))
	{		
		l_hd.pos = p_ray.origin + p_ray.direction * l_hd.distance;
		float3 temp = cross(e1.xyz, e2.xyz);
		l_hd.normal = normalize(float4(temp, 1.0f));
		l_hd.color = p_tri.color;
		l_hd.ID = p_tri.ID;
		return l_hd;
	}
	
	//}
	return p_hd;
}