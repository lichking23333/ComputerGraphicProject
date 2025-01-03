#include "Shading.h"
#include <cmath>
#include <algorithm>
#include <limits>

std::vector<std::vector<float>> zBuffer;

float vectorMagnitude(const Vertex& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vertex normalize(const Vertex& v) {
    float mag = vectorMagnitude(v);
    return { v.x / mag, v.y / mag, v.z / mag, v.u, v.v };
}

Vertex calculateNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    Vertex edge1 = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
    Vertex edge2 = { v3.x - v2.x, v3.y - v2.y, v3.z - v2.z };
    Vertex normal = {
        edge1.y * edge2.z - edge1.z * edge2.y,
        edge1.z * edge2.x - edge1.x * edge2.z,
        edge1.x * edge2.y - edge1.y * edge2.x
    };
    return normalize(normal);
}

float calculateIntensity(const Vertex& normal, const Light& light) {
    Vertex normalizedLight = normalize({ light.x, light.y, light.z });
    return max(0.0f, 
        normal.x * normalizedLight.x +
        normal.y * normalizedLight.y +
        normal.z * normalizedLight.z);
}

float calculateCentroidZ(const std::vector<Vertex>& vertices, const Face& face) {

    int numVertices = face.vertexIndices.size();
	float sum = 0.0f;
	for (int i = 0; i < numVertices; ++i) {
		const Vertex& v = vertices[face.vertexIndices[i]];
		sum += v.z;
	}

    return sum;
}

// 计算面的重心z坐标并排序
void sortFacesByCentroidZ(std::vector<Face>& faces, const std::vector<Vertex>& vertices) {
    std::sort(faces.begin(), faces.end(), [&vertices](const Face& a, const Face& b) {
        float centroidY_a = calculateCentroidZ(vertices, a);
        float centroidY_b = calculateCentroidZ(vertices, b);

        return centroidY_a < centroidY_b;
        });
}

// Constant Shading
void constantShading(const std::vector<Vertex>& vertices, const std::vector<Vertex>& projectedVertices, std::vector<Face>& faces, const int width, const int height, const Light& light) {

    sortFacesByCentroidZ(faces, vertices);

    for(const auto& face : faces) {
        const Vertex& v1 = vertices[face.vertexIndices[0]];
        const Vertex& v2 = vertices[face.vertexIndices[1]];
        const Vertex& v3 = vertices[face.vertexIndices[2]];

        Vertex normal = calculateNormal(v1, v2, v3);
        float intensity = calculateIntensity(normal, light);

		// Back-face culling
        Vertex viewVector = { 0.0f, 0.0f, 1.0f };
        float dotProduct = normal.x * viewVector.x + normal.y * viewVector.y + normal.z * viewVector.z;
        if (dotProduct < 0) {
            // 面朝向屏幕外，剔除该面
            continue;
        }

        int numVertices = face.vertexIndices.size();

        std::vector<POINT> points(numVertices);
        std::vector<float> depths(numVertices);

        // 投影坐标转换为屏幕坐标
        for (int i = 0; i < numVertices; ++i) {
            const Vertex& v = projectedVertices[face.vertexIndices[i]];
            points[i] = { static_cast<int>(v.x), static_cast<int>(v.y) };
            depths[i] = v.z;
        }

        COLORREF color = RGB(intensity * 255, intensity * 255, intensity * 255);

        setfillcolor(color);
        setfillstyle(BS_SOLID);
        fillpolygon(points.data(), numVertices);
    }
}

// 判断边 (p1, p2) 是否与扫描线 y = scanLineY 相交，并计算交点的 x 坐标
float calculateIntersectionX(const Vertex& p1, const Vertex& p2, float scanLineY) {
    if ((scanLineY >= p1.y && scanLineY < p2.y) || (scanLineY >= p2.y && scanLineY < p1.y)) {
        float t = (scanLineY - p1.y) / (p2.y - p1.y); // 线性插值因子
        return p1.x + t * (p2.x - p1.x); 
    }
    return std::numeric_limits<float>::quiet_NaN(); // 无交点
}

Vertex calculateIntersection(const Vertex& p1, const Vertex& p2, float scanLineY) {
    if ((scanLineY >= p1.y && scanLineY < p2.y) || (scanLineY >= p2.y && scanLineY < p1.y)) {
        float t = (scanLineY - p1.y) / (p2.y - p1.y); // 线性插值因子
		return { p1.x + t * (p2.x - p1.x),p1.y + t * (p2.y - p1.y),p1.z + t * (p2.z - p1.z) };
    }
	return { std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN() }; // 无交点
}

