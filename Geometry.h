#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

const float INF = FLT_MAX;

// 4*4矩阵
struct Matrix4x4 {
    float m[4][4] = { 0 };
};

// 顶点（顶点坐标和纹理坐标）
struct Vertex {
    float x, y, z;
    float u, v;
};

// 面的顶点索引
struct Face {
    std::vector<int> vertexIndices; 
};

// scanline 扫描时交点
struct Intersection {
    float x;
    float y;
    float z;
    float xWorld;
	float yWorld;
	float zWorld;
    float intensity;
	Vertex normal; // 用于phongshading的插值法线
};

struct PhongShadingLight {
    Vertex position;   // 光源位置
    float ambient;     // 环境光强度
    float diffuse;     // 漫反射光强度
    float specular;    // 镜面反射光强度
};


#endif // GEOMETRY_H
#pragma once
