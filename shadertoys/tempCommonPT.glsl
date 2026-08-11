#if GL_ES
precision highp float;
#endif

/* ----------------- Settings ----------------- */

/* Scene number
 *
 * 1 - Cornel Box (Slow-ish)
 * 2 - Outside Shapes
 * 3 - Reflections & Emmisive Balls
 * 4 - Ball Pyramid
 * 5 - Reinder Box - No Anim. - NOT WORKING
 */
#define SCENE 4

#if (SCENE == 1)

// Array lengths
#define SPH_NUM 5
#define BOX_NUM 5
#define PLN_NUM 0
#define SPH_LIGHT_NUM 4

#elif (SCENE == 2)

// Array lengths
#define SPH_NUM 4
#define BOX_NUM 1
#define PLN_NUM 1
#define SPH_LIGHT_NUM 1

#elif (SCENE == 3)

// Array lengths
#define SPH_NUM 4
#define BOX_NUM 0
#define PLN_NUM 3
#define SPH_LIGHT_NUM 4

#elif (SCENE == 4)

// Array lengths
#define SPH_NUM 10
#define BOX_NUM 0
#define PLN_NUM 1
#define SPH_LIGHT_NUM 0

#elif (SCENE == 5)

// Array lengths

#define SPH_NUM 3
#define BOX_NUM 0
#define PLN_NUM 5
#define SPH_LIGHT_NUM 1

#endif

// Exposure & Sky settings
#define EXPOSURE 1.0
#if (SCENE == 3 || SCENE == 5)
#define SKYBOX_COLOR_MULT 0.2
#else
#define SKYBOX_COLOR_MULT 0.9
#endif

// Ray Settings
#define MAX_BOUNCES 6
#define SAMPLES 1
#define MIN_BOUNCE_RUSSIAN_ROULETTE 2

// Camera Position
#if (SCENE == 1)
#define CAMERA_DISPLACEMENT vec3(0.0, 1.0, 1.0)
#define CAMERA_DISTANCE 10.0
#else
#define CAMERA_DISPLACEMENT vec3(0.0, 0.0, 4.0)
#define CAMERA_DISTANCE 10.0
#endif
#define INITIAL_CAMERA_YAW 0.1
#define INITIAL_CAMERA_PITCH 3.14159265359/2.0
// Camera Field Of View
#define FOV 60.0

// Anti Aliassing (0==Off; 1==On)
#define ANTI_ALIASING 0
#define TAA 1

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
// Material types
#define LAMBERTION 0
#define METAL 1
// Object types
#define SPH 0
#define BOX 1
#define PLN 2
// Camera Angle Settings
#define MIN_CAMERA_ANGLE 0.01
#define MAX_CAMERA_ANGLE (PI - MIN_CAMERA_ANGLE)

// MIS & Accumulation (0==Off; 1==On)
#define MIS 1
#define ACCUM 1
// Over-writes ACCUM
#define TEMPORAL_REPROJECTION 0

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

struct Sphere { 
    vec3 origin;
    float radius;
    Material material;
};

struct Box {
    vec3 minPos;
    vec3 maxPos;
    Material material;
};

struct Plane {
    vec3 normal;
    float height;
    Material material;
};

struct Scene {
    #if (SPH_NUM > 0)
    Sphere spheres[SPH_NUM];
    #endif
    #if (BOX_NUM > 0)
    Box boxes[BOX_NUM];
    #endif
    #if (PLN_NUM > 0)
    Plane planes[PLN_NUM];
    #endif

