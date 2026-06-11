// Numbers
#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define INV_PI 0.31830988618
#define INV_TWO_PI  0.15915494309
#define EPSILON 0.001
#define FAR 25.0
#define REALLY_FAR 50.0
#define SUPER_FAR 90.0

// Get the local basis matrix
mat3 getBasis(vec3 n) {
    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
    vec3 t = normalize(cross(up, n));
    vec3 b = cross(n, t);
    return mat3(t, b, n);
}
