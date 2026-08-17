#if GL_ES
precision highp float;
#endif

#define MAX_BOUNCES 6
#define SAMPLES 2
#define MIN_BOUNCE_RUSSIAN_ROULETTE 2

// Exposure & Sky settings
#define EXPOSURE 1.0
#define SKYBOX_COLOR_MULT 1.0

// Camera Position
#define CAMERA_DISPLACEMENT vec3(0.0, 0.0, 4.0)
#define CAMERA_DISTANCE 10.0

#define INITIAL_CAMERA_YAW 0.1
#define INITIAL_CAMERA_PITCH 3.14159265359/2.0
// Camera Field Of View
#define FOV 60.0

// Numbers
#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define INV_PI 0.31830988618
#define INV_TWO_PI  0.15915494309
#define EULERS 2.71828182846
#define EPSILON 0.001
#define FAR 25.0
#define REALLY_FAR 50.0
#define SUPER_FAR 90.0
// Camera Angle Settings
#define MIN_CAMERA_ANGLE 0.01
#define MAX_CAMERA_ANGLE (PI - MIN_CAMERA_ANGLE)

// Post-processing (0==Off; 1==On)
#define SDR 1
#define SRGB 1
#define DO_EXPOSURE 1
#define DENOISE 1



/* ----------------- Structs ----------------- */

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Material {
    vec3 color;
    vec3 emmisive;

    float roughness;
    float metallic;

    int type;
};

struct HitInfo {
    bool hit;
    float dist;

    vec3 hitPos;
    vec3 normal;

    int objectIndex;
    int objectType;
};

struct LightSample {
    vec3 direction;
    vec3 radiance;
    float dist;
    float pdf;
};



/* ----------------- Camera ----------------- */

float CAMERA_YAW   = 0.0; // Horizontal rotation angle (radians)
float CAMERA_PITCH = 0.0; // Vertical rotation angle (radians), clamped to [MIN_CAMERA_ANGLE, MAX_CAMERA_ANGLE]
float CAMERA_ROLL  = 0.0; // Roll angle (radians), currently unused but reserved

// Get the camera vectors based on the mousepos
void getCamereaState(vec2 mouse, vec2 resolution, out vec3 cameraPos, out vec3 cameraFwd, out vec3 cameraUp, out vec3 cameraRight)
{
    // If the mouse is at (0,0) it hasn't been moved yet, so use a default camera setup
    if (mouse == vec2(0.0))
    {
        cameraPos = CAMERA_DISPLACEMENT - vec3(0.0, 0.0, CAMERA_DISTANCE);
        cameraFwd = vec3(0.0, 0.0, 1.0);
        cameraUp = vec3(0.0, 1.0, 0.0);
        cameraRight = vec3(1.0, 0.0, 0.0);

        cameraPos.x = sin(INITIAL_CAMERA_YAW) * sin(INITIAL_CAMERA_PITCH) * CAMERA_DISTANCE;
        cameraPos.y = -cos(INITIAL_CAMERA_PITCH) * CAMERA_DISTANCE;
        cameraPos.z = cos(INITIAL_CAMERA_YAW) * sin(INITIAL_CAMERA_PITCH) * CAMERA_DISTANCE;

        // Modify the camera position by the constant displacement
        cameraPos += CAMERA_DISPLACEMENT;

        // Forward Vector
        cameraFwd = normalize(CAMERA_DISPLACEMENT - cameraPos);

        // Saftey for weird angles that would cause vectors approaching 0
        vec3 worldRef = abs(cameraFwd.y) > 0.999 ? vec3(0.0, 0.0, 1.0) 
                                               : vec3(0.0, 1.0, 0.0);

        // Cross product our way to freedom
        cameraRight = normalize(cross(worldRef, cameraFwd));
        cameraUp    = normalize(cross(cameraFwd, cameraRight));

        return;
    }

    // Get the Euler angles based on the mouse
    CAMERA_YAW   = -mouse.x * 16.0 / float(resolution.x);
    CAMERA_PITCH = mix(MIN_CAMERA_ANGLE, MAX_CAMERA_ANGLE, mouse.y / float(resolution.y));
    CAMERA_ROLL  = 0.0;

    // Rotate based on the Euler angles
    cameraPos.x = sin(CAMERA_YAW) * sin(CAMERA_PITCH) * CAMERA_DISTANCE;
    cameraPos.y = -cos(CAMERA_PITCH) * CAMERA_DISTANCE;
    cameraPos.z = cos(CAMERA_YAW) * sin(CAMERA_PITCH) * CAMERA_DISTANCE;

    // Modify the camera position by the constant displacement
    cameraPos += CAMERA_DISPLACEMENT;

    // Forward Vector
    cameraFwd = normalize(CAMERA_DISPLACEMENT - cameraPos);

    // Saftey for weird angles that would cause vectors approaching 0
    vec3 worldRef = abs(cameraFwd.y) > 0.999 ? vec3(0.0, 0.0, 1.0) 
                                           : vec3(0.0, 1.0, 0.0);

    // Cross product our way to freedom
    cameraRight = normalize(cross(worldRef, cameraFwd));
    cameraUp    = normalize(cross(cameraFwd, cameraRight));
}



