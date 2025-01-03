#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include <vector>
#include <string>
#include "Geometry.h"  // ���� Vertex �Ѿ����壬���ڱ�ʾ��ά����
#include "Perlin.h"

// TerrainGenerator �ฺ�����ɵ��β�����Ϊ OBJ �ļ�
class TerrainGenerator {
public:
    // ���캯��
    TerrainGenerator(int length, int width, int height);

    // ���ɵ���
    void generateTerrain();

    // ������Ϊ OBJ �ļ�
    void writeOBJ(const std::string& filename);

private:
    int m_length;   // ����ĳ��� (�� x ��)
    int m_width;    // ����Ŀ�� (�� y ��)
    int m_height;   // ����ĸ߶� (�� z ��)

    // �洢�������ݣ�ÿ��λ�õĸ߶ȣ�
    std::vector<std::vector<std::vector<int>>> m_terrain;

    // �洢���ζ�������
    std::vector<Vertex> m_vertices;

    // �������������ɸ߶�ֵ
    float getNoise(float x, float y);
};

#endif  // TERRAIN_GENERATOR_H
