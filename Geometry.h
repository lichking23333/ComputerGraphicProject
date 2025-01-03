#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

const float INF = FLT_MAX;

// 4*4����
struct Matrix4x4 {
    float m[4][4] = { 0 };
};

// ���㣨����������������꣩
struct Vertex {
    float x, y, z;
    float u, v;
};

// ��Ķ�������
struct Face {
    std::vector<int> vertexIndices; 
};

// scanline ɨ��ʱ����
struct Intersection {
    float x;
    float y;
    float z;
    float xWorld;
	float yWorld;
	float zWorld;
    float intensity;
	Vertex normal; // ����phongshading�Ĳ�ֵ����
};

struct PhongShadingLight {
    Vertex position;   // ��Դλ��
    float ambient;     // ������ǿ��
    float diffuse;     // �������ǿ��
    float specular;    // ���淴���ǿ��
};


#endif // GEOMETRY_H
#pragma once
