/**
 * common.glsl
 * Common types and functions used for ray tracing.
 */

const float pi = 3.14159265358979;
const float epsilon = 0.001;

struct Ray {
    vec3 o;     // origin
    vec3 d;     // direction - always set with normalized vector
    float t;    // time, for motion blur
};

Ray createRay(vec3 o, vec3 d, float t)
{
    Ray r;
    r.o = o;
    r.d = d;
    r.t = t;
    return r;
}

Ray createRay(vec3 o, vec3 d)
{
    return createRay(o, d, 0.0);
}

vec3 pointOnRay(Ray r, float t)
{
    return r.o + r.d * t;
}

float gSeed = 0.0;

uint baseHash(uvec2 p)
{
    p = 1103515245U * ((p >> 1U) ^ (p.yx));
    uint h32 = 1103515245U * ((p.x) ^ (p.y>>3U));
    return h32 ^ (h32 >> 16);
}

float hash1(inout float seed) {
    uint n = baseHash(floatBitsToUint(vec2(seed += 0.1,seed += 0.1)));
    return float(n) / float(0xffffffffU);
}

vec2 hash2(inout float seed) {
    uint n = baseHash(floatBitsToUint(vec2(seed += 0.1,seed += 0.1)));
    uvec2 rz = uvec2(n, n * 48271U);
    return vec2(rz.xy & uvec2(0x7fffffffU)) / float(0x7fffffff);
}

vec3 hash3(inout float seed)
{
    uint n = baseHash(floatBitsToUint(vec2(seed += 0.1, seed += 0.1)));
    uvec3 rz = uvec3(n, n * 16807U, n * 48271U);
    return vec3(rz & uvec3(0x7fffffffU)) / float(0x7fffffff);
}

