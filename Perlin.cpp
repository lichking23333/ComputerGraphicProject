#include "Perlin.h"
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <stdio.h>

PerlinNoise::PerlinNoise() {
    p.resize(permutationTableSize * 2);

    std::vector<int> temp(p.begin(), p.begin() + permutationTableSize);
    for (int i = 0; i < permutationTableSize; ++i) {
        temp[i] = i; 
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(temp.begin(), temp.end(), g);

    std::copy(temp.begin(), temp.end(), p.begin());
    std::copy(temp.begin(), temp.end(), p.begin() + permutationTableSize);
}

float PerlinNoise::grad(int hash, float x, float y) {
    int h = hash & 15;  
    float u = h < 8 ? x : y;  
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0.0f); 
    return (h & 1 ? -1.0f : 1.0f) * (u + v);  
}

float PerlinNoise::lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float PerlinNoise::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::noise(float x, float y) {

    int X = static_cast<int>(floor(x)) & 255;
    int Y = static_cast<int>(floor(y)) & 255;

    x -= floor(x);
    y -= floor(y);

    float u = fade(x);
    float v = fade(y);
    
	//std::printf("X: %d, Y: %d\n", X, Y);
    int A = p[X] + Y;
    int B = p[X + 1] + Y;
    int AA = p[A];
    int AB = p[A + 1];
    int BA = p[B];
    int BB = p[B + 1];

    float res = lerp(v, lerp(u, grad(p[AA], x, y), grad(p[BA], x - 1, y)),
        lerp(u, grad(p[AB], x, y - 1), grad(p[BB], x - 1, y - 1)));

    return (res + 1.0f) * 0.5f;
}

float PerlinNoise::fractalNoise(float x, float y, int octaves, float persistence) {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxAmplitude = 0.0f; 

    for (int i = 0; i < octaves; ++i) {
        total += noise(x * frequency, y * frequency) * amplitude;
        frequency *= 1.5f;
        maxAmplitude += amplitude;
        amplitude *= persistence;
    }

    return total / maxAmplitude;
}
