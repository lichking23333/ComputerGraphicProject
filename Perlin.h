#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include <vector>

class PerlinNoise {
public:
    PerlinNoise();

    // ��������
    float noise(float x, float y);

    // ���ذ�������
    float fractalNoise(float x, float y, int octaves, float persistence);

private:
    static const int permutationTableSize = 256;

    // �û���
    std::vector<int> p;

    // �ݶȼ���
    float grad(int hash, float x, float y);

    // ���Բ�ֵ
    float lerp(float t, float a, float b);

    // ƽ����ֵ
    float fade(float t);
};

#endif
