#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "include/vecmath/vecmath.h"
using namespace std;

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;

//Added for optimization task
GLuint displayListId; //GLuint = unsigned int in OpenGL


// You will need more global variables to implement color and position changes
GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };

int i = 0; // global index
GLfloat Lt0pos[] = {1.0f, 1.0f, 5.0f, 1.0f}; // global position of the light;

// r to spin model
bool isSpinning = false;
float spinAngle = 0.0f;

//More smooth coloring
GLfloat currColor[4]={0.5f, 0.5f, 0.9f, 1.0f};
int prevColorIdx=0;
int nextColorIdx=0;
float colorBlend=1.0f;
bool isColorTransitioning=false;

//Camera attributes
float camYaw=0.0f; // left-right orbit degrees
float camPitch=20.0f; // up down orbit degrees
float camDist=5.0f; // distance from origin;

int lastMouseX, lastMouseY;
bool isDragging=false;
bool isRightDragging=false;


// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
// Inline - eliminates the function call overhead, which is good since these are called a lot in the rendering loop
//const guarantees that the function won't modify the input vector, and we have used pointer
//glVertex - Sets the position of a vertex in immediate mode
//glNormal - Sets the surface normal at the current vertex (used for lighting)
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }

void mouseFunc(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            isDragging=true;
            lastMouseX=x;
            lastMouseY=y;
        }
        else isDragging=false;
    }
    else if(button==GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            isRightDragging=true;
            lastMouseX=x;
            lastMouseY=y;
        }
        else isRightDragging=false;
    }
}

void motionFunc(int x, int y)
{
    int dx=x-lastMouseX;
    int dy=y-lastMouseY;
    if(isDragging)
    {
        camYaw-=dx*0.5f;
        camPitch+=dy*0.5f;
        if(camPitch > 89.0f) camPitch = 89.0f;
        if(camPitch < -89.0f) camPitch = -89.0f;
    }
    else if(isRightDragging)
    {
        camDist+=dy*0.5f;
        if(camDist < 1.0f) camDist = 1.0f;
    }
    lastMouseX=x;
    lastMouseY=y;
    glutPostRedisplay();
}

void mouseWheelFunc(int wheel, int direction, int x, int y)
{
    camDist -= direction * 0.3f;
    if (camDist < 1.0f) camDist = 1.0f;
    glutPostRedisplay();
}

void colorTimerFunc(int value)
{
    if(isColorTransitioning)
    {
        colorBlend+=0.05f;
        if(colorBlend>=1.0f)
        {
            colorBlend=1.0f;
            isColorTransitioning=false;
        }
        for(int k=0; k<4; k++)
        {
            currColor[k]=diffColors[prevColorIdx][k]*(1.0f-colorBlend)+diffColors[nextColorIdx][k]*colorBlend;
        }
        glutPostRedisplay();
        if(isColorTransitioning) glutTimerFunc(16, colorTimerFunc, 0);
    }
}

void timerFunc(int value)
{
    if(isSpinning)
    {
        spinAngle+=2.0f;
        if(spinAngle>=360.0f) spinAngle-=360.0f;
        glutPostRedisplay();
        glutTimerFunc(16, timerFunc, 0);
    }
}

// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
        //i = (i + 1) % 4;
		//cout << "The C was pressed for chaging color " << key << "." << endl; 
        //break;
        //New way to change the color
        prevColorIdx=nextColorIdx;
        nextColorIdx=(nextColorIdx+1)%4;
        colorBlend=0.0f;
        isColorTransitioning=true;
        glutTimerFunc(16, colorTimerFunc, 0);
        cout<<"Transitioning to color "<<nextColorIdx<<endl;
        break;
    case 'r':
        isSpinning=!isSpinning;
        cout<<"Spinnig: "<<(isSpinning ? "ON" : "OFF")<<endl;
        if(isSpinning) glutTimerFunc(16, timerFunc, 0);
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
        Lt0pos[1] += 0.5f;
		cout << "Up Arrow" << endl;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
        Lt0pos[1] -= 0.5f;
		cout << "Down Arrow" << endl;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
        Lt0pos[0] -= 0.5f;
		cout << "Left Arrow" << endl;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
        Lt0pos[0] += 0.5f;
		cout << "Right Arrow" << endl;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}



