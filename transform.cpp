// 2D transforms: left-click the polygon's corners, right-click to finish, then use the keys:
// w a s d move, k/l scale up/down, r/t rotate, v/b/n reflect about x / y / origin,
// h shear in x, p reflect about the line y = x + 2, Esc quit.
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

#define CENTER_X 320
#define CENTER_Y 240

#define SCREEN_HEIGHT 480
#define SCREEN_WIDTH 640

float arrx[100], arry[100];


int cnt =0;

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3d(1,1,1);
    glBegin(GL_LINES);
        glVertex2f(0,CENTER_Y);
        glVertex2f(SCREEN_WIDTH,CENTER_Y);
        glVertex2f(CENTER_X,0);
        glVertex2f(CENTER_X,SCREEN_HEIGHT);
    glEnd();
    glBegin(GL_POLYGON);
    for(int i=0;i<cnt;i++)
    {
        glVertex2f(arrx[i],arry[i]);
    }
    glEnd();
    glFlush();
}

void translate(int x, int y)
{
    for(int i=0;i<cnt;i++)
    {
        arrx[i] = arrx[i]+x;
        arry[i] = arry[i]+y;
    }
}

void rotation(float theta)
{
    float cx = arrx[0];
    float cy = arry[0];
    theta = theta*(2*M_PI/360.0);
    for(int i=0;i<cnt;i++)
    {
        arrx[i] = arrx[i] - cx;
        arry[i] = arry[i] - cy;
    }
    for(int i=0;i<cnt;i++)
    {
        float x = arrx[i];
        float y = arry[i];
        arrx[i] = x*cos(theta)-y*sin(theta);
        arry[i] = x*sin(theta)+y*cos(theta);
    }
    for(int i=0;i<cnt;i++)
    {
        arrx[i] = arrx[i]+cx;
        arry[i] = arry[i]+cy;
    }
}

void reflectx()
{
    for(int i=0;i<cnt;i++)
    {
      arry[i] = SCREEN_HEIGHT-arry[i];
    }
}

void reflecty()
{
    for(int i=0;i<cnt;i++)
    {
      arrx[i] = SCREEN_WIDTH-arrx[i];
    }
}
void reflectxy()
{
    for(int i=0;i<cnt;i++)
    {
        arrx[i] = SCREEN_WIDTH-arrx[i];
        arry[i] = SCREEN_HEIGHT-arry[i];
    }
}

void shearingx(float shx)
{
    float cx = arrx[0];
    float cy = arry[0];
    for(int i=0;i<cnt;i++)
    {
        arrx[i] = arrx[i] - cx;
        arry[i] = arry[i] - cy;
    }
    for(int i=0;i<cnt;i++)
    {
        arrx[i] = arrx[i]+ shx*arry[i];
    }
    for(int i=0;i<cnt;i++)
    {
        arrx[i] = arrx[i]+cx;
        arry[i] = arry[i]+cy;
    }
}

void scale(float sx, float sy)
{
    float cx = arrx[0];
    float cy = arry[0];
    for(int i=0;i<cnt;i++)
    {
        arrx[i] = arrx[i] - cx;
        arry[i] = arry[i] - cy;
    }
    for(int i=0;i<cnt;i++)
    {
        arrx[i] = arrx[i]* sx;
        arry[i] = arry[i]* sy;
    }
    for(int i=0;i<cnt;i++)
    {
        arrx[i] = arrx[i] + cx;
        arry[i] = arry[i] + cy;
    }
}

// reflection about the line y = m*x + c, measured from the drawn axes (origin at the centre)
void reflectmx(float m,float c)
{
    float x,y,d = m*m + 1;
    for(int i=0;i<cnt;i++)
    {
        x = arrx[i] - CENTER_X;
        y = arry[i] - CENTER_Y;
        arrx[i] = (x*(1-m*m) + 2*m*y - 2*m*c)/d + CENTER_X;
        arry[i] = (2*m*x + y*(m*m-1) + 2*c)/d + CENTER_Y;
    }
}

void key(unsigned char k, int x, int y)
{
    cout<<"Key:"<<k<<endl;
    cout<<x<<" "<<y<<endl;
    switch (k)
    {
    case 'w':
        translate(0,10);
        break;
    case 'a':
        translate(-10,0);
        break;
    case 'd':
        translate(10,0);
        break;
    case 's':
        translate(0,-10);
        break;
    case 'k':
        scale(2,2);
        break;
    case 'l':
        scale(0.5,0.5);
        break;
    case 'r':
        rotation(5);
        break;
    case 't':
        rotation(-5);
        break;
    case 'v':
        reflectx();
        break;
    case 'b':
        reflecty();
        break;
    case 'n':
        reflectxy();
        break;
    case 'h':
        shearingx(0.25);
        break;
    case 'p':
        reflectmx(1,2);
        break;
    case 27:
      exit(0);
      break;
    default:
      break;
    }
    glutPostRedisplay();
}
static void mouse(int button, int status, int x, int y)
{
    if(status != GLUT_DOWN) // GLUT reports both press and release; only count the press
        return;
    if(button==GLUT_LEFT_BUTTON && cnt < 100)
    {
        cout<<x<<","<<y<<endl;
        y = SCREEN_HEIGHT-y;
        glBegin(GL_POINTS);
            glVertex2f( x,y);
        glEnd();

        arrx[cnt] = x;
        arry[cnt] = y;
        cnt++;
    }
    if(button ==GLUT_RIGHT_BUTTON)
    {
        glutKeyboardFunc(key);
        glutPostRedisplay();
    }
    glFlush();
}


void init()
{
    glClearColor(0.5,0.5,0.5,0.5);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,SCREEN_WIDTH,0,SCREEN_HEIGHT);
}


int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(SCREEN_WIDTH,SCREEN_HEIGHT);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    glutCreateWindow("Transforms");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMainLoop();

    return EXIT_SUCCESS;
}
