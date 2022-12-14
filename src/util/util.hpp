#pragma once
#include <util/ranges.hpp>

namespace util{
template<typename T>
inline T align(T size, T alignment){
    return (size + alignment - 1) / alignment * alignment;
}

template<typename T>
inline T normalize_val_for_range(const T& val, const T& min, const T& max){
    return (val - min) / (max - min);
}

inline bool point_in_box(const ImVec2& point, const ImVec2& a, const ImVec2& b){
    return point.x > a.x && point.x < b.x && point.y > a.y && point.y < b.y;
}

inline ImU32 vec4_to_imu32(const ImVec4& v){
    return IM_COL32(v.x * 255, v.y * 255, v.z * 255, v.w * 255);
}

inline ImVec2 vec2_add_vec2(const ImVec2& a, const ImVec2& b){
    return {a.x + b.x, a.y + b.y};
}
}