    #if (SPH_LIGHT_NUM > 0)
    float emmisiveValues[SPH_LIGHT_NUM];
    float totalEmmisiveValue;
    #endif
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



/* ----------------- Scene ----------------- */

#if (SPH_NUM > 0)
// Static Array of Spheres
const Sphere spheres[SPH_NUM] = Sphere[SPH_NUM](
    #if (SCENE == 1)
    Sphere( // Emmissive spheres MUST be at the front of the array
        vec3(-3.0, 3.8, 2.9),
        0.4,
        Material(
            vec3(1.0),
            vec3(0.86, 0.57, 1.0) * 10.0,

            0.0,
            0.0,

            LAMBERTION
        )
    ),
    Sphere( // Emmissive spheres MUST be at the front of the array
        vec3(3.0, 3.8, 2.9),
        0.4,
        Material(
            vec3(1.0),
            vec3(0.12, 0.12, 1.0) * 10.0,

            0.0,
            0.0,

            LAMBERTION
        )
    ),
    Sphere( // Emmissive spheres MUST be at the front of the array
        vec3(3.0, -1.4, 2.9),
        0.4,
        Material(
            vec3(1.0),
            vec3(1.0, 0.22, 0.16) * 10.0,

            0.0,
            0.0,

            LAMBERTION
        )
    ),
    Sphere( // Emmissive spheres MUST be at the front of the array
        vec3(-3.0, -1.4, 2.9),
        0.4,
        Material(
            vec3(1.0),
            vec3(0.11, 1.0, 0.5) * 10.0,

            0.0,
            0.0,

            LAMBERTION
        )
    ),
    Sphere(
        vec3(0.0, -0.5, 1.0),
        1.5,
        Material(
            vec3(0.8),
            vec3(0.0),

            0.4,
            1.0,

            METAL
        )
    )
    #elif (SCENE == 2)
    Sphere( // Emmissive spheres MUST be at the front of the array
        vec3(-3.7, 1.5, 1.5),
        0.4,
        Material(
            vec3(1.0),
            vec3(9.0, 6.0, 10.5),

            0.0,
            0.0,

            LAMBERTION
        )
    ),
    Sphere(
        vec3(4.5, 0.0 + EPSILON, 5.0),
        2.0,
        Material(
            vec3(1.3, 0.4, 0.1),
            vec3(0.0),

            0.3,
            1.0,

            METAL
        )
    ),
    Sphere(
        vec3(0.4, 0.0, 9.0),
        2.0,
        Material(
            vec3(0.8, 0.2 + EPSILON, 0.8),
            vec3(0.0),

            0.48,
            0.35,

            METAL
        )
    ),
    Sphere(
        vec3(-4.8, 0.0 + EPSILON, 3.5),
        2.0,
        Material(
            vec3(0.1, 0.4, 0.6),
            vec3(0.0),

            0.7,
            0.0,

            LAMBERTION
        )
    )
    #elif (SCENE == 3)
    Sphere(
        vec3(-2, 3, 5),
        0.5,
        Material(
            vec3(1.0),
            vec3(0.0, 0.0, 1.0) * 8.0,

            0.0,
            0.0,

            LAMBERTION
        )
    ),
    Sphere(
        vec3(2, 3, 5),
        1.0,
        Material(
            vec3(0.0),
            vec3(0.12, 0.12, 1.0) * 5.0,

            0.0,
            0.0,

            LAMBERTION
        )
    ),
    Sphere(
        vec3(6, 3, 5),
        1.5,
        Material(
            vec3(1.0),
            vec3(1.0, 0.0, 0.0) * 2.0,

            0.0,
            0.0,

            LAMBERTION
        )
    ),
    Sphere(
        vec3(-6, 3, 5),
        0.1,
        Material(
            vec3(1.0),
            vec3(0.11, 1.0, 0.5) * 10.0,

            0.0,
            0.0,

            LAMBERTION
        )
    )
    #elif (SCENE == 4)
    Sphere( // B-F-L (1)
        vec3(-2.0, -2.0, 3.0),
        1.0,
        Material(
            vec3(1.0),
            vec3(0.0),

            0.3,
            1.0,

            METAL
        )
    ),
    Sphere( // B-F-M (2)
        vec3(0.0, -2.0, 3.0),
        1.0,
        Material(
            vec3(1.0),
            vec3(0.0),

            0.3,
            1.0,

            METAL
        )
    ),
    Sphere( // B-F-R (3)
        vec3(2.0, -2.0, 3.0),
        1.0,
        Material(
            vec3(1.0),
            vec3(0.0),

            0.3,
            1.0,

            METAL
        )
    ),
    Sphere( // B-M-L (4)
        vec3(-1.0, -2.0, 3.0 + sqrt(3.0)),
        1.0,
        Material(
            vec3(1.0),
            vec3(0.0),

            0.3,
            1.0,

            METAL
        )
    ),
    Sphere( // B-M-R (5)
        vec3(1.0, -2.0, 3.0 + sqrt(3.0)),
        1.0,
        Material(
            vec3(1.0),
            vec3(0.0),

            0.3,
            1.0,

            METAL
        )
    ),
    Sphere( // B-B-M (6)
        vec3(0.0, -2.0, 4.0 + sqrt(6.0)),
        1.0,
        Material(
            vec3(1.0),
            vec3(0.0),

            0.3,
            1.0,

            METAL
        )
    ),
    Sphere( // M-F-L (7)
        vec3(-1.0, -2.0 + sqrt(8.0)/sqrt(3.0), 3.0 + sqrt(3.0)/3.0 ),
        1.0,
        Material(
            vec3(1.0),
            vec3(0.0),

            0.3,
            1.0,

            METAL
        )
    ),
    Sphere( // M-F-R (8)
        vec3(1.0, -2.0 + sqrt(8.0)/sqrt(3.0), 3.0 + sqrt(3.0)/3.0 ),
        1.0,
        Material(
            vec3(1.0),
            vec3(0.0),

            0.3,
            1.0,

            METAL
        )
    ),
    Sphere( // M-F-R (9)
        vec3(0.0, -2.0 + sqrt(8.0)/sqrt(3.0), 3.0 + 4.0*sqrt(3.0)/3.0 ),
        1.0,
        Material(
            vec3(1.0),
            vec3(0.0),

            0.3,
            1.0,

            METAL
        )
    ),
    Sphere( // T-M-M (10)
        vec3(0.0, -2.0 + 2.0*sqrt(8.0)/sqrt(3.0), 3.0 + 2.0*sqrt(3.0)/3.0 ),
        1.0,
        Material(
            vec3(1.0),
            vec3(0.0),

            0.3,
            1.0,

            METAL
        )
    )

    #elif (SCENE == 5)
    Sphere(
        vec3(-2.0, 1.5, -1.0),
        0.6,
        Material(
            vec3(0.0),
            vec3(1.0) * 5.0,
            
            1.0,
            0.0,
            
            LAMBERTION
        )
    ),
    Sphere(
        vec3(1.0, -1.0, -0.3),
        1.0,
        Material(
            vec3(0.8),
            vec3(0.0),
            
            1.0,
            0.0,
            
            LAMBERTION
        )
    ),
    Sphere(
        vec3(-1.2, -1.0, 0.5),
        1.0,
        Material(
            vec3(0.9),
            vec3(0.0),
            
            0.1,
            1.0,
            
            METAL
        )
    )

    #endif
);

#endif
#if (BOX_NUM > 0)
// Static array of boxes
const Box boxes[BOX_NUM] = Box[BOX_NUM](
    #if (SCENE == 1)
    Box( // Top
        vec3(-3.6, 4.5, -2.0),
        vec3(3.7, 4.6, 3.5),
        Material(
            vec3(0.3, 0.7, 0.1),
            vec3(0.0),
            1.0,
            0.0,
            LAMBERTION
        )
    ),
    Box( // Bottom
        vec3(-3.7, -2.1, -2.0),
        vec3(3.6, -2.0, 3.5),
        Material(
            vec3(0.2, 0.4, 0.6),
            vec3(0.0),
            1.0,
            0.0,
            LAMBERTION
        )
    ),
    Box( // Left
        vec3(-3.7, -2.0, -2.0),
        vec3(-3.6, 4.6, 3.5),
        Material(
            vec3(0.6, 0.4, 0.05),
            vec3(0.0),
            1.0,
            0.0,
            LAMBERTION
        )
    ),
    Box( // Right
        vec3(3.6, -2.1, -2.0),
        vec3(3.7, 4.6, 3.5),
        Material(
            vec3(0.75, 0.85, 0.2),
            vec3(0.0),
            1.0,
            0.0,
            LAMBERTION
        )
    ),
    Box( // Front
        vec3(-3.6, -2.0, 3.5),
        vec3(3.6, 4.5, 3.6),
        Material(
            vec3(0.7, 0.6, 0.7),
            vec3(0.0),
            1.0,
            0.0,
            LAMBERTION
        )
    )
    #elif (SCENE == 2)
    Box(
        vec3(-4.0, -2.0, 9.0),
        vec3(-2.0,  3.1, 7.0),
        Material(
            vec3(0.8, 0.1, 0.2),
            vec3(0.0),
            0.1,
            1.0,
            METAL
        )
    )
    #endif
);
#endif
#if (PLN_NUM > 0)
// Static array of planes
const Plane planes[PLN_NUM] = Plane[PLN_NUM](
    #if (SCENE == 1)
    #elif (SCENE == 2)
    Plane( // Bottom
        normalize(vec3(0.0, 1.0, 0.0)),
        -2.0,
        Material(
            vec3(0.5, 0.7, 0.1),
            vec3(0.0),

            0.43,
            0.4,

            METAL
        )
    )
    #elif (SCENE == 3)
    Plane( // Top
        normalize(vec3(0.0, 0.5, -0.7)),
        -6.0,
        Material(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0),

            0.8,
            1.0,

            METAL
        )
    ),
    Plane( // Mid
        normalize(vec3(0.0, 0.7, -0.3)),
        -2.0,
        Material(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0),

            0.5,
            1.0,

            METAL
        )
    ),
    Plane( // Bottom
        normalize(vec3(0.0, 1.0, -0.3)),
        -2.0,
        Material(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0),

            0.2,
            1.0,

            METAL
        )
    )
    #elif (SCENE == 4)
    Plane( // Floor
        normalize(vec3(0.0, 1.0, 0.0)),
        -3.0 - 2.*EPSILON,
        Material(
            vec3(0.4, 0.3, 0.1),
            vec3(0.0),

            1.0,
            0.0,

            LAMBERTION
        )
    )
    #elif (SCENE == 5)
    Plane( // Top
        normalize(vec3(0.0, -1.0, 0.0)),
        -5.0,
        Material(
            vec3(0.9),
            vec3(0.0),
            
            1.0,
            0.0,
            
            LAMBERTION
        )
    ),
    Plane( // Bottom
        normalize(vec3(0.0, 1.0, 0.0)),
        -2.0,
        Material(
            vec3(0.9),
            vec3(0.0),
            
            1.0,
            0.0,
            
            LAMBERTION
        )
    ),
    Plane( // Left
        normalize(vec3(-1.0, 0.0, 0.0)),
        -3.0,
        Material(
            vec3(0.9),
            vec3(0.0),
            
            1.0,
            0.0,
            
            LAMBERTION
        )
    ),
    Plane( // Right
        normalize(vec3(1.0, 0.0, 0.0)),
        -3.0,
        Material(
            vec3(0.9),
            vec3(0.0),
            
            1.0,
            0.0,
            
            LAMBERTION
        )
    ),
    Plane( // Front
        normalize(vec3(0.0, 0.0, -1.0)),
        -2.0,
        Material(
            vec3(0.9),
            vec3(0.0),
            
            1.0,
            0.0,
            
            LAMBERTION
        )
    )
        
    #endif
);
#endif

