#ifndef OBJMODEL_H
#define OBJMODEL_H

#include "model.h"
#include <string>
#include <map>

namespace Models {
    //to do mtl
    struct Material {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
        std::string name;

        Material() :
            ambient(0.2f, 0.2f, 0.2f),
            diffuse(0.8f, 0.8f, 0.8f),
            specular(1.0f, 1.0f, 1.0f),
            shininess(32.0f),
            name("default") {}
    };

    class ObjModel : public Model {
    public:
        ObjModel(const std::string& filename);
        virtual ~ObjModel();
        virtual void drawSolid(bool smooth = true);

        glm::vec3 getDiffuseColor() const { return currentMaterial.diffuse; }
        glm::vec3 getAmbientColor() const { return currentMaterial.ambient; }
        glm::vec3 getSpecularColor() const { return currentMaterial.specular; }
        float getShininess() const { return currentMaterial.shininess; }

    private:
        void loadOBJ(const std::string& filename);
        void loadMTL(const std::string& filename);
        std::vector<glm::vec4> internalVertices;
        std::vector<glm::vec4> internalVertexNormals;
        std::vector<glm::vec4> internalFaceNormals;
        std::vector<glm::vec4> internalTexCoords;

        std::map<std::string, Material> materials;
        Material currentMaterial;
    };

}

#endif
