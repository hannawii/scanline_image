// main.cpp

//
// For this project, we use OpenGL, GLUT
// and GLEW (to load OpenGL extensions)
//
#include "stglew.h"

#include <stdio.h>
#include <string.h>

//
// Globals used by this application.
// As a rule, globals are Evil, but this is a small application
// and the design of GLUT makes it hard to avoid them.
//

// Window size, kept for screenshots
static int gWindowSizeX = 0;
static int gWindowSizeY = 0;

// File locations
std::string vertexShader;
std::string fragmentShader;
std::string normalMap;
std::string displacementMap;
std::string colorMap;
std::string meshOBJ;

// Light source attributes
static float specularLight[] = {1.5, 1.5, 1.5, 1.0};
static float ambientLight[]  = {0.90, 0.90, 0.90, 1.0};
static float diffuseLight[]  = {1.20, 1.20, 1.20, 1.0};

float lightPosition[] = {10.0f, 15.0f, -5.0f, 1.0f};

// Material color properties
static float materialAmbient[]  = { 0.3, 0.3, 0.3, 1.0 };
static float materialDiffuse[]  = { 0.9, 0.9, 0.9, 0.4 };
static float materialSpecular[] = { 0.8, 0.8, 0.8, 1.0 };
static float shininess          = 8.0;  // # between 1 and 128.

// Material color properties

// island
static float materialIslandAmbient[]  = { 0.1, 0.1, 0.1, 0.7 };
static float materialIslandDiffuse[]  = { 0.15, 0.1, 0.1, 0.6 };
static float materialIslandSpecular[] = { 0.15, 0.1, 0.2, 1.0 };
static float shiniessIsland = 2.0;

static float materialMoonAmbient[]  = { 0.1, 0.1, 0.1, 1.0 };
static float materialMoonDiffuse[]  = { 1.0, 1.0, 1.0, 0.5 };
static float materialMoonSpecular[] = { 0.1, 0.1, 0.1, 1.0 };
static float shininessMoon          = 3.0;  // # between 1 and 128.

// Material color properties
static float materialSkyAmbient[]  = { 0.5, 0.5, 0.5, 1.0 };
static float materialSkyDiffuse[]  = { 0.5, 0.5, 0.5, 1.0 };
static float materialSkySpecular[] = { 0.1, 0.1, 0.1, 1.0 };
static float shininessSky          = 1.0;

//static float material2Ambient[]  = { 1., 1., 1., 1. };
//static float material2Diffuse[]  = { 1., 1., 1., 1. };
//static float material2Specular[] = { 1., 1., 1., 1. };
//static float shininess2          = 1.;  // # between 1 and 128.

//rock
STImage   *surfaceNormRock1Img;
STTexture *surfaceNormRock1Tex;

STImage   *surfaceDisplaceRock1Img;
STTexture *surfaceDisplaceRock1Tex;

STImage   *surfaceColorRock1Img;
STTexture *surfaceColorRock1Tex;

//sky
STImage   *surfaceNormSkyImg;
STTexture *surfaceNormSkyTex;

STImage   *surfaceDisplaceSkyImg;
STTexture *surfaceDisplaceSkyTex;

STImage   *surfaceColorSkyImg;
STTexture *surfaceColorSkyTex;

//water
STImage   *surfaceNormWaterImg;
STTexture *surfaceNormWaterTex;

STImage   *surfaceDisplaceWaterImg;
STTexture *surfaceDisplaceWaterTex;

STImage   *surfaceColorWaterImg;
STTexture *surfaceColorWaterTex;

//moon
STImage   *surfaceNormMoonImg;
STTexture *surfaceNormMoonTex;

STImage   *surfaceDisplaceMoonImg;
STTexture *surfaceDisplaceMoonTex;

STImage   *surfaceColorMoonImg;
STTexture *surfaceColorMoonTex;

//island
STImage   *surfaceNormIslandImg;
STTexture *surfaceNormIslandTex;

