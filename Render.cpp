#include "Renderer.h"
#include <graphics.h>
#include <cmath>
#include <vector>
using namespace std;

Matrix4x4 multiplyMatrix(const Matrix4x4& a, const Matrix4x4& b) {
    Matrix4x4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}

Vertex transformVertex(const Vertex& v, const Matrix4x4& m) {
    float x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];
    float y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];
    float z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];
    float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];
    if (w != 0) {
        x /= w;
        y /= w;
        z /= w;
    }
    return { x, y, z };
}

Matrix4x4 createRotationMatrix(float angleX, float angleY, float angleZ) {
    Matrix4x4 rx = {
        1, 0, 0, 0,
        0, cos(angleX), -sin(angleX), 0,
        0, sin(angleX), cos(angleX), 0,
        0, 0, 0, 1
    };
    Matrix4x4 ry = {
        cos(angleY), 0, sin(angleY), 0,
        0, 1, 0, 0,
        -sin(angleY), 0, cos(angleY), 0,
        0, 0, 0, 1
    };
    Matrix4x4 rz = {
        cos(angleZ), -sin(angleZ), 0, 0,
        sin(angleZ), cos(angleZ), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    return multiplyMatrix(multiplyMatrix(rx, ry), rz);
}

Matrix4x4 createProjectionMatrix(float d) {
    return {
        d, 0, 0, 0,
        0, d, 0, 0,
        0, 0, 1, 0,
        0, 0, 1, 0
    };
}

void transformVertices(const vector<Vertex>& vertices,
    const Matrix4x4& transform,
    const Matrix4x4& projection,
    vector<Vertex>& transformedVertices) {
    transformedVertices.clear();
    for (const auto& v : vertices) {
		Vertex t = transformVertex(v, transform);  // 变换矩阵（模型坐标 -> 世界坐标）
        //t = transformVertex(t, projection);       // 投影矩阵
        transformedVertices.push_back(t);
    }
}

void projectVertices(const vector<Vertex>& vertices,
    const Matrix4x4& transform,
    const Matrix4x4& projection,
    vector<Vertex>& projectedVertices) {
    projectedVertices.clear();
    for (const auto& v : vertices) {
        Vertex t = transformVertex(v, projection);
        projectedVertices.push_back(t);
    }
}

//void drawModel(const vector<Vertex>& vertices, const vector<Face>& faces) {
//    for (const auto& face : faces) {
//        for (size_t i = 0; i < face.vertexIndices.size(); ++i) {
//            int startIdx = face.vertexIndices[i];
//            int endIdx = face.vertexIndices[(i + 1) % face.vertexIndices.size()];
//
//            int x1 = static_cast<int>(vertices[startIdx].x * 100 + 200);
//            int y1 = static_cast<int>(vertices[startIdx].y * 100 + 200);
//            int x2 = static_cast<int>(vertices[endIdx].x * 100 + 200);
//            int y2 = static_cast<int>(vertices[endIdx].y * 100 + 200);
//
//            line(x1, y1, x2, y2);
//        }
//    }
//}

// 绘制立方体
//void drawCube(const Matrix4x4& transform, const Matrix4x4& projection) {
//    vector<Vertex> transformedVertices;
//    for (const auto& v : vertices) {
//        auto t = transformVertex(v, transform);  // 应用变换矩阵
//        t = transformVertex(t, projection);     // 应用投影矩阵
//        transformedVertices.push_back(t);
//    }
//
//    for (const auto& edge : edges) {
//        int x1 = static_cast<int>(transformedVertices[edge.first].x * 100 + 400);
//        int y1 = static_cast<int>(transformedVertices[edge.first].y * 100 + 300);
//        int x2 = static_cast<int>(transformedVertices[edge.second].x * 100 + 400);
//        int y2 = static_cast<int>(transformedVertices[edge.second].y * 100 + 300);
//        line(x1, y1, x2, y2);
//    }
//}
