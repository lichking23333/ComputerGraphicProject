#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <string>
#include "Geometry.h"

bool readObj(const std::string& filename, std::vector<Vertex>& vertices, std::vector<Face>& faces);

// ����ģ��������û��Ⱦ��
void drawModel(const std::vector<Vertex>& vertices, const std::vector<Face>& faces);

#endif // OBJLOADER_H
