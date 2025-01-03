#include "ObjLoader.h"
#include <graphics.h>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

bool readObj(const string& filename, vector<Vertex>& vertices, vector<Face>& faces) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "无法打开文件: " << filename << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string type;
        ss >> type;

        if (type == "v") {
            Vertex vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (type == "f") {
            Face face;
            string index;
            while (ss >> index) {
                int vertexIndex = stoi(index);
                face.vertexIndices.push_back(vertexIndex - 1); 
            }
            faces.push_back(face);
        }
    }

    file.close();
    return true;
}

#include <string>
#include <graphics.h>
#include <string>

// Helper function to convert std::string to std::wstring
std::wstring stringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void drawModel(const vector<Vertex>& vertices, const vector<Face>& faces) {
    for (const auto& face : faces) {
        for (size_t i = 0; i < face.vertexIndices.size(); ++i) {
            int startIdx = face.vertexIndices[i];
            int endIdx = face.vertexIndices[(i + 1) % face.vertexIndices.size()];

            int x1 = static_cast<int>(vertices[startIdx].x);
            int y1 = static_cast<int>(vertices[startIdx].y);
            int x2 = static_cast<int>(vertices[endIdx].x);
            int y2 = static_cast<int>(vertices[endIdx].y);

            line(x1, y1, x2, y2);
        }
    }
}
