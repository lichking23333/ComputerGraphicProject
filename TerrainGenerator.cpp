#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>  // ���ڿ��������ʽ
#include "Perlin.h"
#include "TerrainGenerator.h"

// Minecraft-like ���������������ɽ������Ϊ OBJ �ļ�

float TerrainGenerator::getNoise(float x, float y) {

    PerlinNoise perlinNoise;
    float scaleFactor = 0.1f;
    float scaledX = x * scaleFactor;
    float scaledY = y * scaleFactor;

    int octaves = 6;
    float persistence = 0.5f;
    float fractalNoiseValue = perlinNoise.fractalNoise(scaledX, scaledY, octaves, persistence);

    /*
    // ��������Ĵ�С
    int width = 100;  // ������
    int height = 100;  // ����߶�


    // ���ɲ������������
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // ��ȡÿ���������ֵ
            float noiseValue = perlinNoise.fractalNoise(static_cast<float>(x), static_cast<float>(y), octaves, persistence);

            // �������ֵ�����������ʽΪ0��1֮��
            std::cout << std::fixed << std::setprecision(2) << noiseValue << " ";
        }
        std::cout << std::endl;  // ����
    }
	system("pause");
    */

    return fractalNoiseValue;
}

TerrainGenerator::TerrainGenerator(int length, int width, int height)
    : m_length(length), m_width(width), m_height(height) {
    // ��ʼ���������
    srand(static_cast<unsigned int>(time(0)));

    // ��ʼ�� m_terrain������λ�õĳ�ʼ״̬Ϊ 0 (��ʾΪ�յ�)
    m_terrain.resize(m_length, std::vector<std::vector<int>>(m_width, std::vector<int>(m_height, 0)));
}

void TerrainGenerator::generateTerrain() {
    // �������������ÿһ��λ��
    for (int x = 0; x < m_length; ++x) {
        for (int y = 0; y < m_width; ++y) {
            // ʹ���������������λ�õĸ߶ȣ��� [0, m_height] ��Χ�ڣ�
            int height = static_cast<int>(getNoise(static_cast<float>(x), static_cast<float>(y)) * m_height);

            // ����λ�õĸ߶���Ϣ
            for (int z = 0; z < height; ++z) {
                m_terrain[x][y][z] = 1;  // 1 ��ʾ��λ��Ϊ��ʵ�ĵ��桱 (����Ը����������)
            }

            // ������Ӹ�������������ɲ�ͬ���͵ķ���
            // ���磬���ǿ�����ĳЩλ������ˮ������ľ
            if (height > m_height * 0.8) {
                // �߶Ƚϸߵĵط�������ʯ
                for (int z = height; z < m_height; ++z) {
                    m_terrain[x][y][z] = 2;  // 2 ��ʾ��ʯ
                }
            }
        }
    }
}

// ����������ת OBJ 
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
                        {1, 4, 3, 2}, {5, 6, 7, 8}, // ������
                        {1, 2, 6, 5}, {2, 3, 7, 6}, // ǰ����
                        {3, 4, 8, 7}, {4, 1, 5, 8}  // ������
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

    // �ر��ļ�
    objFile.close();

    std::cout << "OBJ file has been written successfully: " << filename << std::endl;
}
