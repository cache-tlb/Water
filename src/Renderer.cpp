#include "Renderer.h"
#include "Raytracer.h"

Renderer::Renderer() 
    : sphereCenter(), sphereRadius(0.f)
{
    Option options;
    options["minFilter"] = toString(GL_LINEAR_MIPMAP_LINEAR);
    options["warp"] = toString(GL_REPEAT);
    options["format"] = toString(GL_RGB);

    tileTexture = Texture::fromImage("./images/tiles.jpg", options);
    lightDir = Vector(2.0, 2.0, -1.0).unit();
    lightDir.toArray(lightDirArray);
    sphereCenter.toArray(sphereCenterArray);

    options.clear();
    causticTex = new Texture(1024,1024, options);

    waterMesh = Mesh::plane(200,200);
    for (int i = 0; i < 2; i++) {
        if (i) waterShaders[i] = new Shader("./shaders/RendererWater.vert", "./shaders/RendererUnderWater.frag");
        else waterShaders[i] = new Shader("./shaders/RendererWater.vert", "./shaders/RendererAboveWater.frag");
    }

    sphereMesh = Mesh::sphere(10);
    sphereShader = new Shader("./shaders/RendererSphere.vert", "./shaders/RendererSphere.frag");

    cubeMesh = Mesh::cube();
    std::vector<unsigned int> new_tri;
    for (int i = 0; i < cubeMesh->triangles.size(); i++) {
        if (i < 12 || i >= 18) new_tri.push_back(cubeMesh->triangles[i]);
    }
    cubeMesh->triangles.swap(new_tri);
    cubeMesh->compile();
    cubeShader = new Shader("./shaders/RendererCube.vert", "./shaders/RendererCube.frag");

    causticsShader = new Shader("./shaders/RendererCaustics.vert", "./shaders/RendererCaustics.frag");

}

Renderer::~Renderer() {
    if (waterMesh) delete waterMesh;
    if (sphereMesh) delete sphereMesh;
    if (cubeMesh) delete cubeMesh;

    if (tileTexture) delete tileTexture;
    if (causticTex) delete causticTex;

    if (waterShaders[0]) delete waterShaders[0];
    if (waterShaders[1]) delete waterShaders[1];
    if (sphereShader) delete sphereShader;
    if (cubeShader) delete cubeShader;
    if (causticsShader) delete causticsShader;
}

void Renderer::updateCaustics(Water *water) {
    if (!causticsShader) return;
    Shader *_causticsShader = causticsShader;
    Mesh *_waterMesh = waterMesh;
    float *_lightDir  = lightDirArray;
    float *_sphereCenter = sphereCenterArray;
    float _sphereRadius = sphereRadius;

    std::function<void(void)> callback = [water, _causticsShader, _waterMesh, _lightDir, _sphereCenter, _sphereRadius](){
        glClear(GL_COLOR_BUFFER_BIT);
        water->textureA->bind(0);
        _causticsShader->uniforms("light", _lightDir);
        _causticsShader->uniforms("water", 0);
        _causticsShader->uniforms("sphereCenter", (void*)_sphereCenter);
        _causticsShader->uniforms("sphereRadius", _sphereRadius);
        _causticsShader->draw(_waterMesh);
    };

    causticTex->drawTo(callback);
}

void Renderer::renderSphere(Water *water) {
    water->textureA->bind(0);
    causticTex->bind(1);
    sphereShader->uniforms("light", lightDirArray);
    sphereShader->uniforms("water", 0);
    sphereShader->uniforms("causticTex", 1);
    sphereShader->uniforms("sphereCenter", sphereCenterArray);
    sphereShader->uniforms("sphereRadius", sphereRadius);
    sphereShader->draw(sphereMesh);
}

void Renderer::renderCube(Water *water) {
    glEnable(GL_CULL_FACE);
    water->textureA->bind(0);
    tileTexture->bind(1);
    causticTex->bind(2);
    cubeShader->uniforms("light", lightDirArray);
    cubeShader->uniforms("water", 0);
    cubeShader->uniforms("tiles", 1);
    cubeShader->uniforms("causticTex", 2);
    cubeShader->uniforms("sphereCenter", sphereCenterArray);
    cubeShader->uniforms("sphereRadius", sphereRadius);
    cubeShader->draw(cubeMesh);
    glDisable(GL_CULL_FACE);
}

void Renderer::renderWater(Water *water, Cubemap *sky) {
    // todo
    RayTracer tracer;
    float eye_array[3]; tracer.eye.toArray(eye_array);
    water->textureA->bind(0);
    tileTexture->bind(1);
    sky->bind(2);
    causticTex->bind(3);
    glEnable(GL_CULL_FACE);
    for (int i = 0; i < 2; i++) {
        glCullFace(i ? GL_BACK : GL_FRONT);
        waterShaders[i]->uniforms("light", lightDirArray);
        waterShaders[i]->uniforms("water", 0);
        waterShaders[i]->uniforms("tiles", 1);
        waterShaders[i]->uniforms("sky", 2);
        waterShaders[i]->uniforms("causticTex", 3);
        waterShaders[i]->uniforms("eye", eye_array);
        waterShaders[i]->uniforms("sphereCenter", sphereCenterArray);
        waterShaders[i]->uniforms("sphereRadius", sphereRadius);
        waterShaders[i]->draw(waterMesh);
    }
    glDisable(GL_CULL_FACE);
}

void Renderer::setLightDir(const Vector& v) {
    lightDir = v;
    lightDir.toArray(lightDirArray);
}

void Renderer::setSphereCenter(const Vector& v) {
    sphereCenter = v;
    sphereCenter.toArray(sphereCenterArray);
}

bool Renderer::chectShader() const {
    return waterShaders[0]->vsshader.isProgramValid()
        && waterShaders[1]->vsshader.isProgramValid()
        && sphereShader->vsshader.isProgramValid()
        && cubeShader->vsshader.isProgramValid()
        && causticsShader->vsshader.isProgramValid();
}

