// Liang-Barsky line clipping: left-click the line's two ends, right-click two opposite corners
// of the clip window. The part of the line inside the window is redrawn in green.
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#define SCREEN_HEIGHT 700
#define SCREEN_WIDTH 1000
using namespace std;

vector< pair<int,int> > points;
vector< pair<int,int> > window;
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

void plotPoints(int x,int y)
{
    glBegin(GL_POINTS);
        glVertex2f(x,y);
    glEnd();
    glFlush();
}

void drawLine()
{
    glBegin(GL_LINES);
        glVertex2f(points[0].first,points[0].second);
        glVertex2f(points[1].first,points[1].second);
    glEnd();
    glFlush();
}

void drawLine(int x1, int y1, int x2, int y2)
{
    glColor3f(0,1,0);
    glBegin(GL_LINES);
        glVertex2f(x1,y1);
        glVertex2f(x2,y2);
    glEnd();
    glFlush();
}

void drawWindow()
{
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glBegin(GL_POLYGON);
        glVertex2d(window[0].first,window[0].second);
        glVertex2d(window[0].first,window[1].second);
        glVertex2d(window[1].first,window[1].second);
        glVertex2d(window[1].first,window[0].second);
    glEnd();
    glFlush();
}

// The line is P(u) = P0 + u*(P1-P0), u in [0,1]. Each window edge gives p*u <= q.
// p < 0: the line enters through that edge  -> u1 = max(u1, q/p)
// p > 0: the line leaves through that edge  -> u2 = min(u2, q/p)
// p == 0: parallel to the edge; q < 0 means fully outside it.
void LBC()
{
    float xmin = min(window[0].first, window[1].first), xmax = max(window[0].first, window[1].first);
    float ymin = min(window[0].second, window[1].second), ymax = max(window[0].second, window[1].second);
    float delx = points[1].first - points[0].first, dely = points[1].second - points[0].second;
    float p[4] = {-delx, delx, -dely, dely};
    float q[4] = {points[0].first - xmin, xmax - points[0].first,
                  points[0].second - ymin, ymax - points[0].second};

    float u1=0,u2=1;
    for(int i=0;i<4;i++)
    {
        if(p[i] == 0)
        {
            if(q[i] < 0) return;   // parallel and outside: nothing to draw
        }
        else if(p[i] < 0) u1 = max(u1,q[i]/p[i]);
        else u2 = min(u2,q[i]/p[i]);
    }

    cout<<"u1: "<<u1<<" u2: "<<u2<<endl;
    if(u1<u2)
    {
        float x1 = points[0].first + delx*u1;
        float y1 = points[0].second + dely*u1;

        float x2 = points[0].first + delx*u2;
        float y2 = points[0].second + dely*u2;

        cout<<"x1: "<<x1<<" y1:"<<y1<<" x2:"<<x2<<" y2:"<<y2<<endl;
        drawLine(x1,y1, x2,y2);
    }
}


void mouse_handle(int button, int status, int x, int y)
{
    if(status != GLUT_DOWN)
        return;
    pair<int,int> p(x, SCREEN_HEIGHT - y);
    glColor3d(1,0,0);
    plotPoints(p.first,p.second);

    if(button == GLUT_LEFT_BUTTON && points.size() < 2)
    {
        points.push_back(p);
        if(points.size() == 2)
            drawLine();
    }
    else if(button == GLUT_RIGHT_BUTTON && window.size() < 2)
    {
        cout<<"w.x: "<<p.first<<" w.y: "<<p.second<<endl;
        window.push_back(p);
        if(window.size() == 2)
            drawWindow();
    }

    if(points.size() == 2 && window.size() == 2)
    {
        LBC();
        points.clear();   // next clicks start a new line and window
        window.clear();
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(50,50);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    glutCreateWindow("Liang Clipping");
    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse_handle);
    glutMainLoop();

    return EXIT_SUCCESS;
}