STImage *surfaceColorIslandImg;
STTexture *surfaceColorIslandTex;

STImage   *surfaceDisplaceIslandImg;
STTexture *surfaceDisplaceIslandTex;

//huts
STImage   *surfaceNormHutsImg;
STTexture *surfaceNormHutsTex;

STImage *surfaceColorHutsImg;
STTexture *surfaceColorHutsTex;

STImage   *surfaceDisplaceHutsImg;
STTexture *surfaceDisplaceHutsTex;


//shaders
STShaderProgram *shaderWater;
STShaderProgram *shaderSky;
STShaderProgram *shaderRock1;
STShaderProgram *shaderMoon;
STShaderProgram *shaderIsland;
STShaderProgram *shaderHuts;


// Stored mouse position for camera rotation, panning, and zoom.
int gPreviousMouseX = -1;
int gPreviousMouseY = -1;
int gMouseButton = -1;
STVector3 mCameraTranslation;
float mCameraAzimuth;
float mCameraElevation;
bool mesh = false; // draw mesh
bool smooth = true; // smooth/flat shading for mesh
bool normalMapping = true; // true=normalMapping, false=displacementMapping
bool proxyType=false; // false: use cylinder; true: use sphere

STTriangleMesh* gTriangleMesh = 0;
STTriangleMesh* water = 0;
STTriangleMesh* sky = 0;
STTriangleMesh* rock1 = 0;
STTriangleMesh* moon = 0;
STTriangleMesh* island = 0;
STTriangleMesh* huts = 0;
STTriangleMesh* boat = 0;

int TesselationDepth = 100;

void resetCamera()
{
    mCameraTranslation = STVector3(0.f, 0.f, 16.5f);
    mCameraAzimuth = 0.f;
    mCameraElevation = 65.0f;
}