// getLuma function that setScene() needs
float getLuma(vec3 color) {
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

Scene scene; // Global scene
void setScene(vec4 mouse) {

    #if (SPH_NUM > 0)
    scene.spheres = spheres;
    #endif
    #if (BOX_NUM > 0)
    scene.boxes = boxes;
    #endif
    #if (PLN_NUM > 0)
    scene.planes = planes;
    #endif

    #if (SCENE == 1)

    // Emmisive values
    scene.emmisiveValues[0] = getLuma(spheres[0].material.emmisive);
    scene.emmisiveValues[1] = getLuma(spheres[1].material.emmisive);
    scene.emmisiveValues[2] = getLuma(spheres[2].material.emmisive);
    scene.emmisiveValues[3] = getLuma(spheres[3].material.emmisive);

    // Total emmisive values
    scene.totalEmmisiveValue = scene.emmisiveValues[0] + scene.emmisiveValues[1] + scene.emmisiveValues[2] + scene.emmisiveValues[3];

    #elif (SCENE == 2)

    // Set the emmisive value & total emmisive value
    scene.emmisiveValues[0] = getLuma(spheres[0].material.emmisive);
    scene.totalEmmisiveValue = scene.emmisiveValues[0];

    #elif (SCENE == 3)

    // Emmisive values
    scene.emmisiveValues[0] = getLuma(spheres[0].material.emmisive);
    scene.emmisiveValues[1] = getLuma(spheres[1].material.emmisive);
    scene.emmisiveValues[2] = getLuma(spheres[2].material.emmisive);
    scene.emmisiveValues[3] = getLuma(spheres[3].material.emmisive);

    // Total emmisive values
    scene.totalEmmisiveValue = scene.emmisiveValues[0] + scene.emmisiveValues[1] + scene.emmisiveValues[2] + scene.emmisiveValues[3];

    #endif
}



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


/* ----------------- Helper ----------------- */

void fetchMaterial(const in HitInfo hit, out Material material) {
    if (hit.objectType == BOX) { // Box material
        #if (BOX_NUM > 0)
        material = scene.boxes[hit.objectIndex].material;
        #endif
    } else if (hit.objectType == PLN) { // Plane material
        #if (PLN_NUM > 0)
        material = scene.planes[hit.objectIndex].material;
        #endif
    } else { // Sphere material (default)
        #if (SPH_NUM > 0)
        material = scene.spheres[hit.objectIndex].material;
        #endif
    }
}



/* ----------------- Normal Packing & G-Buffer ----------------- */

// Helper to prevent sign(0.0) from returning 0.0
vec2 signNotZero(vec2 v) {
    return mix(vec2(-1.0), vec2(1.0), step(vec2(0.0), v));
}

// Packs a 3D unit normal into a 2D vector in the range [-1, 1]
vec2 packNormal(vec3 n) {
    vec2 p = n.xy / (abs(n.x) + abs(n.y) + abs(n.z));
    return (n.z <= 0.0) ? (1.0 - abs(p.yx)) * signNotZero(p) : p;
}

// Unpacks a 2D normal vector back into a 3D normal vector
vec3 unpackNormal(vec2 e) {
    vec3 n = vec3(e.xy, 1.0 - abs(e.x) - abs(e.y));
    if (n.z < 0.0) {
        n.xy = (1.0 - abs(n.yx)) * signNotZero(n.xy);
    }
    return normalize(n);
}

// Helper function to unpack G-Buffer data and retrieve hit/material properties
bool unpackGBuffer(const in Ray ray, out HitInfo hit, out Material material, in sampler2D channel, vec2 fragCoord) {
    // Get the g-buffer information
    vec4 gBuffer = texelFetch(channel, ivec2(fragCoord), 0);

    // Get the hit information
    float dist = gBuffer.z;
    float encodedID = gBuffer.w;

    // Check for a background/sky miss
    if (dist >= SUPER_FAR || encodedID < 0.0) {
        return false;
    }

    // Set hit info
    hit.hit = true;
    hit.dist = dist;
    hit.normal = unpackNormal(gBuffer.xy); // Unpack the normal
    hit.hitPos = ray.origin + ray.direction * hit.dist;

    // Unpack IDs safely away from float rounding issues
    int idInt = int(encodedID + 0.5);
    hit.objectType = idInt / 100;
    hit.objectIndex = idInt % 100;

    // Retrieve material information
    fetchMaterial(hit, material);
    return true;
}



/* ----------------- Anti Aliasing ----------------- */

// Halton 2, 3 series
vec2 halton(int index)
{
    const vec2 coprimes = vec2(2.0f, 3.0f);
    vec2 s = vec2(index, index);
	  vec4 a = vec4(1,1,0,0);
    while (s.x > 0. && s.y > 0.)
    {
        a.xy = a.xy/coprimes;
        a.zw += a.xy*mod(s, coprimes);
        s = floor(s/coprimes);
    }
    return a.zw;
}



/* ----------------- Reprojection ----------------- */

// Reconstructs the world-space hit position from a G-Buffer depth sample.
vec3 reconstructWorld(vec2 fragCoord, float depth, vec2 mouse, vec2 resolution) {

    // Calculate uv and FOV dist
    vec2 uv = (fragCoord / resolution * 2.0 - 1.0) 
              * vec2(resolution.x / resolution.y, 1.0);
    float cameraDist = 1.0 / tan(FOV * 0.5 * PI / 180.0);

    // Get the camera state
    vec3 camPos, camFwd, camUp, camRight;
    getCamereaState(mouse, resolution, camPos, camFwd, camUp, camRight);

    // Final ray dir
    vec3 rayDir = normalize(
        mat3(camRight, camUp, camFwd) * normalize(vec3(uv, cameraDist))
    );

    // Use the camera state w/ the rayDir & depth to get the hit world-pos
    return camPos + rayDir * depth;
}

// Projects a world-space point into screen UV [0,1]^2 using a given camera state.
vec2 projectToScreen(vec3 worldPos, vec2 mouse, vec2 resolution) {

    // Get the camera state
    vec3 camPos, camFwd, camUp, camRight;
    getCamereaState(mouse, resolution, camPos, camFwd, camUp, camRight);

    vec3  toPoint = worldPos - camPos;
    float fwd     = dot(toPoint, camFwd);
    if (fwd <= 0.0) return vec2(-1.0);   // behind camera

    float cameraDist = 1.0 / tan(FOV * 0.5 * PI / 180.0);
    float aspect     = resolution.x / resolution.y;

    float ndcX = dot(toPoint, camRight) / fwd * cameraDist / aspect;
    float ndcY = dot(toPoint, camUp)    / fwd * cameraDist;

    return vec2(ndcX, ndcY) * 0.5 + 0.5;
}



/* ----------------- Intersections ----------------- */

// Ray-Sphere distance
float intersectSphere(Ray ray, const in Sphere sphere) {

    // Move the world around the ray
    vec3 oc = ray.origin - sphere.origin;

    // Quadradic formula (discriminant) solving - No "a" as it is always 1
    float b = dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - c;

    // One single clean branch to catch complete misses
    if (discriminant > EPSILON) {

        // Sqrt the discriminant
        float dSqrt = sqrt(discriminant);

        // If c < 0, we are inside (add dSqrt); vice-versa (subtract dSqrt)
        float t0 = -b - dSqrt;
        float t1 = -b + dSqrt;

        return t0 > EPSILON ? t0 : (t1 > EPSILON ? t1 : 0.0);
    }

    return 0.0;
}

// Ray-Box distance
float intersectBox(Ray ray, const in Box box) {

    vec3 invDirection = 1.0 / ray.direction;

    vec3 t0 = (box.minPos - ray.origin) * invDirection;
    vec3 t1 = (box.maxPos - ray.origin) * invDirection;

    vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);

    float tNear = max(max(tmin.x, tmin.y), tmin.z);
    float tFar = min(min(tmax.x, tmax.y), tmax.z);

    if (tNear <= tFar && tFar > EPSILON) {
        float t = tNear > EPSILON ? tNear : tFar;
        if (t > EPSILON) return t;
    }
    return 0.0;
}

