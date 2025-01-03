#include <graphics.h>
#include "ObjLoader.h"
#include "Renderer.h"
#include "Shading.h"
#include "TerrainGenerator.h"
#include "NormalMapping.h"
#include <vector>
#include <string>
#include <iostream>
#include <conio.h>

using namespace std;
extern void NormalMappingShading(std::vector<Vertex>& vertices, std::vector<Vertex>& projectedVertices, const std::vector<Face>& faces, const int width, const int height, const PhongShadingLight& phongShadingLight);

int main() {

    //TerrainGenerator terrain(16, 16, 32);  // 16x16x32 区块
    //terrain.generateTerrain();
	//terrain.writeOBJ("terrain.obj");

	const int width = 800;
	const int height = 600;

    vector<Vertex> vertices;
    vector<Face> faces;

    // 初始化图形窗口
    initgraph(width, height);
    setorigin(width/2, height/2);

	
    // 多边形obj模型
    //string filename = "correct_17gon.obj";
    
	// 精细的球体obj模型
    string filename = "ball.obj";
	

    if (readObj(filename, vertices, faces)) {
        //cout << "成功读取 OBJ 文件" << endl;
        
        float angleX = 5.9f;
        float angleY = 0.8f;
        float angleZ = 5.0f;

        char currentAxis = 'x'; 

        Matrix4x4 projectionMatrix = createProjectionMatrix(25.0f);

        while (true) {
            if (_kbhit()) {
                char key = _getch();
                if (key == 27) { // ESC
                    break;
                }
            }

            // 检查长按按键状态
            if (GetAsyncKeyState('Q') & 0x8000) {
                angleX += 0.02f;
            }
            if (GetAsyncKeyState('W') & 0x8000) {
                angleY += 0.02f;
            }
            if (GetAsyncKeyState('E') & 0x8000) {
                angleZ += 0.02f;
            }

            cleardevice();

            // 旋转矩阵
            Matrix4x4 rotationMatrix = createRotationMatrix(angleX, angleY, angleZ);

			PhongShadingLight phongShadingLight = {
				{ -6.9, -0.9, -1.3}, // 光源位置
				0.2f,        // 环境光强
			    0.8f,        // 漫反射光强
				0.5f         // 镜面反射光强
			};

            vector<Vertex> transformedVertices;
            vector<Vertex> projectedVertices;
            transformVertices(vertices, rotationMatrix, projectionMatrix, transformedVertices);
            projectVertices(transformedVertices, rotationMatrix, projectionMatrix, projectedVertices);
            // 无渲染效果
            //drawModel(projectedVertices, faces);
            
			// Constant Shading实现
            //constantShading(transformedVertices, projectedVertices, faces, width, height, { 0, 0, 0.5 });
			
			// Gouraud Shading实现
            //gouraudShading(transformedVertices, projectedVertices, faces, width, height,{ 0, 0, 0.5 });
            
			// Phong Shading实现
			//phongShading(transformedVertices, projectedVertices, faces, width, height, phongShadingLight);

			// Normal Mapping实现（月球表面法线贴图，可以替换为其他法线贴图）
			NormalMappingShading(transformedVertices, projectedVertices, faces, width, height, phongShadingLight);

            //Sleep(16); // 60 FPS
            system("pause");
        }
    }
    else {
        cout << "读取 OBJ 文件失败" << endl;
    }

    closegraph();

    return 0;
}
