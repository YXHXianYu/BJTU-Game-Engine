/**
 * This file defines hash functions for some library classes.
 */

#include <glm/vec3.hpp>
#include <unordered_map>

namespace std {

template <> struct hash<glm::vec3> {
    size_t operator()(const glm::vec3& v) const {
        std::hash<float> hasher;
        auto             hashX = hasher(v.x);
        auto             hashY = hasher(v.y);
        auto             hashZ = hasher(v.z);

        return hashX ^ (hashY << 1) ^ (hashZ << 2);
    }
};

} // namespace std