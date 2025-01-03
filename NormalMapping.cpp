// NormalMapping.cpp
#include "NormalMapping.h"
#include "Shading.h"
#include <cmath>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <algorithm>

#define M_PI 3.14159265358979323846

// 从文件中读取法线贴图并存储到 ImageData
ImageData loadNormalMap(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open normal map file: " + filename);
    }

    // BMP文件头解析
    char header[54];
    file.read(header, 54);

    int width = *reinterpret_cast<int*>(&header[18]);
    int height = *reinterpret_cast<int*>(&header[22]);

    std::vector<COLORREF> pixels(width * height);

    int rowPadding = (4 - (width * 3) % 4) % 4;
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            unsigned char b, g, r;
            file.read(reinterpret_cast<char*>(&b), 1);
            file.read(reinterpret_cast<char*>(&g), 1);
            file.read(reinterpret_cast<char*>(&r), 1);
            pixels[y * width + x] = RGB(r, g, b);
        }
        file.ignore(rowPadding);
    }

    file.close();

    return { width, height, std::move(pixels) };
}

Vertex calculateCentroid(const std::vector<Vertex>& vertices) {
    Vertex centroid = { 0.0f, 0.0f, 0.0f };
    int count = vertices.size();

    for (const auto& vertex : vertices) {
        centroid.x += vertex.x;
        centroid.y += vertex.y;
        centroid.z += vertex.z;
    }

    centroid.x /= count;
    centroid.y /= count;
    centroid.z /= count;

    return centroid;
}

// 球面映射UV坐标
void calculateSphericalUV(Vertex& vertex, const Vertex& centroid) {

    float x = vertex.x - centroid.x;
    float y = vertex.y - centroid.y;
    float z = vertex.z - centroid.z;

    float length = std::sqrt(x * x + y * y + z * z);
    float nx = x / length;
    float ny = y / length;
    float nz = z / length;

    float theta = std::atan2(nz, nx); 
    float phi = std::acos(ny);

    vertex.u = (theta + M_PI) / (2.0f * M_PI);
    vertex.v = phi / M_PI;
}



COLORREF GetPixel(const ImageData& image, int x, int y) {
    // 检查坐标是否超出边界
    if (x < 0 || x >= image.width || y < 0 || y >= image.height) {
        return RGB(0, 0, 0);
    }

    return image.pixels[y * image.width + x];
}

// 法线贴图采样
Vertex NormalMapping::sampleNormalFromNormalMap(const ImageData& normalMap, float u, float v) {
    int texX = static_cast<int>(u * normalMap.width);
    int texY = static_cast<int>(v * normalMap.height);

    COLORREF normalColor = GetPixel(normalMap, texX, texY);

    float nx = (GetRValue(normalColor) / 255.0f) * 2.0f - 1.0f;
    float ny = (GetGValue(normalColor) / 255.0f) * 2.0f - 1.0f;
    float nz = (GetBValue(normalColor) / 255.0f) * 2.0f - 1.0f;

    return normalize({ nx, ny, nz });
}

// 计算切线和副切线
std::vector<Vertex> calculateTangentAndBitangent(
    const Vertex& v1, const Vertex& v2, const Vertex& v3
) {
    Vertex deltaPos1 = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
    Vertex deltaPos2 = { v3.x - v1.x, v3.y - v1.y, v3.z - v1.z };

    float deltaU1 = v2.u - v1.u;
    float deltaV1 = v2.v - v1.v;
    float deltaU2 = v3.u - v1.u;
    float deltaV2 = v3.v - v1.v;

    float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

    Vertex tangent = {
        f * (deltaV2 * deltaPos1.x - deltaV1 * deltaPos2.x),
        f * (deltaV2 * deltaPos1.y - deltaV1 * deltaPos2.y),
        f * (deltaV2 * deltaPos1.z - deltaV1 * deltaPos2.z)
    };

    Vertex bitangent = {
        f * (-deltaU2 * deltaPos1.x + deltaU1 * deltaPos2.x),
        f * (-deltaU2 * deltaPos1.y + deltaU1 * deltaPos2.y),
        f * (-deltaU2 * deltaPos1.z + deltaU1 * deltaPos2.z)
    };

    tangent = normalize(tangent);
    bitangent = normalize(bitangent);

    return { tangent, bitangent };
}


Vertex transformNormalToWorldSpace(
    const Vertex& normalMapNormal, // 切空间法线
    const Vertex& tangent,        // 切线
    const Vertex& bitangent,      // 副切线
    const Vertex& normal          // 法线
) {
    return normalize({
        normalMapNormal.x * tangent.x + normalMapNormal.y * bitangent.x + normalMapNormal.z * normal.x,
        normalMapNormal.x * tangent.y + normalMapNormal.y * bitangent.y + normalMapNormal.z * normal.y,
        normalMapNormal.x * tangent.z + normalMapNormal.y * bitangent.z + normalMapNormal.z * normal.z
        });
}

float dot(const Vertex& a, const Vertex& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}


Vertex calculateFaceNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    Vertex edge1 = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
    Vertex edge2 = { v3.x - v2.x, v3.y - v2.y, v3.z - v2.z };
    Vertex normal = {
        edge1.y * edge2.z - edge1.z * edge2.y,
        edge1.z * edge2.x - edge1.x * edge2.z,
        edge1.x * edge2.y - edge1.y * edge2.x
    };
    return normalize(normal);
}

