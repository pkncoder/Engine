#pragma once

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace glm {

inline void from_json(const nlohmann::json& j, vec3& v) {
    if (j.is_array() && j.size() >= 3) {
        v.x = j[0].get<float>();
        v.y = j[1].get<float>();
        v.z = j[2].get<float>();
    }
}

inline void from_json(const nlohmann::json& j, quat& q) {
    if (j.is_array() && j.size() >= 4) {
        // GLM constructor order: (w, x, y, z)
        q.w = j[0].get<float>();
        q.x = j[1].get<float>();
        q.y = j[2].get<float>();
        q.z = j[3].get<float>();
    }
}

} // namespace glm
