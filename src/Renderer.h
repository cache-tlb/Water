#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Water.h"
#include "vec3.h"
#include "Water.h"
#include "Cubemap.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    Mesh *waterMesh;
    Mesh *sphereMesh;
    Mesh *cubeMesh;

    Texture *tileTexture;
    Texture *causticTex;
    
    Vector lightDir;
    Vector sphereCenter;
    float lightDirArray[3];
    float sphereCenterArray[3];
    float sphereRadius;

    Shader *waterShaders[2];
    Shader *sphereShader;
    Shader *cubeShader;
    Shader *causticsShader;

    bool chectShader() const;

    void updateCaustics(Water *water);
    void renderWater(Water *water, Cubemap *sky);
    void renderSphere(Water *water);
    void renderCube(Water *water);

    void setSphereCenter(const Vector& v);
    void setLightDir(const Vector& v);
};