//找扫描线和多边形的交点
void findIntersections(const std::vector<Vertex>& projectedVertices,
    const std::vector<Vertex>& worldVertices,
    const std::vector<Face>& faces,
	const std::vector<Vertex>& normals,
    float scanLineY,
    std::vector<float>& intensities,
    std::vector<Intersection>& intersections) {
    intersections.clear(); 
    for (const auto& face : faces) {
        for (int i = 0; i < face.vertexIndices.size(); i++) {
            const Vertex& p1 = projectedVertices[face.vertexIndices[i]];
            const Vertex& p2 = projectedVertices[face.vertexIndices[(i + 1) % face.vertexIndices.size()]];

			Vertex interPoint = calculateIntersection(p1, p2, scanLineY);
			float x = interPoint.x;
			float y = interPoint.y;
			float z = interPoint.z;
            //float x = calculateIntersectionX(p1, p2, scanLineY);

            if (!std::isnan(x)) { 
				// 线性插值计算光强度、法线和世界坐标
                float intensity = intensities[face.vertexIndices[i]] +
                    (intensities[face.vertexIndices[(i + 1) % face.vertexIndices.size()]] -
                        intensities[face.vertexIndices[i]]) *
                    (scanLineY - p1.y) / (p2.y - p1.y);
                Vertex normal;
                normal.x = normals[face.vertexIndices[i]].x + (normals[face.vertexIndices[(i + 1) % face.vertexIndices.size()]].x - normals[face.vertexIndices[i]].x) * (scanLineY - p1.y) / (p2.y - p1.y);
				normal.y = normals[face.vertexIndices[i]].y + (normals[face.vertexIndices[(i + 1) % face.vertexIndices.size()]].y - normals[face.vertexIndices[i]].y) * (scanLineY - p1.y) / (p2.y - p1.y);
				normal.z = normals[face.vertexIndices[i]].z + (normals[face.vertexIndices[(i + 1) % face.vertexIndices.size()]].z - normals[face.vertexIndices[i]].z) * (scanLineY - p1.y) / (p2.y - p1.y);

				float xWorld = worldVertices[face.vertexIndices[i]].x + (worldVertices[face.vertexIndices[(i + 1) % face.vertexIndices.size()]].x - worldVertices[face.vertexIndices[i]].x) * (scanLineY - p1.y) / (p2.y - p1.y);
				float yWorld = worldVertices[face.vertexIndices[i]].y + (worldVertices[face.vertexIndices[(i + 1) % face.vertexIndices.size()]].y - worldVertices[face.vertexIndices[i]].y) * (scanLineY - p1.y) / (p2.y - p1.y);
				float zWorld = worldVertices[face.vertexIndices[i]].z + (worldVertices[face.vertexIndices[(i + 1) % face.vertexIndices.size()]].z - worldVertices[face.vertexIndices[i]].z) * (scanLineY - p1.y) / (p2.y - p1.y);

                normal = normalize(normal);

                intersections.push_back({ x, y, z, xWorld, yWorld, zWorld, intensity, normal});
            }
        }
    }
}

// 对交点进行排序
void sortIntersections(std::vector<Intersection>& intersections) {
    std::sort(intersections.begin(), intersections.end(),
        [](const Intersection& a, const Intersection& b) {
            return a.x < b.x;  // 按照x坐标升序排序
        });
}

// 插值法线（用于Phong Shading）
Vertex interpolateNormal(const Vertex& normal1, const Vertex& normal2, float t) {
    return { normal1.x * (1 - t) + normal2.x * t,
             normal1.y * (1 - t) + normal2.y * t,
             normal1.z * (1 - t) + normal2.z * t };
}

