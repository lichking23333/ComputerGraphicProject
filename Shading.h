#ifndef SHADING_H
#define SHADING_H

#include "Geometry.h"
#include <graphics.h>
#include <vector>

// 光照方向
struct Light {
    float x, y, z;
};

// 计算法向量
Vertex calculateNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3);

float calculatePhongIntensity(float worldX, float worldY, float worldZ, const Vertex& rawNormal, const PhongShadingLight& phongShadingLight);

// Constant Shading
void constantShading(const std::vector<Vertex>& vertices, const std::vector<Vertex>& projectedVertices, std::vector<Face>& faces, const int width, const int height, const Light& light);

// Gouraud Shading
void gouraudShading(const std::vector<Vertex>& vertices, const std::vector<Vertex>& projectedVertices, const std::vector<Face>& faces, const int width, const int height, const Light& light);

// Phong Shading
void phongShading(const std::vector<Vertex>& vertices, const std::vector<Vertex>& projectedVertices, const std::vector<Face>& faces, const int width, const int height, const PhongShadingLight& phongShadingLight);

#endif // SHADING_H
