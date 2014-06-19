//
// Lighthouse3D.com GLSL Core Tutorial - Hello World Example
//
//
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
//
// If you do use it I would love to hear about it. Just post a comment
// at Lighthouse3D.com
#include "StdHeader.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "VSMathlib.h"
#include "VSShaderlib.h"
#include "Shader.h"
#include "Mesh.h"
#include "Water.h"
#include "Renderer.h"
#include "Cubemap.h"
#include "vec3.h"
#include "SOIL.h"
#include "Texture.h"
#include "Raytracer.h"


VSMathLib *vsml;
Shader *shader;
Mesh *mesh;

// the Water begin
Water *water;
Cubemap *cubemap;
Renderer *renderer;
float angleX = -25.f;
float angleY = -200.5f;

// sphere info
bool useSpherePhysics = false;
Vector center;
Vector oldCenter;
Vector velocity;
Vector gravity;
float radius;
int paused = 0;

// misc
int prevTime;
Vector prevHit;
Vector planeNormal;
int mode = -1;
const int MODE_ADD_DROPS = 0;
const int MODE_MOVE_SPHERE = 1;
const int MODE_ORBIT_CAMERA = 2;
int oldX, oldY;
int winWidth, winHeight;

void resize(int w, int h) {
	float fov;
	// Prevent a divide by zero, when window is too short
	if(h == 0) h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
    winWidth = w;
    winHeight = h;
	// set the projection matrix
	fov = (1.0f * w) / h;
	vsml->loadIdentity(VSMathLib::PROJECTION);
	vsml->perspective(45.f, fov, 0.1f, 100.0f);
}

void update(float seconds) {
    if (seconds > 1) return;

    if (mode == MODE_MOVE_SPHERE) {
        velocity = Vector();
    }
    else if (useSpherePhysics) {
        float percentUnderWater = std::max<float>(0, std::min<float>(1, (radius - center.y) / (2.f * radius)));
        velocity = velocity.add(gravity.multiply(seconds - 1.1f * seconds * percentUnderWater));
        velocity = velocity.subtract(velocity.unit().multiply(percentUnderWater * seconds * velocity.dot(velocity)));
        center = center.add(velocity.multiply(seconds));

        if (center.y < radius - 1.f) {
            center.y = radius - 1.f;
            velocity.y = fabs(velocity.y) * 0.7f;
        }
    }

    water->moveSphere(oldCenter, center, radius);
    oldCenter = center;

    water->stepSimulation();
    //water->stepSimulation();
    water->updateNormals();
    renderer->updateCaustics(water);
}


void draw(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vsml->loadIdentity(VSMathLib::VIEW);
	vsml->loadIdentity(VSMathLib::MODEL);
    vsml->translate(0,0,-4);
    vsml->rotate(-angleX, 1, 0, 0);
    vsml->rotate(-angleY, 0, 1, 0);
    vsml->translate(0, 0.5, 0);

    glEnable(GL_DEPTH_TEST);
    
    renderer->setSphereCenter(center);
    renderer->sphereRadius = radius;
    renderer->renderCube(water);
    renderer->renderWater(water, cubemap);
    renderer->renderSphere(water);
    glDisable(GL_DEPTH_TEST);
	glutSwapBuffers();
}


void processKeys(unsigned char key, int xx, int yy) {
	switch(key) {
	case 27:
		glutLeaveMainLoop();
		break;
    case ' ':
        paused = !paused;
        break;
    case 'G':
    case 'g':
        useSpherePhysics = !useSpherePhysics;
        break;
    case 'L':
    case 'l':
        renderer->setLightDir(Vector::fromAngles((90 - angleY) * M_PI / 180.f, -angleX * M_PI / 180));
        if (paused) {
            renderer->updateCaustics(water);
            draw();
        }
        break;
    default:
        break;
	}

}

