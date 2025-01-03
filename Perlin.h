#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include <vector>

class PerlinNoise {
public:
    PerlinNoise();

    // 柏林噪声
    float noise(float x, float y);

    // 多重柏林噪声
    float fractalNoise(float x, float y, int octaves, float persistence);

private:
    static const int permutationTableSize = 256;

    // 置换表
    std::vector<int> p;

    // 梯度计算
    float grad(int hash, float x, float y);

    // 线性插值
    float lerp(float t, float a, float b);

    // 平滑插值
    float fade(float t);
};

#endif