void CreateYourOwnMesh()
{
    float leftX   = -8.0f;
    float rightX  = -leftX;
    float nearZ   = -2.0f;
    float farZ    = 5.0f;
    
    water= new STTriangleMesh();
    for (int i = 0; i < TesselationDepth+1; i++){
        for (int j = 0; j < TesselationDepth+1; j++) {
            float s0 = (float) i / (float) TesselationDepth;
            float x0 =  s0 * (rightX - leftX) + leftX;
            float t0 = (float) j / (float) TesselationDepth;
            float z0 = t0 * (farZ - nearZ) + nearZ;

            water->AddVertex(x0,(x0*x0+z0*z0)*0.0f,z0,s0,t0);
        }
    }
    for (int i = 0; i < TesselationDepth; i++){
        for (int j = 0; j < TesselationDepth; j++) {
            unsigned int id0=i*(TesselationDepth+1)+j;
            unsigned int id1=(i+1)*(TesselationDepth+1)+j;
            unsigned int id2=(i+1)*(TesselationDepth+1)+j+1;
            unsigned int id3=i*(TesselationDepth+1)+j+1;
            water->AddFace(id0,id2,id1);
            water->AddFace(id0,id3,id2);
        }
    }
    water->Build();

    sky = water;
    
}
//
// Initialize the application, loading all of the settings that
// we will be accessing later in our fragment shaders.
//
void Setup()
{
    // Set up lighting variables in OpenGL
    // Once we do this, we will be able to access them as built-in
    // attributes in the shader (see examples of this in normalmap.frag)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_SPECULAR,  specularLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT,   ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,   diffuseLight);

    surfaceNormWaterImg = new STImage("images/water1.jpg");
    surfaceNormWaterTex = new STTexture(surfaceNormWaterImg);
    
    surfaceDisplaceWaterImg = new STImage("images/water1.jpg");
    surfaceDisplaceWaterTex = new STTexture(surfaceDisplaceWaterImg);

	surfaceColorWaterImg = new STImage("images/deep_blue.jpg");
    surfaceColorWaterTex = new STTexture(surfaceColorWaterImg);
    
    shaderWater = new STShaderProgram();
    shaderWater->LoadVertexShader(vertexShader);
    shaderWater->LoadFragmentShader(fragmentShader);
    
    
    surfaceNormSkyImg = new STImage("images/night_sky.jpg");
    surfaceNormSkyTex = new STTexture(surfaceNormSkyImg);
    
    surfaceDisplaceSkyImg = new STImage("images/night_sky.jpg");
    surfaceDisplaceSkyTex = new STTexture(surfaceDisplaceSkyImg);
    
    surfaceColorSkyImg = new STImage("images/night_sky.jpg");
    surfaceColorSkyTex = new STTexture(surfaceColorSkyImg);
    
    shaderSky = new STShaderProgram();
    shaderSky->LoadVertexShader(vertexShader);
    shaderSky->LoadFragmentShader(fragmentShader);
    
    
    surfaceNormRock1Img = new STImage("images/rock1.jpg");
    surfaceNormRock1Tex = new STTexture(surfaceNormRock1Img);
    
    surfaceDisplaceRock1Img = new STImage("images/rock1.jpg");
    surfaceDisplaceRock1Tex = new STTexture(surfaceDisplaceRock1Img);
    
	surfaceColorRock1Img = new STImage("images/rock1.jpg");
    surfaceColorRock1Tex = new STTexture(surfaceColorRock1Img);
    
    shaderRock1 = new STShaderProgram();
    shaderRock1->LoadVertexShader(vertexShader);
    shaderRock1->LoadFragmentShader(fragmentShader);
    

    surfaceNormMoonImg = new STImage("images/moon.jpg");
    surfaceNormMoonTex = new STTexture(surfaceNormMoonImg);
    
    surfaceDisplaceMoonImg = new STImage("images/moon.jpg");
    surfaceDisplaceMoonTex = new STTexture(surfaceDisplaceMoonImg);
    
	surfaceColorMoonImg = new STImage("images/moon.jpg");
    surfaceColorMoonTex = new STTexture(surfaceColorMoonImg);
    
    shaderMoon = new STShaderProgram();
    shaderMoon->LoadVertexShader(vertexShader);
    shaderMoon->LoadFragmentShader(fragmentShader);
    
    surfaceNormIslandImg = new STImage("images/texture3.jpg");
    surfaceNormIslandTex = new STTexture(surfaceNormIslandImg);
    
    surfaceDisplaceIslandImg = new STImage("images/texture3.jpg");
    surfaceDisplaceIslandTex = new STTexture(surfaceDisplaceIslandImg);
    
	surfaceColorIslandImg = new STImage("images/texture3.jpg");
    surfaceColorIslandTex = new STTexture(surfaceColorIslandImg);
    
    
//    surfaceNormHutsImg = new STImage("images/huts.jpg");
//    surfaceNormHutsTex = new STTexture(surfaceNormHutsImg);
//    
//    surfaceDisplaceHutsImg = new STImage("images/huts.jpg");
//    surfaceDisplaceHutsTex = new STTexture(surfaceDisplaceHutsImg);
//    
//	surfaceColorHutsImg = new STImage("images/huts.jpg");
//    surfaceColorHutsTex = new STTexture(surfaceColorHutsImg);
//
//    shaderHuts = new STShaderProgram();
//    shaderHuts->LoadVertexShader(vertexShader);
//    shaderHuts->LoadFragmentShader(fragmentShader);

    resetCamera();
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    gTriangleMesh=new STTriangleMesh(meshOBJ);
    if(proxyType) gTriangleMesh->CalculateTextureCoordinatesViaSphericalProxy();
	else gTriangleMesh->CalculateTextureCoordinatesViaCylindricalProxy(-1,1,0,0,1);
    rock1=new STTriangleMesh("meshes/rock1.obj");
    rock1->CalculateTextureCoordinatesViaSphericalProxy();
    
    moon = new STTriangleMesh("meshes/sphere_fine.obj");
    
    moon->CalculateTextureCoordinatesViaSphericalProxy();
    CreateYourOwnMesh();
    island = new STTriangleMesh("meshes/island.obj");
    island->CalculateTextureCoordinatesViaSphericalProxy();
    //island->CalculateTextureCoordinatesViaCylindricalProxy(-40, 20, 2, 0, 1);
    shaderIsland = new STShaderProgram();

    huts = new STTriangleMesh("meshes/huts2.obj");
    //huts->CalculateTextureCoordinatesViaSphericalProxy();

    //boat = new STTriangleMesh("meshes/deadtree.obj");
    }