/* ----------------- Random Functions ----------------- */

uint seed; // Global seed

// Backup seed for when the blue noise texture isn't loaded
uint getBackupSeed(vec2 fragCoord, uint frame)
{
    return uint(fragCoord.x) * 1973u + uint(fragCoord.y) * 9277u + frame * 26699u | 1u;
}

// Seed pulled from blue noise
void setSeed(vec2 fragCoord, uint frame, sampler2D noise) {

    // Go over the uv of the noise to pick a spot, change the uv on the golden ratio
    vec2 noiseUV = fragCoord / 1024.0 + fract(float(frame) * vec2(0.6180339887, 0.7548776662));
    float blueNoise = texture(noise, noiseUV).r;

    // Inject it into the seed
    if (blueNoise > 0.0)
        seed = uint(blueNoise * 4294967295.0) ^ (frame * uint(26699));
    else 
        seed = getBackupSeed(fragCoord, frame);
}

// Hash function for random uint
uint wang_hash(inout uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

// Random float one from [0-1]
float rnd1(inout uint seed){
    return float(wang_hash(seed)) / float(-1u);
}

// Random normalized unit vector
vec3 rndUnit(inout uint seed) {
    float z = rnd1(seed) * 2.0f - 1.0f;
    float a = rnd1(seed) * 6.28318530718;
    float r = sqrt(1.0f - z * z);
    return vec3(r * cos(a), r * sin(a), z);
}



/* ----------------- SRGB ----------------- */

vec3 LessThan(vec3 f, float value) {
    return vec3(
        (f.x < value) ? 1.0f : 0.0f,
        (f.y < value) ? 1.0f : 0.0f,
        (f.z < value) ? 1.0f : 0.0f
    );
}

vec3 LinearToSRGB(vec3 rgb) {
    rgb = clamp(rgb, 0.0f, 1.0f);
    return mix(
        pow(rgb, vec3(1.0f / 2.4f)) * 1.055f - 0.055f,
        rgb * 12.92f,
        LessThan(rgb, 0.0031308f)
    );
}

vec3 SRGBToLinear(vec3 rgb) {
    rgb = clamp(rgb, 0.0f, 1.0f);
    return mix(
        pow(((rgb + 0.055f) / 1.055f), vec3(2.4f)),
        rgb / 12.92f,
        LessThan(rgb, 0.04045f)
    );
}



/* ----------------- Tone Mapping / SDR ----------------- */

vec3 ACESFilm(vec3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x + b)) / (x*(c*x + d) + e), 0.0f, 1.0f);
}
