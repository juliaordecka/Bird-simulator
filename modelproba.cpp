#include "modelproba.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

namespace Models {

    ObjModel::ObjModel(const string& filename) {
        cout << "Wczytywany plik: " << filename << endl;
        loadOBJ(filename);
    }

    ObjModel::~ObjModel() {}

    void ObjModel::loadOBJ(const string& filename) {
        vector<vec3> temp_vertices;
        vector<vec3> temp_normals;
        vector<vec2> temp_texcoords;

        internalVertices.clear();
        internalVertexNormals.clear();
        internalFaceNormals.clear();
        internalTexCoords.clear();

        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Nie da sie otworzyc pliku: " << filename << endl;
            return;
        }

        cout << "Otwarto plik: " << filename << endl;

        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string type;
            iss >> type;

            if (type == "v") {
                vec3 vertex;
                iss >> vertex.x >> vertex.y >> vertex.z;
                temp_vertices.push_back(vertex);
            }
            else if (type == "vn") {
                vec3 normal;
                iss >> normal.x >> normal.y >> normal.z;
                temp_normals.push_back(normal);
            }
            else if (type == "vt") {
                vec2 texcoord;
                iss >> texcoord.x >> texcoord.y;
                temp_texcoords.push_back(texcoord);
            }
            else if (type == "f") {
                string vertex1, vertex2, vertex3;
                iss >> vertex1 >> vertex2 >> vertex3;

                int v1, v2, v3, t1, t2, t3, n1, n2, n3;

                sscanf_s(vertex1.c_str(), "%d/%d/%d", &v1, &t1, &n1);
                sscanf_s(vertex2.c_str(), "%d/%d/%d", &v2, &t2, &n2);
                sscanf_s(vertex3.c_str(), "%d/%d/%d", &v3, &t3, &n3);

                v1--; v2--; v3--;
                t1--; t2--; t3--;
                n1--; n2--; n3--;

                internalVertices.push_back(vec4(temp_vertices[v1], 1.0f));
                internalVertices.push_back(vec4(temp_vertices[v2], 1.0f));
                internalVertices.push_back(vec4(temp_vertices[v3], 1.0f));

                internalVertexNormals.push_back(vec4(temp_normals[n1], 0.0f));
                internalVertexNormals.push_back(vec4(temp_normals[n2], 0.0f));
                internalVertexNormals.push_back(vec4(temp_normals[n3], 0.0f));

                internalFaceNormals.push_back(vec4(temp_normals[n1], 0.0f));
                internalFaceNormals.push_back(vec4(temp_normals[n2], 0.0f));
                internalFaceNormals.push_back(vec4(temp_normals[n3], 0.0f));

                if (!temp_texcoords.empty()) {
                    internalTexCoords.push_back(vec4(temp_texcoords[t1], 0.0f, 0.0f));
                    internalTexCoords.push_back(vec4(temp_texcoords[t2], 0.0f, 0.0f));
                    internalTexCoords.push_back(vec4(temp_texcoords[t3], 0.0f, 0.0f));
                }
                else {
                    internalTexCoords.push_back(vec4(0.0f, 0.0f, 0.0f, 0.0f));
                    internalTexCoords.push_back(vec4(0.0f, 0.0f, 0.0f, 0.0f));
                    internalTexCoords.push_back(vec4(0.0f, 0.0f, 0.0f, 0.0f));
                }
            }
        }

        vertices = (float*)internalVertices.data();
        texCoords = (float*)internalTexCoords.data();
        vertexNormals = (float*)internalVertexNormals.data();
        normals = (float*)internalFaceNormals.data();
        vertexCount = internalVertices.size();

        cout << "Model zostal zaladowany z " << vertexCount << " wierzcholkami" << endl;
        //cout << "Vertices array at address: " << vertices << endl;
    }

    void ObjModel::drawSolid(bool smooth) {

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, vertices);
        glVertexAttribPointer(1, 4, GL_FLOAT, false, 0, smooth ? vertexNormals : normals);
        glVertexAttribPointer(2, 4, GL_FLOAT, false, 0, texCoords);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            cerr << "OpenGL error: " << error << endl;
        }

        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        error = glGetError();
        if (error != GL_NO_ERROR) {
            cerr << "OpenGL error: " << error << endl;
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
}