// 分割三角形
std::vector<std::vector<int>> splitPolygonIntoTriangles(const Face& face) {
    std::vector<std::vector<int>> triangles;

    int vertexCount = face.vertexIndices.size();

    if (vertexCount < 3) {
        throw std::invalid_argument("The polygon must have at least 3 vertices.");
    }

    const int v0 = face.vertexIndices[0]; 
    for (int i = 1; i < vertexCount - 1; ++i) {
        const int v1 = face.vertexIndices[i];
        const int v2 = face.vertexIndices[i + 1];

        std::vector<int> triangle = { v0, v1, v2 };
        triangles.push_back(triangle);
    }

    return triangles;
}

float calculateArea(const Vertex& A, const Vertex& B, const Vertex& C) {
    return std::abs(A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)) / 2.0f;
}

bool isPointInTriangle(float x, float y, const Vertex& A, const Vertex& B, const Vertex& C, float& alpha, float& beta, float& gamma) {

    float areaABC = calculateArea(A, B, C);

    float areaPBC = calculateArea({ x, y }, B, C);
    float areaPCA = calculateArea(A, { x, y }, C);
    float areaPAB = calculateArea(A, B, { x, y });

    alpha = areaPBC / areaABC;
    beta = areaPCA / areaABC;
    gamma = areaPAB / areaABC;

	// 判断点是否在三角形内，1e-6会有bug，所以改成1e-3
    return (alpha >= 0 && beta >= 0 && gamma >= 0) && (std::abs(alpha + beta + gamma - 1.0f) < 1e-3);
}

void drawTriangleWithNormals(const std::vector<Vertex>& vertices, const std::vector<Vertex>& projectedVertices, const std::vector<int>& triangle, ImageData normalMap, const PhongShadingLight& phongShadingLight) {

    const Vertex& v1 = vertices[triangle[0]];
    const Vertex& v2 = vertices[triangle[1]];
    const Vertex& v3 = vertices[triangle[2]];

    Vertex faceNormal = calculateFaceNormal(v1, v2, v3);
    // Back-face culling
    Vertex viewVector = { 0.0f, 0.0f, 1.0f };
    float dotProduct = faceNormal.x * viewVector.x + faceNormal.y * viewVector.y + faceNormal.z * viewVector.z;
    if (dotProduct > 0) {
        return;
    }

    std::vector<Vertex> tangentAndBitangent = calculateTangentAndBitangent(v1, v2, v3);

    const Vertex& projectedV1 = projectedVertices[triangle[0]];
	const Vertex& projectedV2 = projectedVertices[triangle[1]];
	const Vertex& projectedV3 = projectedVertices[triangle[2]];

	// 以三角形包围盒为界限,在屏幕上绘制三角形内的像素点
    int xmin = (std::min)({ projectedV1.x, projectedV2.x, projectedV3.x });
    int xmax = (std::max)({ projectedV1.x, projectedV2.x, projectedV3.x });
    int ymin = (std::min)({ projectedV1.y, projectedV2.y, projectedV3.y });
    int ymax = (std::max)({ projectedV1.y, projectedV2.y, projectedV3.y });

    for (int yScreen = ymin; yScreen <= ymax; ++yScreen) {
        for (int xScreen = xmin; xScreen <= xmax; ++xScreen) {

            float alpha, beta, gamma;

            if (isPointInTriangle(xScreen, yScreen, projectedV1, projectedV2, projectedV3, alpha, beta, gamma)) {
                float x, y, z, u, v;
				x = v1.x * alpha + v2.x * beta + v3.x * gamma;
				y = v1.y * alpha + v2.y * beta + v3.y * gamma;
				z = v1.z * alpha + v2.z * beta + v3.z * gamma;
                u = v1.u * alpha + v2.u * beta + v3.u * gamma;
                v = v1.v * alpha + v2.v * beta + v3.v * gamma;
                Vertex tempTextureNormal = NormalMapping::sampleNormalFromNormalMap(normalMap, u, v);
                Vertex tempNormal = transformNormalToWorldSpace(tempTextureNormal, tangentAndBitangent[0], tangentAndBitangent[1], faceNormal);
                float intensity = calculatePhongIntensity(x, y, z, tempNormal, phongShadingLight);
                //float intensity_ref = calculatePhongIntensity(x, y, z, faceNormal, phongShadingLight);
                COLORREF color = RGB(intensity * 255, intensity * 255, intensity * 255);
                putpixel(xScreen, yScreen, color);
            }
        }
    }
}


void NormalMappingShading(std::vector<Vertex>& vertices, std::vector<Vertex>& projectedVertices, const std::vector<Face>& faces, const int width, const int height, const PhongShadingLight& phongShadingLight) {

    std::vector<Vertex> normals(vertices.size());
    Vertex Centroid = calculateCentroid(vertices);
	ImageData normalMap = loadNormalMap("normal_map_3.bmp");

	for (size_t i = 0; i < vertices.size(); i++) {
		//Vertex vertex = vertices[i];
		calculateSphericalUV(vertices[i], Centroid);
		projectedVertices[i].u = vertices[i].u;
		projectedVertices[i].v = vertices[i].v;
	}

    for (size_t i = 0; i < faces.size(); ++i) {
        const Face& face = faces[i];

        std::vector<std::vector<int>> Triangles = splitPolygonIntoTriangles(face);

        for (size_t i = 0; i < Triangles.size(); ++i) {
			drawTriangleWithNormals(vertices, projectedVertices, Triangles[i], normalMap, phongShadingLight);
        }

    }
}