// Ray-Plane distance
float intersectPlane(Ray ray, const in Plane plane) {

    // Check for hit
    float denom = dot(plane.normal, ray.direction);
    if (abs(denom) < EPSILON) return 0.0;

    // Get distance
    float t = (plane.height - dot(ray.origin, plane.normal)) / denom;

    // Check for a basicly 0 distance and return the correct value
    return t > EPSILON ? t : 0.0;

}

// Ray-Scene
HitInfo rayScene(Ray ray, inout uint seed) {

    HitInfo closestHit;
    closestHit.hit = false;
    closestHit.dist = SUPER_FAR;

    #if (SPH_NUM > 0)
    // Spheres
    for (int i = 0; i < SPH_NUM; i++) {
        float t = intersectSphere(ray, scene.spheres[i]);
        if (t > EPSILON && t < closestHit.dist) {
            closestHit.dist = t;
            closestHit.objectIndex = i;
            closestHit.objectType = SPH;
            closestHit.hit = true;
        }
    }
    #endif

    #if (BOX_NUM > 0)
    // Boxes
    for (int i = 0; i < BOX_NUM; i++) {
        float t = intersectBox(ray, scene.boxes[i]);
        if (t > EPSILON && t < closestHit.dist) {
            closestHit.dist = t;
            closestHit.objectIndex = i;
            closestHit.objectType = BOX;
            closestHit.hit = true;
        }
    }
    #endif

    #if (PLN_NUM > 0)
    // Planes
    for (int i = 0; i < PLN_NUM; i++) {
        float t = intersectPlane(ray, scene.planes[i]);
        if (t > EPSILON && t < closestHit.dist) {
            closestHit.dist = t;
            closestHit.objectIndex = i;
            closestHit.objectType = PLN;
            closestHit.hit = true;
        }
    }
    #endif

    // Calculate hit attributes only once per ray
    if (closestHit.hit) {
        closestHit.hitPos = ray.origin + ray.direction * closestHit.dist;

        if (closestHit.objectType == SPH) {
            #if (SPH_NUM > 0)
            closestHit.normal = (closestHit.hitPos - scene.spheres[closestHit.objectIndex].origin) / scene.spheres[closestHit.objectIndex].radius;
            #endif
        } else if (closestHit.objectType == BOX) {
            #if (BOX_NUM > 0)
            Box box = scene.boxes[closestHit.objectIndex];

            vec3 invDirection = 1.0 / ray.direction;

            vec3 t0 = (box.minPos - ray.origin) * invDirection;
            vec3 t1 = (box.maxPos - ray.origin) * invDirection;
            vec3 tmin = min(t0, t1);
            vec3 mask = step(tmin.yzx, tmin.xyz) * step(tmin.zxy, tmin.xyz);
            closestHit.normal = -sign(ray.direction) * mask;
            #endif
        } else if (closestHit.objectType == PLN) {
            #if (PLN_NUM > 0)
            closestHit.normal = scene.planes[closestHit.objectIndex].normal;
            #endif
        }
    }

    // Final hit w/ attributes
    return closestHit;
}

// Ray-Scene hit
bool rayHit(Ray ray, float maxDist, int ignoredSphereIndex) {

    #if (SPH_NUM > 0)
    // Spheres
    for (int i = 0; i < SPH_NUM; i++) {

        if (i == ignoredSphereIndex) continue;
        float t = intersectSphere(ray, scene.spheres[i]); 
        if (t > EPSILON && t < maxDist) {
            return true; // Hit sphere
        }
    }
    #endif

    #if (PLN_NUM > 0)
    // Planes
    for (int i = 0; i < PLN_NUM; i++) {
        float t = intersectPlane(ray, scene.planes[i]); 
        if (t > EPSILON && t < maxDist) {
            return true; // Hit plane
        }
    }
    #endif


    #if (BOX_NUM > 0)
    // Boxes
    for (int i = 0; i < BOX_NUM; i++) {
        float t = intersectBox(ray, scene.boxes[i]);
        if (t > EPSILON && t < maxDist) {
            return true; // Hit box
        }
    }
    #endif

    // Did not hit anything
    return false;
}