// This function is responsible for displaying the object.
void drawScene(void)
{
    //int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    //gluLookAt(0.0, 0.0, 5.0,
              //0.0, 0.0, 0.0,
              //0.0, 1.0, 0.0);

    float yawRad   = camYaw   * 3.14159265f / 180.0f;
    float pitchRad = camPitch * 3.14159265f / 180.0f;

    float camX = camDist * cos(pitchRad) * sin(yawRad);
    float camY = camDist * sin(pitchRad);
    float camZ = camDist * cos(pitchRad) * cos(yawRad);

    gluLookAt(camX, camY, camZ,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);

    glRotatef(spinAngle, 0.0f, 1.0f, 0.0f);// y axis rotation

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    // GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
    //                              {0.9, 0.5, 0.5, 1.0},
    //                              {0.5, 0.9, 0.3, 1.0},
    //                              {0.3, 0.8, 0.9, 1.0} };
    
	// Here we use the first color entry as the diffuse color
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[i]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, currColor);
	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
	//GLfloat Lt0pos[] = {1.0f, 1.0f, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	//glutSolidTeapot(1.0);
    
    //Instead of teapot, code for loading OBJ file.
    //glBegin(GL_TRIANGLES);
    //for(unsigned int k = 0; k<vecf.size(); k++)
    //{
        //vector<unsigned> &face=vecf[k];
        //for(int corner=0; corner<3; corner++)
        //{
            //unsigned vIdx = face[corner*2];
            //unsigned nIdx = face[corner*2+1];
            //glNormal(vecn[nIdx-1]);
            //glVertex(vecv[vIdx-1]);
        //}
    //}
    //glEnd();

    glCallList(displayListId); //Entire stuff will be drawn by it
    
    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION); // Defines how the 3D scene is projected onto 2D Screen, GL_MODELVIEW defines object positions and camera
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

#include <fstream>

//Newly Added Function
void loadInput()
{
    char buffer[1024]; //each line of the file
    while(cin.getline(buffer, 1024))
    {
        stringstream ss(buffer);
        string s;
        ss>>s;
        if(s == "v")
        {
            Vector3f v;
            ss>>v[0]>>v[1]>>v[2];
            vecv.push_back(v);
        }
        else if(s == "vn")
        {
            Vector3f n;
            ss>>n[0]>>n[1]>>n[2];
            vecn.push_back(n);
        }
        else if(s == "f")
        {
            vector<unsigned> face;
            for(int k=0; k<3; k++)
            {
                string group;
                ss>>group;
                for(size_t p=0; p<group.size(); p++)
                {
                    if(group[p] == '/') group[p] = ' ';
                }
                stringstream gs(group);
                unsigned a, b, c;
                gs>>a>>b>>c;
                face.push_back(a);
                face.push_back(c);
            }
            vecf.push_back(face);
        }
    }
}

void buildDisplayList()
{
    displayListId = glGenLists(1);
    glNewList(displayListId, GL_COMPILE);

    glBegin(GL_TRIANGLES);
    for(unsigned int k=0; k<vecf.size(); k++)
    {
        vector<unsigned> & face=vecf[k];
        for(int corner=0; corner<3; corner++)
        {
            unsigned vIdx=face[corner*2];
            unsigned nIdx=face[corner*2+1];
            glNormal(vecn[nIdx-1]);
            glVertex(vecv[vIdx-1]);
        }
    }
    glEnd();
    glEndList();
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    cout << "Step 1: Program Started" << endl;
    loadInput();
    cout << "Step 2: Input Loaded" << endl;
    //buildDisplayList(); // For optimization (wrong place)
    //Must be the first GLUT call — everything else depends on it.
    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    buildDisplayList(); //Correct place

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);
    glutMouseWheelFunc(mouseWheelFunc);
     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