void onMouseMove(int x, int y){
    RayTracer tracer;
    Vector ray = tracer.getRayForPixel((float)x, (float)y);
    Vector pointOnPlane;
    switch (mode) {
    case MODE_ADD_DROPS: {
        pointOnPlane = tracer.eye.add(ray.multiply(- tracer.eye.y / ray.y));
        water->addDrop(pointOnPlane.x, pointOnPlane.z, 0.03, 0.01);
        if (paused) {
            water->updateNormals();
            renderer->updateCaustics(water);
        }
        break;
    }
    case MODE_MOVE_SPHERE: {
        float t = -planeNormal.dot(tracer.eye.subtract(prevHit)) / planeNormal.dot(ray);
        Vector nextHit = tracer.eye.add(ray.multiply(t));
        center = center.add(nextHit.subtract(prevHit));
        center.x = std::max<float>(radius - 1, std::min<float>(1 - radius, center.x));
        center.y = std::max<float>(radius - 1, std::min<float>(10, center.y));
        center.z = std::max<float>(radius - 1, std::min<float>(1 - radius, center.z));
        prevHit = nextHit;
        if (paused) renderer->updateCaustics(water);
        break;
    }
    case MODE_ORBIT_CAMERA:
        angleY -= x - oldX;
        angleX -= y - oldY;
        angleX = std::max<float>(-89.999, std::min<float>(89.999, angleX));
        break;
    default:
        break;
    }
    oldX = x;
    oldY = y;
    if (paused) draw();
}

void onMouseDown(int x, int y) {
    oldX = x;
    oldY = y;
    RayTracer tracer;
    Vector ray = tracer.getRayForPixel((float)x, (float)y);
    Vector pointOnPlane = tracer.eye.add(ray.multiply(- tracer.eye.y / ray.y));
    HitTest sphereHitTest;
    bool hit = RayTracer::hitTestSphere(tracer.eye, ray, center, radius, sphereHitTest);
    if (hit) {
        mode = MODE_MOVE_SPHERE;
        prevHit = sphereHitTest.hit;
        planeNormal = tracer.getRayForPixel(float(winWidth)/2.f, float(winHeight)/2.f).multiply(-1.f);
    } 
    else if (fabs(pointOnPlane.x) < 1 && fabs(pointOnPlane.z) < 1) {
        mode = MODE_ADD_DROPS;
        onMouseMove(x, y);
    } 
    else {
        mode = MODE_ORBIT_CAMERA;
    }
}

void onMouseUp(int x, int y) {
    mode = -1;
} 

void processMouseButtons(int button, int state, int xx, int yy) {
	// start tracking the mouse
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)  {
		onMouseDown(xx, yy);
	}
    else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
        onMouseUp(xx, yy);
    }
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy) {
	onMouseMove(xx, yy);
}

void initOpenGL()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


void initVSL() {
	vsml = VSMathLib::getInstance();
}

float rand_float() {
    return (float)rand() / (float)RAND_MAX;
}

void initScene() {
    water = new Water;
    cubemap = Cubemap::fromImages(
        "./images/xneg.jpg", 
        "./images/xpos.jpg",
        "./images/ypos.jpg",
        "./images/ypos.jpg",
        "./images/zneg.jpg",
        "./images/zpos.jpg"
    );
    renderer = new Renderer;
    center = Vector(-0.4, -0.75, 0.2);
    oldCenter = Vector(-0.4, -0.75, 0.2);
    velocity = Vector(0.0,0.0,0.0);
    gravity = Vector(0.0,-4.0,0.0);
    radius = 0.25;
    prevTime = glutGet(GLUT_ELAPSED_TIME);
    for (int i = 0; i < 20; i++) {
        water->addDrop(rand_float() * 2.f - 1.f, rand_float() * 2.f - 1.f, 0.03, (i & 1) ? 0.01 : -0.01);
    }
}


void idle(){
    int nextTime = glutGet(GLUT_ELAPSED_TIME);
    if (!paused) {
        update(float(nextTime - prevTime) / 1000.f);
        draw();
    }
    prevTime = nextTime;
}


int main(int argc, char **argv) {

//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	glutInitContextVersion (3, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(512,512);
	glutCreateWindow("Lighthouse3D - Simple Shader Demo");

//  Callback Registration
	glutDisplayFunc(draw);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);


//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));

	initOpenGL();

	initVSL();
    initScene();

	//  GLUT main loop
	glutMainLoop();

	return(0);

}

