// NormalMapping.h
#ifndef NORMAL_MAPPING_H
#define NORMAL_MAPPING_H

#include <graphics.h>
#include <string>
#include <cmath>
#include "Geometry.h"  // 顶点结构体的定义

// 法线贴图图像数据
struct ImageData {
    int width;
    int height;
    std::vector<COLORREF> pixels;
};

void NormalMappingShading(const std::vector<Vertex>& transformedVertices,
    const std::vector<Vertex>& projectedVertices,
    const std::vector<Face>& faces,
    int width, int height,
    const PhongShadingLight& light);

float dot(const Vertex& a, const Vertex& b);

Vertex normalize(const Vertex& v);

class NormalMapping {
public:
    // 法线贴图采样
    static Vertex sampleNormalFromNormalMap(const ImageData& normalMap, float u, float v);
};

COLORREF GetPixel(const ImageData& image, int x, int y);


#endif // NORMAL_MAPPING_H