float dotProduct(const Vertex& a, const Vertex& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vertex reflect(const Vertex& lightDir, const Vertex& normal) {
    float dot = dotProduct(lightDir, normal);
    return { lightDir.x - 2 * dot * normal.x,
             lightDir.y - 2 * dot * normal.y,
             lightDir.z - 2 * dot * normal.z };
}


float calculatePhongIntensity(float worldX, float worldY, float worldZ, const Vertex& rawNormal, const PhongShadingLight& phongShadingLight) {
	Vertex normal = normalize(rawNormal);
    //Vertex normal = rawNormal;
    // 假设光源是点光源，计算漫反射和镜面反射光照
    Vertex lightDir = normalize({ phongShadingLight.position.x - worldX, phongShadingLight.position.y - worldY, phongShadingLight.position.z - worldZ });
    Vertex viewDir = normalize({ 0.0f, 0.0f, 1.0f });

    // 漫反射强度
    float diffuse = max(0.0f, -dotProduct(normal, lightDir));

    // 镜面反射强度
    Vertex reflectDir = reflect(lightDir, normal);
    float specular = std::pow(max(0.0f, dotProduct(viewDir, reflectDir)), 32.0f); // 高光强度

    // 总光照
    float intensity = phongShadingLight.ambient + phongShadingLight.diffuse * diffuse + phongShadingLight.specular * specular;
    intensity = max(0.0f, intensity);
    return min(intensity, 1.0f);
}

void processGouraudShadingPixel(std::vector<std::vector<float>> zBuffer, const int width, const int height, const std::vector<Vertex>& worldVertices, const std::vector<Vertex>& projectedVertices, const std::vector<Face> needToDrawFaces, const std::vector<Vertex> normals, const int y, std::vector<float> intensities) {
    //printf("Processing column %d\n", x);
    std::vector<Intersection> intersections;
    findIntersections(projectedVertices, worldVertices, needToDrawFaces, normals, y, intensities, intersections);
    sortIntersections(intersections);
    for (int i = 0; i < int(intersections.size()); i += 2) {
        if (intersections.size() == 0) {
            break;
        }
        int x1 = static_cast<int>(intersections[i].x);
        int x2 = static_cast<int>(intersections[i + 1].x);
        for (int j = x1; j <= x2; j++) {
            float t = (x1 == x2) ? 0 : ((float)j - (float)x1) / ((float)x2 - (float)x1);
            float intensity = intersections[i].intensity * (1 - t) + intersections[i + 1].intensity * t;
            float depth = intersections[i].z * (1 - t) + intersections[i + 1].z * t;
            COLORREF color = RGB(intensity * 255, intensity * 255, intensity * 255);
            //COLORREF color = RGB(255, 255, 255);
            //COLORREF color = RGB((30 * i + 20) % 256, (30 * i + 20) % 256, (30 * i + 20) % 256);
            
            // 更新 Z-buffer 和绘制像素
            if (depth < zBuffer[j + width / 2][y + height / 2]) {
                zBuffer[j + width / 2][y + height / 2] = depth;
                COLORREF color = RGB(intensity * 255, intensity * 255, intensity * 255);
                putpixel(j, y, color);
            }
        }
    }
}

// Gouraud Shading
void gouraudShading(const std::vector<Vertex>& vertices, const std::vector<Vertex>& projectedVertices, const std::vector<Face>& faces, const int width, const int height, const Light& light) {
    zBuffer = std::vector<std::vector<float>>(width, std::vector<float>(height, INF));
    std::vector<float> intensities(vertices.size());
	std::vector<Vertex> normals(vertices.size());
	std::vector<Face> needToDrawFaces;
    for (size_t i = 0; i < faces.size(); ++i) {
        const Face& face = faces[i];

        const Vertex& v1 = vertices[face.vertexIndices[0]];
        const Vertex& v2 = vertices[face.vertexIndices[1]];
        const Vertex& v3 = vertices[face.vertexIndices[2]];

        Vertex normal = calculateNormal(v1, v2, v3);

        // Back-face culling
        Vertex viewVector = { 0.0f, 0.0f, 1.0f };
        float dotProduct = normal.x * viewVector.x + normal.y * viewVector.y + normal.z * viewVector.z;
        if (dotProduct > 0) {
			needToDrawFaces.push_back(face);
        }

        int numVertices = face.vertexIndices.size();
        
        for (int i = 0; i < numVertices; i++){
            normals[face.vertexIndices[i]].x += normal.x;
			normals[face.vertexIndices[i]].y += normal.y;
			normals[face.vertexIndices[i]].z += normal.z;
        }
    }

    for (auto& normal : normals) {
        normal = normalize(normal);
    }

	for (int i = 0; i < vertices.size(); i++) {
		intensities[i] = calculateIntensity(normals[i], light);
	}

	int originX = width / 2;
	int originY = height / 2;

    for (int y = -originY; y <= height - originY; y++) {
		//printf("Processing column %d\n", x);
        std::vector<Intersection> intersections;
        findIntersections(projectedVertices, vertices, needToDrawFaces, normals, y, intensities, intersections);
        sortIntersections(intersections);
        processGouraudShadingPixel(zBuffer, width, height, vertices, projectedVertices, needToDrawFaces, normals, y, intensities);

        /*
		for (int i = 0; i < int(intersections.size()); i += 2) {
			if (intersections.size() == 0) {
				break;
			}
			int x1 = static_cast<int>(intersections[i].x);
			int x2 = static_cast<int>(intersections[i + 1].x);
			for (int j = x1; j <= x2; j++) {
				float t = (x1 == x2) ? 0 : ((float)j - (float)x1) / ((float)x2 - (float)x1);
				float intensity = intersections[i].intensity * (1 - t) + intersections[i + 1].intensity * t;
				COLORREF color = RGB(intensity * 255, intensity * 255, intensity * 255);
                //COLORREF color = RGB(255, 255, 255);
                //COLORREF color = RGB((30 * i + 20) % 256, (30 * i + 20) % 256, (30 * i + 20) % 256);
				putpixel(j, y, color);
			}
		}
        */
    }
}


void processPhongShadingPixel(const int y, std::vector<Intersection> intersections, const PhongShadingLight& phongShadingLight) {
    if (intersections.size() == 0) {
        return;
    }

    for (int i = 0; i < int(intersections.size()); i += 2) {
        int x1 = static_cast<int>(intersections[i].x);
        int x2 = static_cast<int>(intersections[i + 1].x);
        for (int j = x1; j <= x2; j++) {
            // 法线插值
            float t = (x1 == x2) ? 0 : ((float)j - (float)x1) / ((float)x2 - (float)x1);
            Vertex normalInterpolated = interpolateNormal(intersections[i].normal, intersections[i + 1].normal, t);
            //Vertex normalInterpolated = interpolateNormal(intersections[i + 1].normal, intersections[i].normal, t);

            // 计算冯氏光强
			float worldX = intersections[i].xWorld * (1 - t) + intersections[i + 1].xWorld * t;
			float worldY = intersections[i].yWorld * (1 - t) + intersections[i + 1].yWorld * t;
			float worldZ = intersections[i].zWorld * (1 - t) + intersections[i + 1].zWorld * t;
            float intensity = calculatePhongIntensity(worldX, worldY, worldZ, normalInterpolated, phongShadingLight);

            // Gamma补正，效果不好
            //float gamma = 2.2f;
            //intensity = pow(intensity, 1.0f / intensity);

            COLORREF color = RGB(intensity * 255, intensity * 255, intensity * 255);

            putpixel(j, y, color); 
        }
    }
}

// Phong Shading
void phongShading(const std::vector<Vertex>& vertices, const std::vector<Vertex>& projectedVertices, const std::vector<Face>& faces, const int width, const int height, const PhongShadingLight& phongShadingLight) {
    zBuffer = std::vector<std::vector<float>>(width, std::vector<float>(height, INF));
    std::vector<Vertex> normals(vertices.size());
    std::vector<Face> needToDrawFaces;

    for (size_t i = 0; i < faces.size(); ++i) {
        const Face& face = faces[i];

        const Vertex& v1 = vertices[face.vertexIndices[0]];
        const Vertex& v2 = vertices[face.vertexIndices[1]];
        const Vertex& v3 = vertices[face.vertexIndices[2]];

        Vertex normal = calculateNormal(v1, v2, v3);

        // Back-face culling
        Vertex viewVector = { 0.0f, 0.0f, 1.0f };
        float dotProduct = normal.x * viewVector.x + normal.y * viewVector.y + normal.z * viewVector.z;
        if (dotProduct > 0) {
            needToDrawFaces.push_back(face);
        }

		// 处理多个面共顶点的情况，先累加顶点所有面法线，再归一化
        for (int i = 0; i < face.vertexIndices.size(); ++i) {
            normals[face.vertexIndices[i]].x += normal.x;
            normals[face.vertexIndices[i]].y += normal.y;
            normals[face.vertexIndices[i]].z += normal.z;
        }
    }

    // 归一化法线
    for (auto& normal : normals) {
        normal = normalize(normal);
    }

    int originX = width / 2;
    int originY = height / 2;

    // Scanline方法绘图
    for (int y = -originY; y <= height - originY; y++) {
        std::vector<Intersection> intersections;
		std::vector<float> intensities(vertices.size()); // 填充参数，没用到
        findIntersections(projectedVertices, vertices, needToDrawFaces, normals, y, intensities, intersections);
        sortIntersections(intersections);
		processPhongShadingPixel(y, intersections, phongShadingLight);

        /*
        if (intersections.size() == 0) {
            continue;
         }

        for (int i = 0; i < int(intersections.size()); i += 2) {
            int x1 = static_cast<int>(intersections[i].x);
            int x2 = static_cast<int>(intersections[i + 1].x);
            for (int j = x1; j <= x2; j++) {
                // 在每个像素上进行法线插值
                float t = (x1 == x2) ? 0 : ((float)j - (float)x1) / ((float)x2 - (float)x1);
                Vertex normalInterpolated = interpolateNormal(intersections[i].normal, intersections[i + 1].normal, t);
                //Vertex normalInterpolated = interpolateNormal(intersections[i + 1].normal, intersections[i].normal, t);

                // 计算光照（Phong Shading）
                float intensity = calculatePhongIntensity(normalInterpolated, phongShadingLight);
                COLORREF color = RGB(intensity * 255, intensity * 255, intensity * 255);
                putpixel(j, y, color);  // 绘制像素
            }
        }
        */
    }
}