float rand(vec2 v)
{
    return fract(sin(dot(v.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 toLinear(vec3 c)
{
    return pow(c, vec3(2.2));
}

vec3 toGamma(vec3 c)
{
    return pow(c, vec3(1.0 / 2.2));
}

vec2 randomInUnitDisk(inout float seed) {
    vec2 h = hash2(seed) * vec2(1.0, 6.28318530718);
    float phi = h.y;
    float r = sqrt(h.x);
	return r * vec2(sin(phi), cos(phi));
}

vec3 randomInUnitSphere(inout float seed)
{
    vec3 h = hash3(seed) * vec3(2.0, 6.28318530718, 1.0) - vec3(1.0, 0.0, 0.0);
    float phi = h.y;
    float r = pow(h.z, 1.0/3.0);
	return r * vec3(sqrt(1.0 - h.x * h.x) * vec2(sin(phi), cos(phi)), h.x);
}

vec3 randomUnitVector(inout float seed) //to be used in diffuse reflections with distribution cosine
{
    return(normalize(randomInUnitSphere(seed)));
}

struct Camera
{
    vec3 eye;
    vec3 u, v, n;
    float width, height;
    float lensRadius;
    float planeDist, focusDist;
    float time0, time1;
};

Camera createCamera(
    vec3 eye,
    vec3 at,
    vec3 worldUp,
    float fovy,
    float aspect,
    float aperture,  //diametro em multiplos do pixel size
    float focusDist,  //focal ratio
    float time0,
    float time1)
{
    Camera cam;
    if(aperture == 0.0) cam.focusDist = 1.0; //pinhole camera then focus in on vis plane
    else cam.focusDist = focusDist;
    vec3 w = eye - at;
    cam.planeDist = length(w);
    cam.height = 2.0 * cam.planeDist * tan(fovy * pi / 180.0 * 0.5);
    cam.width = aspect * cam.height;

    cam.lensRadius = aperture * 0.5 * cam.width / iResolution.x;  //aperture ratio * pixel size; (1 pixel=lente raio 0.5)
    cam.eye = eye;
    cam.n = normalize(w);
    cam.u = normalize(cross(worldUp, cam.n));
    cam.v = cross(cam.n, cam.u);
    cam.time0 = time0;
    cam.time1 = time1;
    return cam;
}

Ray getRay(Camera cam, vec2 pixel_sample)  //rnd pixel_sample viewport coordinates
{
    vec2 ls = randomInUnitDisk(gSeed) * cam.lensRadius;  //ls - lens sample for DOF

    float time = cam.time0 + hash1(gSeed) * (cam.time1 - cam.time0);

    float x_scalar = cam.width * (pixel_sample.x / iResolution.x - 0.5);
    float y_scalar = cam.height * (pixel_sample.y / iResolution.y - 0.5);

    // Create center ray
	vec3 center_ray = vec3(x_scalar, y_scalar, cam.planeDist); // goes from eye to pixel_sample, in camera coords

	vec3 actual_p = vec3(center_ray.x * cam.focusDist,
						    center_ray.y * cam.focusDist,
                            cam.focusDist * cam.planeDist); // in camera coords.

	vec3 ray_dir = normalize(cam.u * (actual_p.x - ls.x) 
						+ cam.v * (actual_p.y - ls.y) 
						- cam.n * actual_p.z); // in world coordinates

	vec3 eye_offset = cam.eye + cam.u * ls.x + cam.v * ls.y; // also in world coordinates

    return createRay(eye_offset, ray_dir, time);
}

// MT_ material type
#define MT_DIFFUSE 0
#define MT_METAL 1
#define MT_DIALECTRIC 2

struct Material
{
    int type;
    vec3 albedo;  //diffuse color
    vec3 specColor;  //the color tint for specular reflections. for metals and opaque dieletrics like coloured glossy plastic
    vec3 emissive; //
    float roughness; // controls roughness for metals. It can be used for rough refractions
    float refIdx; // index of refraction for dialectric
    vec3 refractColor; // absorption for beer's law
};

Material createDiffuseMaterial(vec3 albedo)
{
    Material m;
    m.type = MT_DIFFUSE;
    m.albedo = albedo;
    m.specColor = vec3(0.0);
    m.roughness = 1.0;  //ser usado na iluminação direta
    m.refIdx = 1.0;
    m.refractColor = vec3(0.0);
    m.emissive = vec3(0.0);
    return m;
}

Material createMetalMaterial(vec3 specClr, float roughness)
{
    Material m;
    m.type = MT_METAL;
    m.albedo = vec3(0.0);
    m.specColor = specClr;
    m.roughness = roughness;
    m.emissive = vec3(0.0);
    return m;
}

Material createDialectricMaterial(vec3 refractClr, float refIdx, float roughness)
{
    Material m;
    m.type = MT_DIALECTRIC;
    m.albedo = vec3(0.0);
    m.specColor = vec3(0.04);
    m.refIdx = refIdx;
    m.refractColor = refractClr;  
    m.roughness = roughness;
    m.emissive = vec3(0.0);
    return m;
}

struct HitRecord
{
    vec3 pos;
    vec3 normal;
    float t;            // ray parameter
    Material material;
};


float schlick(float cosine, float refIdx)
{
    float r_0;
    if (refIdx == 1.0) {
        r_0 = pow((1.0 - refIdx) / (1.0 + refIdx), 2.0);
    }
    else {
        r_0 = pow((refIdx - 1.0) / (refIdx + 1.0), 2.0);
    }
    
    return r_0 + (1.0 - r_0) * pow(1.0 - cosine, 5.0);
}

bool scatter(Ray rIn, HitRecord rec, out vec3 atten, out Ray rScattered)
{    
    if (rec.material.type == MT_DIFFUSE)
    {
        rScattered = createRay(rec.pos + epsilon * rec.normal, normalize(rec.normal + randomInUnitSphere(gSeed)));
        atten = rec.material.albedo * max(dot(rScattered.d, rec.normal), 0.0) / pi;
        
        return true;
    }
    if (rec.material.type == MT_METAL)
    {
        vec3 fuzzy_direction = normalize(reflect(rIn.d, rec.normal) + rec.material.roughness * randomInUnitSphere(gSeed)); 

        rScattered = createRay(rec.pos + epsilon * rec.normal, fuzzy_direction);
        atten = rec.material.specColor;

        return dot(fuzzy_direction, rec.normal) > 0.0;
    }

    if (rec.material.type == MT_DIALECTRIC)
    {
        atten = vec3(1.0);
        vec3 outwardNormal;
        float niOverNt;
        float cosine = -dot(rIn.d, rec.normal);
        
        bool total_internal_ref = false;

        if (dot(rIn.d, rec.normal) > 0.0) //hit inside
        {
            outwardNormal = -rec.normal;
            niOverNt = rec.material.refIdx;
            
            float sin_t2 = niOverNt * niOverNt * (1.0 - cosine * cosine);
            if (sqrt(sin_t2) > 1.0)
                total_internal_ref = true;
		    
            cosine = sqrt(1.0 - sin_t2);

            atten = exp(-rec.material.refractColor * rec.t); // t is the distance
        }
        else  //hit from outside
        {
            outwardNormal = rec.normal;
            niOverNt = 1.0 / rec.material.refIdx;
        }

        //Use probabilistic math to decide if scatter a reflected ray or a refracted ray

        float reflectProb;
        
        if (!total_internal_ref) {
            reflectProb = schlick(cosine, rec.material.refIdx);  
        }
        else {
            reflectProb = 1.0;
        }

        if (hash1(gSeed) < reflectProb) { //Reflection
            rScattered = createRay(rec.pos + epsilon * outwardNormal, reflect(rIn.d, outwardNormal));
        }  
        else { //Refraction
            rScattered = createRay(rec.pos - epsilon * outwardNormal, refract(rIn.d, outwardNormal, niOverNt));
        } 
        
        return true;
    }

    return false;
}

struct Triangle {vec3 a; vec3 b; vec3 c; };

Triangle createTriangle(vec3 v0, vec3 v1, vec3 v2)
{
    Triangle t;
    t.a = v0; t.b = v1; t.c = v2;
    return t;
}

bool hit_triangle(Triangle t, Ray r, float tmin, float tmax, out HitRecord rec)
{
    vec3 ab = t.b - t.a;
    vec3 ac = t.c - t.a;
    vec3 ao = r.o - t.a;
    
    // compute the possible intersection using Cramer's rule
    float d = 1.0 / determinant(mat3(ab, ac, -r.d));
    float beta = d * determinant(mat3(ao, ac, -r.d ));
    float gamma = d * determinant(mat3(ab, ao, -r.d ));
    float hit_time = d * determinant(mat3(ab, ac, ao));

    // since we are using baricentric coordinates, we need to make sure
    // beta >= 0, gamma >= 0, and that 0 <= beta + gamma <= 1
    if (beta < 0.0 || gamma < 0.0 || beta + gamma > 1.0) return false;

    else if (hit_time < tmax && hit_time > tmin) {
        rec.t = hit_time;
        rec.normal = normalize(cross(ab, ac));
        rec.pos = pointOnRay(r, rec.t);
        return true;
    }
    
    return false;
}


struct Sphere
{
    vec3 center;
    float radius;
};

Sphere createSphere(vec3 center, float radius)
{
    Sphere s;
    s.center = center;
    s.radius = radius;
    return s;
}


struct MovingSphere
{
    vec3 center0, center1;
    float radius;
    float time0, time1;
};

MovingSphere createMovingSphere(vec3 center0, vec3 center1, float radius, float time0, float time1)
{
    MovingSphere s;
    s.center0 = center0;
    s.center1 = center1;
    s.radius = radius;
    s.time0 = time0;
    s.time1 = time1;
    return s;
}

vec3 center(MovingSphere mvsphere, float time)
{
    vec3 moving_center = vec3(
                            mix(mvsphere.center0.x, mvsphere.center1.x,time),
                            mix(mvsphere.center0.y, mvsphere.center1.y,time),
                            mix(mvsphere.center0.z, mvsphere.center1.z,time));
    return moving_center;
}


/*
 * The function naming convention changes with these functions to show that they implement a sort of interface for
 * the book's notion of "hittable". E.g. hit_<type>.
 */

bool hit_sphere(Sphere s, Ray r, float tmin, float tmax, out HitRecord rec)
{
    vec3 oc = s.center - r.o;
    float t;

    // b = d * OC
    float b = dot(r.d, oc);

    // c = OC.OC - r^2
    float c = dot(oc, oc) - (s.radius * s.radius);

    // if ray outside
    if (c > 0.0 && b < 0.0) return false;

    float discriminant = b * b - c;
    // if discriminant is negative
    if (discriminant < 0.0) return false;

    // if origin outside
    if (c > 0.0) {
        t = b - sqrt(discriminant);
    }
    else {
        t = b + sqrt(discriminant);
    }

    // Check if t within range
    if (t < tmax && t > tmin) {
        rec.t = t;
        rec.pos = pointOnRay(r, rec.t);
        rec.normal = normalize(rec.pos - s.center);
        return true;
    }
    else {
        return false;
    }
}

bool hit_movingSphere(MovingSphere s, Ray r, float tmin, float tmax, out HitRecord rec)
{
    float B, C, delta;
    bool outside;
    float t;

    //Calculate the moving center
    vec3 center = center(s, r.t);

    //calculate a valid t and normal
    return hit_sphere(Sphere(center, s.radius), r, tmin, tmax, rec);
	
    /*
    if(t < tmax && t > tmin) {
        rec.t = t;
        rec.pos = pointOnRay(r, rec.t);
        rec.normal = normal;
        return true;
    }
    else return false;
    */
}

struct pointLight {
    vec3 pos;
    vec3 color;
};

pointLight createPointLight(vec3 pos, vec3 color) 
{
    pointLight l;
    l.pos = pos;
    l.color = color;
    return l;
}