void CleanUp()
{
    if(gTriangleMesh!=0)
        delete gTriangleMesh;
    if(water!=0)
        delete water;
}

/**
 * Camera adjustment methods
 */
void AdjustCameraAzimuthBy(float delta)
{
    mCameraAzimuth += delta;
}

void AdjustCameraElevationBy(float delta)
{
    mCameraElevation += delta;
}

void AdjustCameraTranslationBy(STVector3 delta)
{
    mCameraTranslation += delta;
}

void skyTransformations(){
//    glScalef(60.f, 60.f, 60.f);
//    glTranslatef(0.f, -0.01f, -.7f);
//    glRotatef(-66.5, 1, 0, 0);
    
    glRotatef(65, 1, 0, 0);
    //glScalef(0.1f, 0.f, 0.f);
    glTranslatef(0.f, -4.f, -4.f);
}

void rockTransformations(){
    glTranslatef(0,-10, -40);
    glRotatef(10, 10, 19, 10);
    glTranslatef(1,-20, -40);
}

void moonTransformations(){
    glRotatef(60, 0, 1, 0);
    glRotatef(-10, 1, 0, 0);
    glScalef(3.2f, 3.2f, 3.2f);
    glTranslatef(2.2f, .5f, 0.f);
}

void hutTransformations(){
    glScalef(0.001f, 0.001f, 0.001f);
    glRotatef(-90, 0, 1, 0);
    glTranslatef(-1500.f, 600.f, 3000.f);
    glRotatef(20, 0, 0, 1);
}


