#ifndef CUBERENDERER_H
#define CUBERENDERER_H

#include <vector>
#include "Geometry.h"

// �������
Matrix4x4 multiplyMatrix(const Matrix4x4& a, const Matrix4x4& b);

// ������Ծ���
Vertex transformVertex(const Vertex& v, const Matrix4x4& m);

// ��ת����
Matrix4x4 createRotationMatrix(float angleX, float angleY, float angleZ);

// ͸��ͶӰ����
Matrix4x4 createProjectionMatrix(float d);

// �任���㣨ģ������->�������꣩���洢��������
void transformVertices(const std::vector<Vertex>& vertices,
    const Matrix4x4& transform,
    const Matrix4x4& projection,
    std::vector<Vertex>& transformedVertices);

// ͶӰ���㣨��������->ͶӰ���꣩���洢ͶӰ����
void projectVertices(const std::vector<Vertex>& vertices,
    const Matrix4x4& transform,
    const Matrix4x4& projection,
    std::vector<Vertex>& transformedVertices);

#endif // CUBERENDERER_H
