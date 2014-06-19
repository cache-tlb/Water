#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "vec3.h"

class Water {
public:
	Water();
	~Water();

    Mesh *plane;
    Texture *textureA;
    Texture *textureB;

    Shader *dropShader;
    Shader *updateShader;
    Shader *sphereShader;
    Shader *normalShader;

    bool checkShader() const;

    void addDrop(float x, float y, float radius, float strength);
    void moveSphere(const Vector &oldCenter, const Vector &newCenter, float radius);
    void stepSimulation();
    void updateNormals();
protected:
	
private:
};