void boatTransformations(){
    glTranslatef(0,0,0);
    glScalef(0.001f, 0.001f, 0.001f);

}
void islandTransformations(){
    glScalef(0.08f, 0.08f, 0.05f);
    glTranslatef(1.5f, 0.2f, 0.f);    //glRotatef(90.0f, 1, 0, 0);
    glRotatef(-90, 1, 0, 0);
    //glRotatef(20, 0, 1, 0);
    glRotatef(0, 0, 0, 1);
    glRotatef(-10, 0, 1, 1 );
    glRotatef(17, 0, 1, 0);

    glRotatef(5, 1,0, 0);

    glTranslatef(- 30.f, -4.f, -4.f);
}
//
// Display the output image from our vertex and fragment shaders
//
void DisplayCallback()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-mCameraTranslation.x * 0, -mCameraTranslation.y * 0, -mCameraTranslation.z);
    
    glRotatef(-mCameraElevation, 1, 0, 0);
    glRotatef(-mCameraAzimuth, 0, 1, 0);

    glRotatef(90.0f, 1, 0, 0);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    
    
    glMaterialfv(GL_FRONT, GL_AMBIENT,   materialSkyAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   materialSkyDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  materialSkySpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &shininessSky);
    
    //SKYYY
    // Texture 0: surface normal map
    glActiveTexture(GL_TEXTURE0);
    surfaceNormSkyTex->Bind();
    
    // Texture 1: surface normal map
    glActiveTexture(GL_TEXTURE1);
    surfaceDisplaceSkyTex->Bind();
    
    // Texture 2: surface color map
    glActiveTexture(GL_TEXTURE2);
    surfaceColorSkyTex->Bind();
    
    // Bind the textures we've loaded into openGl to
    // the variable names we specify in the fragment
    // shader.
    shaderSky->SetTexture("normalTex", 0);
    shaderSky->SetTexture("displacementTex", 1);
    shaderSky->SetTexture("colorTex", 2);
    
    // Invoke the shader.  Now OpenGL will call our
    // shader programs on anything we draw.
    shaderSky->Bind();
    
    shaderSky->SetUniform("displacementMapping", 1.0);
    shaderSky->SetUniform("normalMapping", -1.0);
    shaderSky->SetUniform("colorMapping", 1.0);
    
    glPushMatrix();
    skyTransformations();
    sky->Draw(smooth);
    glPopMatrix();
    
    shaderSky->UnBind();
    
    glActiveTexture(GL_TEXTURE0);
    surfaceNormSkyTex->UnBind();
    
    glActiveTexture(GL_TEXTURE1);
    surfaceDisplaceSkyTex->UnBind();
    
    glActiveTexture(GL_TEXTURE2);
    surfaceColorSkyTex->UnBind();
    

    // Texture 0: surface normal map
    glActiveTexture(GL_TEXTURE0);
    surfaceNormWaterTex->Bind();
    
    // Texture 1: surface normal map
    glActiveTexture(GL_TEXTURE1);
    surfaceDisplaceWaterTex->Bind();

	// Texture 2: surface color map
    glActiveTexture(GL_TEXTURE2);
    surfaceColorWaterTex->Bind();
    
    // Bind the textures we've loaded into openGl to
    // the variable names we specify in the fragment
    // shader.
    shaderWater->SetTexture("normalTex", 0);
    shaderWater->SetTexture("displacementTex", 1);
	shaderWater->SetTexture("colorTex", 2);
    
    // Invoke the shader.  Now OpenGL will call our
    // shader programs on anything we draw.
    shaderWater->Bind();
    
    glMaterialfv(GL_FRONT, GL_AMBIENT,   materialAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   materialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  materialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
    
    if(normalMapping){
        shaderWater->SetUniform("displacementMapping", -1.0);
        shaderWater->SetUniform("normalMapping", 1.0);
        shaderWater->SetUniform("colorMapping", 1.0);
    }
    else{
        shaderWater->SetUniform("displacementMapping", 1.0);
        shaderWater->SetUniform("normalMapping", -1.0);
        shaderWater->SetUniform("colorMapping", 1.0);
        shaderWater->SetUniform("TesselationDepth", TesselationDepth);
    }
    
    glTranslatef(0.f, -1.7f, 0.f);
    water->Draw(smooth);
        
    shaderWater->UnBind();
        
    glActiveTexture(GL_TEXTURE0);
    surfaceNormWaterTex->UnBind();
        
    glActiveTexture(GL_TEXTURE1);
    surfaceDisplaceWaterTex->UnBind();
        
    glActiveTexture(GL_TEXTURE2);
    surfaceColorWaterTex->UnBind();

    
    
    glMaterialfv(GL_FRONT, GL_AMBIENT,   materialMoonAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   materialMoonDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  materialMoonSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &shininessMoon);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Texture 0: surface normal map
    glActiveTexture(GL_TEXTURE0);
    surfaceNormMoonTex->Bind();
    
    // Texture 1: surface normal map
    glActiveTexture(GL_TEXTURE1);
    surfaceDisplaceMoonTex->Bind();
    
    // Texture 2: surface color map
    glActiveTexture(GL_TEXTURE2);
    surfaceColorMoonTex->Bind();
    
    // Bind the textures we've loaded into openGl to
    // the variable names we specify in the fragment
    // shader.
    shaderMoon->SetTexture("normalTex", 0);
    shaderMoon->SetTexture("displacementTex", 1);
    shaderMoon->SetTexture("colorTex", 2);
    
    // Invoke the shader.  Now OpenGL will call our
    // shader programs on anything we draw.
    shaderMoon->Bind();
    
    shaderMoon->SetUniform("displacementMapping", -1.0);
    shaderMoon->SetUniform("normalMapping", 1.0);
    shaderMoon->SetUniform("colorMapping", 1.0);
    
    glPushMatrix();
    moonTransformations();
    moon->Draw(smooth);
    glPopMatrix();
    
    shaderMoon->UnBind();
    
    glActiveTexture(GL_TEXTURE0);
    surfaceNormMoonTex->UnBind();
    
    glActiveTexture(GL_TEXTURE1);
    surfaceDisplaceMoonTex->UnBind();
    
    glActiveTexture(GL_TEXTURE2);
    surfaceColorMoonTex->UnBind();
    
    
//    // Texture 0: surface normal map
//    glActiveTexture(GL_TEXTURE0);
//    surfaceNormRock1Tex->Bind();
//    
//    // Texture 1: surface normal map
//    glActiveTexture(GL_TEXTURE1);
//    surfaceDisplaceRock1Tex->Bind();
//    
//    // Texture 2: surface color map
//    glActiveTexture(GL_TEXTURE2);
//    surfaceColorRock1Tex->Bind();
//    
//    // Bind the textures we've loaded into openGl to
//    // the variable names we specify in the fragment
//    // shader.
//    shaderRock1->SetTexture("normalTex", 0);
//    shaderRock1->SetTexture("displacementTex", 1);
//    shaderRock1->SetTexture("colorTex", 2);
//    
//    // Invoke the shader.  Now OpenGL will call our
//    // shader programs on anything we draw.
//    shaderRock1->Bind();
//    
//    shaderRock1->SetUniform("displacementMapping", 1.0);
//    shaderRock1->SetUniform("normalMapping", -1.0);
//    shaderRock1->SetUniform("colorMapping", 1.0);
//    
//    glPushMatrix();
//    rockTransformations();
//    rock1->Draw(smooth);
//    glPopMatrix();
//    
//    shaderRock1->UnBind();
//    
//    glActiveTexture(GL_TEXTURE0);
//    surfaceNormRock1Tex->UnBind();
//    
//    glActiveTexture(GL_TEXTURE1);
//    surfaceDisplaceRock1Tex->UnBind();
//    
//    glActiveTexture(GL_TEXTURE2);
//    surfaceColorRock1Tex->UnBind();

    
    glMaterialfv(GL_FRONT, GL_AMBIENT,   materialIslandAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   materialIslandDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  materialIslandSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &shiniessIsland);

    
    // Texture 0: surface normal map
    glActiveTexture(GL_TEXTURE0);
    surfaceNormIslandTex->Bind();
    
    // Texture 1: surface normal map
    glActiveTexture(GL_TEXTURE1);
    surfaceDisplaceIslandTex->Bind();
    
    // Texture 2: surface color map
    glActiveTexture(GL_TEXTURE2);
    surfaceColorIslandTex->Bind();
    
    // Bind the textures we've loaded into openGl to
    // the variable names we specify in the fragment
    // shader.
    shaderIsland->SetTexture("normalTex", 0);
    shaderIsland->SetTexture("displacementTex", 1);
    shaderIsland->SetTexture("colorTex", 2);
    
    // Invoke the shader.  Now OpenGL will call our
    // shader programs on anything we draw.
    shaderIsland->Bind();
    
    shaderIsland->SetUniform("displacementMapping", 1.0);
    shaderIsland->SetUniform("normalMapping", -1.0);
    shaderIsland->SetUniform("colorMapping", 1.0);
    
    glPushMatrix();
    islandTransformations();
    island->Draw(smooth);
    glPopMatrix();
    
    shaderIsland->UnBind();
    
    glActiveTexture(GL_TEXTURE0);
    surfaceNormIslandTex->UnBind();
    
    glActiveTexture(GL_TEXTURE1);
    surfaceDisplaceIslandTex->UnBind();
    
    glActiveTexture(GL_TEXTURE2);
    surfaceColorIslandTex->UnBind();
    
//    
//    // Texture 0: surface normal map
//    glActiveTexture(GL_TEXTURE0);
//    surfaceNormHutsTex->Bind();
//    
//    // Texture 1: surface normal map
//    glActiveTexture(GL_TEXTURE1);
//    surfaceDisplaceHutsTex->Bind();
//    
//    // Texture 2: surface color map
//    glActiveTexture(GL_TEXTURE2);
//    surfaceColorHutsTex->Bind();
//    
    // Bind the textures we've loaded into openGl to
    // the variable names we specify in the fragment
    // shader.
//    shaderHuts->SetTexture("normalTex", 0);
//    shaderHuts->SetTexture("displacementTex", 1);
//    shaderHuts->SetTexture("colorTex", 2);
//    
//    // Invoke the shader.  Now OpenGL will call our
//    // shader programs on anything we draw.
//    shaderHuts->Bind();
//    
//    shaderHuts->SetUniform("displacementMapping", -1.0);
//    shaderHuts->SetUniform("normalMapping", -1.0);
//    shaderHuts->SetUniform("colorMapping", 1.0);
    
    glPushMatrix();
    hutTransformations();
    huts->Draw(smooth);
    glPopMatrix();
    
//    shaderHuts->UnBind();
//    
//    glActiveTexture(GL_TEXTURE0);
//    surfaceNormHutsTex->UnBind();
//    
//    glActiveTexture(GL_TEXTURE1);
//    surfaceDisplaceHutsTex->UnBind();
//    
//    glActiveTexture(GL_TEXTURE2);
//    surfaceColorHutsTex->UnBind();
    
    glutSwapBuffers();
}

