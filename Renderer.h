#ifndef CUBERENDERER_H
#define CUBERENDERER_H

#include <vector>
#include "Geometry.h"

// 矩阵相乘
Matrix4x4 multiplyMatrix(const Matrix4x4& a, const Matrix4x4& b);

// 顶点乘以矩阵
Vertex transformVertex(const Vertex& v, const Matrix4x4& m);

// 旋转矩阵
Matrix4x4 createRotationMatrix(float angleX, float angleY, float angleZ);

// 透视投影矩阵
Matrix4x4 createProjectionMatrix(float d);

// 变换顶点（模型坐标->世界坐标）并存储世界坐标
void transformVertices(const std::vector<Vertex>& vertices,
    const Matrix4x4& transform,
    const Matrix4x4& projection,
    std::vector<Vertex>& transformedVertices);

// 投影顶点（世界坐标->投影坐标）并存储投影坐标
void projectVertices(const std::vector<Vertex>& vertices,
    const Matrix4x4& transform,
    const Matrix4x4& projection,
    std::vector<Vertex>& transformedVertices);

#endif // CUBERENDERER_H
