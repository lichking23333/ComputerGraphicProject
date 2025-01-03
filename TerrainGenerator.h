#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H

#include <vector>
#include <string>
#include "Geometry.h"  // 假设 Vertex 已经定义，用于表示三维向量
#include "Perlin.h"

// TerrainGenerator 类负责生成地形并导出为 OBJ 文件
class TerrainGenerator {
public:
    // 构造函数
    TerrainGenerator(int length, int width, int height);

    // 生成地形
    void generateTerrain();

    // 将导出为 OBJ 文件
    void writeOBJ(const std::string& filename);

private:
    int m_length;   // 区块的长度 (沿 x 轴)
    int m_width;    // 区块的宽度 (沿 y 轴)
    int m_height;   // 区块的高度 (沿 z 轴)

    // 存储地形数据（每个位置的高度）
    std::vector<std::vector<std::vector<int>>> m_terrain;

    // 存储地形顶点数据
    std::vector<Vertex> m_vertices;

    // 简单噪声函数生成高度值
    float getNoise(float x, float y);
};

#endif  // TERRAIN_GENERATOR_H
