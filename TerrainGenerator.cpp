#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>  // 用于控制输出格式
#include "Perlin.h"
#include "TerrainGenerator.h"

// Minecraft-like 地形生成器，生成结果保存为 OBJ 文件

float TerrainGenerator::getNoise(float x, float y) {

    PerlinNoise perlinNoise;
    float scaleFactor = 0.1f;
    float scaledX = x * scaleFactor;
    float scaledY = y * scaleFactor;

    int octaves = 6;
    float persistence = 0.5f;
    float fractalNoiseValue = perlinNoise.fractalNoise(scaledX, scaledY, octaves, persistence);

    /*
    // 设置网格的大小
    int width = 100;  // 网格宽度
    int height = 100;  // 网格高度


    // 生成并输出噪声点阵
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // 获取每个点的噪声值
            float noiseValue = perlinNoise.fractalNoise(static_cast<float>(x), static_cast<float>(y), octaves, persistence);

            // 输出噪声值，控制输出格式为0到1之间
            std::cout << std::fixed << std::setprecision(2) << noiseValue << " ";
        }
        std::cout << std::endl;  // 换行
    }
	system("pause");
    */

    return fractalNoiseValue;
}

TerrainGenerator::TerrainGenerator(int length, int width, int height)
    : m_length(length), m_width(width), m_height(height) {
    // 初始化随机种子
    srand(static_cast<unsigned int>(time(0)));

    // 初始化 m_terrain，所有位置的初始状态为 0 (表示为空地)
    m_terrain.resize(m_length, std::vector<std::vector<int>>(m_width, std::vector<int>(m_height, 0)));
}

void TerrainGenerator::generateTerrain() {
    // 遍历整个区块的每一个位置
    for (int x = 0; x < m_length; ++x) {
        for (int y = 0; y < m_width; ++y) {
            // 使用噪声函数计算该位置的高度（在 [0, m_height] 范围内）
            int height = static_cast<int>(getNoise(static_cast<float>(x), static_cast<float>(y)) * m_height);

            // 填充该位置的高度信息
            for (int z = 0; z < height; ++z) {
                m_terrain[x][y][z] = 1;  // 1 表示该位置为“实心地面” (你可以根据需求调整)
            }

            // 可以添加更多的条件来生成不同类型的方块
            // 例如，我们可以在某些位置生成水或者树木
            if (height > m_height * 0.8) {
                // 高度较高的地方生成岩石
                for (int z = height; z < m_height; ++z) {
                    m_terrain[x][y][z] = 2;  // 2 表示岩石
                }
            }
        }
    }
}

// 将地形数据转 OBJ 
void TerrainGenerator::writeOBJ(const std::string& filename) {
    std::ofstream objFile(filename);

    if (!objFile.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return;
    }

    int vertexIndex = 1;

    for (int x = 0; x < m_length; ++x) {
        for (int y = 0; y < m_width; ++y) {
            for (int z = 0; z < m_height; ++z) {
                if (m_terrain[x][y][z] != 0) { 
                    float x0 = x, y0 = y, z0 = z;
                    float x1 = x + 1, y1 = y + 1, z1 = z + 1;

                    std::vector<std::vector<float>> vertices = {
                        {x0, y0, z0}, {x1, y0, z0}, {x1, y1, z0}, {x0, y1, z0},
                        {x0, y0, z1}, {x1, y0, z1}, {x1, y1, z1}, {x0, y1, z1}
                    };

                    for (auto& vertex : vertices) {
                        objFile << "v " << vertex[0] << " " << vertex[1] << " " << vertex[2] << std::endl;
                    }

                    std::vector<std::vector<int>> faces = {
                        {1, 4, 3, 2}, {5, 6, 7, 8}, // 上下面
                        {1, 2, 6, 5}, {2, 3, 7, 6}, // 前后面
                        {3, 4, 8, 7}, {4, 1, 5, 8}  // 左右面
                    };

                    for (auto& face : faces) {
                        objFile << "f " << (vertexIndex + face[0] - 1)
                            << " " << (vertexIndex + face[1] - 1)
                            << " " << (vertexIndex + face[2] - 1)
                            << " " << (vertexIndex + face[3] - 1)
                            << std::endl;
                    }

                    vertexIndex += 8;
                }
            }
        }
    }

    // 关闭文件
    objFile.close();

    std::cout << "OBJ file has been written successfully: " << filename << std::endl;
}
