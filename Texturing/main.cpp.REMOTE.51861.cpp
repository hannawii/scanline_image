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
static float specularLight[] = {1.5, 1.5, 1.5, 1.5};
static float ambientLight[]  = {0.60, 0.60, 0.60, 1.5};
static float diffuseLight[]  = {1.20, 1.20, 1.20, 1.5};

float lightPosition[] = {10.0f, 15.0f, 10.0f, 1.0f};

// Material color properties
static float materialAmbient[]  = { 0.2, 0.2, 0.6, 1.0 };
static float materialDiffuse[]  = { 0.2, 0.2, 0.6, 1.0 };
static float materialSpecular[] = { 0.8, 0.8, 0.8, 1.0 };
static float shininess          = 8.0;  // # between 1 and 128.

static float material2Ambient[]  = { 1., 1., 1., 1. };
static float material2Diffuse[]  = { 1., 1., 1., 1. };
static float material2Specular[] = { 1., 1., 1., 1. };
static float shininess2          = 1.;  // # between 1 and 128.

STImage   *surfaceNormImg;
STTexture *surfaceNormTex;

STImage   *surfaceDisplaceImg;
STTexture *surfaceDisplaceTex;

STImage   *surfaceColorImg;
STTexture *surfaceColorTex;

STShaderProgram *shader;

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
STTriangleMesh* rock1 = 0;

int TesselationDepth = 100;

void resetCamera()
{
    mCameraTranslation = STVector3(0.f, 0.f, 15.f);
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

    surfaceNormImg = new STImage(normalMap);
    surfaceNormTex = new STTexture(surfaceNormImg);
    
    surfaceDisplaceImg = new STImage(displacementMap);
    surfaceDisplaceTex = new STTexture(surfaceDisplaceImg);

	surfaceColorImg = new STImage(colorMap);
    surfaceColorTex = new STTexture(surfaceColorImg);
    
    shader = new STShaderProgram();
    shader->LoadVertexShader(vertexShader);
    shader->LoadFragmentShader(fragmentShader);

    resetCamera();
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    gTriangleMesh=new STTriangleMesh(meshOBJ);
    if(proxyType) gTriangleMesh->CalculateTextureCoordinatesViaSphericalProxy();
	else gTriangleMesh->CalculateTextureCoordinatesViaCylindricalProxy(-1,1,0,0,1);
    rock1=new STTriangleMesh("meshes/rock1.obj");
    rock1->CalculateTextureCoordinatesViaSphericalProxy();
    CreateYourOwnMesh();
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

void rockTransformations(){
    glTranslatef(0,-10, -40);
    
}
//
// Display the output image from our vertex and fragment shaders
//
void DisplayCallback()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-mCameraTranslation.x, -mCameraTranslation.y, -mCameraTranslation.z);
    
    glRotatef(-mCameraElevation, 1, 0, 0);
    glRotatef(-mCameraAzimuth, 0, 1, 0);

    glRotatef(90.0f, 1, 0, 0);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Texture 0: surface normal map
    glActiveTexture(GL_TEXTURE0);
    surfaceNormTex->Bind();
    
    // Texture 1: surface normal map
    glActiveTexture(GL_TEXTURE1);
    surfaceDisplaceTex->Bind();

	// Texture 2: surface color map
    glActiveTexture(GL_TEXTURE2);
    surfaceColorTex->Bind();
    
    // Bind the textures we've loaded into openGl to
    // the variable names we specify in the fragment
    // shader.
    shader->SetTexture("normalTex", 0);
    shader->SetTexture("displacementTex", 1);
	shader->SetTexture("colorTex", 2);
    
    // Invoke the shader.  Now OpenGL will call our
    // shader programs on anything we draw.
    shader->Bind();
    
    glMaterialfv(GL_FRONT, GL_AMBIENT,   materialAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   materialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  materialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
    
    if(normalMapping){
        shader->SetUniform("displacementMapping", -1.0);
        shader->SetUniform("normalMapping", 1.0);
        shader->SetUniform("colorMapping", 1.0);
    }
    else{
        shader->SetUniform("displacementMapping", 1.0);
        shader->SetUniform("normalMapping", -1.0);
        shader->SetUniform("colorMapping", 1.0);
        shader->SetUniform("TesselationDepth", TesselationDepth);
    }
    
    glTranslatef(0.f, -1.5f, 0.f);
    //water->Draw(smooth);

    glPushMatrix();
    rockTransformations();
    rock1->Draw(smooth);
    glPopMatrix();
    

		//change the material to be white color for texturing the world map on the sphere
		//if you do not want to change the material color, you do not need to put glMaterialfv functions here.
		glMaterialfv(GL_FRONT, GL_AMBIENT,   material2Ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   material2Diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  material2Specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, &shininess2);

        shader->SetUniform("normalMapping", -1.0);
        shader->SetUniform("displacementMapping", -1.0);
		shader->SetUniform("colorMapping", 1.0);
        //gTriangleMesh->Draw(smooth);

    shader->UnBind();
    
    glActiveTexture(GL_TEXTURE0);
    surfaceNormTex->UnBind();
    
    glActiveTexture(GL_TEXTURE1);
    surfaceDisplaceTex->UnBind();

	glActiveTexture(GL_TEXTURE2);
    surfaceColorTex->UnBind();
    
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
	gluPerspective(30.0f, aspectRatio, .1f, 10000.0f);

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
    glutInitWindowSize(640, 480);
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