//
// Reshape the window and record the size so
// that we can use it for screenshots.
//
void ReshapeCallback(int w, int h)
{
	gWindowSizeX = w;
    gWindowSizeY = h;

    glViewport(0, 0, gWindowSizeX, gWindowSizeY);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
	// Set up a perspective projection
    float aspectRatio = (float) gWindowSizeX / (float) gWindowSizeY;
	gluPerspective(30.0f, aspectRatio, 0.1f, 10000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SpecialKeyCallback(int key, int x, int y)
{
    switch(key) {
        case GLUT_KEY_LEFT:
            AdjustCameraTranslationBy(STVector3(-0.2,0,0));
            break;
        case GLUT_KEY_RIGHT:
            AdjustCameraTranslationBy(STVector3(0.2,0,0));
            break;
        case GLUT_KEY_DOWN:
            AdjustCameraTranslationBy(STVector3(0,-0.2,0));
            break;
        case GLUT_KEY_UP:
            AdjustCameraTranslationBy(STVector3(0,0.2,0));
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void KeyCallback(unsigned char key, int x, int y)
{
    switch(key) {
    case 's': {
            //
            // Take a screenshot, and save as screenshot.jpg
            //
            STImage* screenshot = new STImage(gWindowSizeX, gWindowSizeY);
            screenshot->Read(0,0);
            screenshot->Save("screenshot.jpg");
            delete screenshot;
        }
        break;
    case 'r':
        resetCamera();
        break;
    case 'm': // switch between the mesh you create and the mesh from file
        mesh = !mesh;
        break;
	case 'p': //switch proxy type between sphere and cylinder
		proxyType=!proxyType;
		if(proxyType) gTriangleMesh->CalculateTextureCoordinatesViaSphericalProxy();
		else gTriangleMesh->CalculateTextureCoordinatesViaCylindricalProxy(-1,1,0,0,1);
		break;
    case 'n': // switch between normalMapping and displacementMapping
        normalMapping = !normalMapping;
        break;
    case 'f': // switch between smooth shading and flat shading
        smooth = !smooth;
        break;
    case 'l': // do loop subdivision
        if(mesh){
            gTriangleMesh->LoopSubdivide();
			if(proxyType) gTriangleMesh->CalculateTextureCoordinatesViaSphericalProxy();
			else gTriangleMesh->CalculateTextureCoordinatesViaCylindricalProxy(-1,1,0,0,1);
        }
        else
            water->LoopSubdivide();
        break;
	case 'q':
		exit(0);
    default:
        break;
    }

    glutPostRedisplay();
}


void usage()
{
	printf("usage: assignment3 vertShader fragShader objMeshFile normalMappingTexture displacementMappingTexture colorMappingTexture\n");
	exit(0);
}

int main(int argc, char** argv)
{
	if (argc != 7) {
        printf("%d", argc);
        for (int i = 0; i < argc; i++) {
            printf("%s\n", argv[i]);
        }
		usage();
    }

	vertexShader   = std::string(argv[1]);
	fragmentShader = std::string(argv[2]);
    meshOBJ        = std::string(argv[3]);
	normalMap      = std::string(argv[4]);
	displacementMap      = std::string(argv[5]);
	colorMap       =std::string(argv[6]);

    //
    // Initialize GLUT.
    //
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(20, 20);
    glutInitWindowSize(675, 480);
    glutCreateWindow("CS148 Texturing");
    
    //
    // Initialize GLEW.
    //
#ifndef __APPLE__
    glewInit();
    if(!GLEW_VERSION_2_0) {
        printf("Your graphics card or graphics driver does\n"
			   "\tnot support OpenGL 2.0, trying ARB extensions\n");

        if(!GLEW_ARB_vertex_shader || !GLEW_ARB_fragment_shader) {
            printf("ARB extensions don't work either.\n");
            printf("\tYou can try updating your graphics drivers.\n"
				   "\tIf that does not work, you will have to find\n");
            printf("\ta machine with a newer graphics card.\n");
            exit(1);
        }
    }
#endif

    // Be sure to initialize GLUT (and GLEW for this assignment) before
    // initializing your application.

    Setup();

    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);
    glutSpecialFunc(SpecialKeyCallback);
    glutKeyboardFunc(KeyCallback);
    glutIdleFunc(DisplayCallback);

    glutMainLoop();

    // Cleanup code should be called here.
    CleanUp();

    return 0;
}


/**
 * Mouse event handler
 */
//void MouseCallback(int button, int state, int x, int y)
//{
//    if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON)
//    {
//        gMouseButton = button;
//    } else
//    {
//        gMouseButton = -1;
//    }
//
//    if (state == GLUT_UP)
//    {
//        gPreviousMouseX = -1;
//        gPreviousMouseY = -1;
//    }
//}
//
///**
// * Mouse active motion callback (when button is pressed)
// */
//void MouseMotionCallback(int x, int y)
//{
//    if (gPreviousMouseX >= 0 && gPreviousMouseY >= 0)
//    {
//        //compute delta
//        float deltaX = x-gPreviousMouseX;
//        float deltaY = y-gPreviousMouseY;
//        gPreviousMouseX = x;
//        gPreviousMouseY = y;
//
//        float zoomSensitivity = 0.2f;
//        float rotateSensitivity = 0.5f;
//
//        //orbit or zoom
//        if (gMouseButton == GLUT_LEFT_BUTTON)
//        {
//            AdjustCameraAzimuthBy(-deltaX*rotateSensitivity);
//            AdjustCameraElevationBy(-deltaY*rotateSensitivity);
//
//        } else if (gMouseButton == GLUT_RIGHT_BUTTON)
//        {
//            STVector3 zoom(0,0,deltaX);
//            AdjustCameraTranslationBy(zoom * zoomSensitivity);
//        }
//
//    } else
//    {
//        gPreviousMouseX = x;
//        gPreviousMouseY = y;
//    }
//
//}
