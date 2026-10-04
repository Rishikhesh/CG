// DDA line drawing: click two points, the line is drawn between them. Click again to start a new line.
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#define SCREEN_HEIGHT 700
#define SCREEN_WIDTH 1000
using namespace std;


vector< pair<float,float> > points;
void init()
{
    glClearColor(0,0,0,0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,SCREEN_WIDTH,0,SCREEN_HEIGHT);
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3d(1,0,0);
    glFlush();
}

void plotPoint(float x,float y)
{
    glColor3d(1,0,0);
    glBegin(GL_POINTS);
        glVertex2f(x,y);
    glEnd();
    glFlush();
}

// Step along the longer axis one pixel at a time; the other coordinate grows by
// dy/steps (or dx/steps) and is rounded. Works for every slope and direction.
void DDA()
{
    float dx = points[1].first - points[0].first;
    float dy = points[1].second - points[0].second;
    int steps = max(fabs(dx), fabs(dy));
    if(steps == 0)
    {
        plotPoint(points[0].first, points[0].second);
        return;
    }
    float xinc = dx / steps, yinc = dy / steps;
    float x = points[0].first, y = points[0].second;

    for(int i = 0; i <= steps; i++)
    {
        plotPoint(round(x),round(y));
        x += xinc;
        y += yinc;
    }
}

void mouseHandle(int button ,int status, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON &&  status == GLUT_DOWN)
    {
        if(points.size() == 2)
            points.clear();
        pair<float,float> p;
        p.first = x;
        p.second = SCREEN_HEIGHT - y;

        plotPoint(p.first,p.second);
        cout<<"X: "<<p.first<<" Y: "<<p.second<<endl;

        points.push_back(p);
        if(points.size() == 2)
            DDA();
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(SCREEN_WIDTH,SCREEN_HEIGHT);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    glutCreateWindow("DDA");
    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouseHandle);

    glutMainLoop();

    return EXIT_SUCCESS;
}
