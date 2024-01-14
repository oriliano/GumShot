/*********

----------
PROBLEMS:
----------
ADDITIONAL FEATURES:
1- There's a time bar as a second counter.
2- I worked so hard for the background I think it can be counted as additional feature.
3- There's a section to count hits.
4- When the game is over it displays the total points you get.
5- Character cannot leave the gamescreen.
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  12 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define TARGET_RADIUS  40


int  sec = 19, salise = 99, beg = 0, disphit = 0, totalHit = 0, cntr = 50;


/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false;
int  winWidth, winHeight; // current Window width and height

bool activeTimer = false;
bool fire = false;
bool hide = false;
bool end = false;


typedef struct {
    float x, y;
} point_t;

typedef struct {
    point_t pos;   // position of the object
} player_t;

typedef struct {
    point_t pos;
    bool active;
} fire_t;

typedef struct {
    point_t center;
    float radius;
    float speed;
} target_t;


#define MAX_FIRE 20 // 20 fires at a time.
#define FIRE_RATE 45 // after 8 frames you can throw another one.

fire_t fr[MAX_FIRE];
player_t p = { {0,-200} };
target_t target[5];
int fire_rate = 0;





// mouse position
point_t mouse;

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void pie_wire(float x, float y, float r, float start, float end) {
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    for (float angle = start; angle < end; angle += 10) {
        glVertex2f(r * cos(angle * D2R) + x, r * sin(angle * D2R) + y);
    }
    glVertex2f(r * cos(end * D2R) + x, r * sin(end * D2R) + y);
    glEnd();
}


void pie_filled(float x, float y, float r, float start, float end) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    for (float angle = start; angle < end; angle += 10) {
        glVertex2f(r * cos(angle * D2R) + x, r * sin(angle * D2R) + y);
    }
    glVertex2f(r * cos(end * D2R) + x, r * sin(end * D2R) + y);
    glEnd();
}

void spiral(float x, float y, float r, float start, float end, float offset) {
    glBegin(GL_LINE_STRIP);
    for (float angle = start; angle < end; angle += 10) {
        r -= offset;
        glVertex2f(r * cos(angle * D2R) + x, r * sin(angle * D2R) + y);
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

void brick() {
    glColor3f(0.3, 0.3, 0.3);
    glRectf(-750, -450, 750, 450);

    glColor3ub(0, 0, 0);
    glBegin(GL_LINES);
    for (int height = 600; height >= -600; height -= 30) {
        glVertex2f(-750, height);
        glVertex2f(750, height);
    }

    for (int width = 750; width >= -750; width -= 60) {
        for (int height = 420; height >= -420; height -= 60) {
            glVertex2f(width, height);
            glVertex2f(width, height - 30);
        }
    }

    for (int width = 730; width >= -730; width -= 60) {
        for (int height = 450; height >= -450; height -= 60) {
            glVertex2f(width, height);
            glVertex2f(width, height - 30);
        }
    }

    glEnd();
    glLineWidth(2.0f);
}

void brickleft() {
    glColor3f(0.3, 0.3, 0.3);
    glRectf(-750, -450, -604, 450);

    glColor3ub(0, 0, 0);
    glBegin(GL_LINES);
    for (int height = 600; height >= -600; height -= 30) {
        glVertex2f(-750, height);
        glVertex2f(-604, height);
    }

    for (int width = -630; width >= -750; width -= 60) {
        for (int height = 420; height >= -420; height -= 60) {
            glVertex2f(width, height);
            glVertex2f(width, height - 30);
        }
    }

    for (int width = -610; width >= -730; width -= 60) {
        for (int height = 450; height >= -450; height -= 60) {
            glVertex2f(width, height);
            glVertex2f(width, height - 30);
        }
    }

    glEnd();
    glLineWidth(2.0f);

}

void brickright() {
    glColor3f(0.3, 0.3, 0.3);
    glRectf(604, -450, 750, 450);

    glColor3ub(0, 0, 0);
    glBegin(GL_LINES);
    for (int height = 600; height >= -600; height -= 30) {
        glVertex2f(604, height);
        glVertex2f(750, height);
    }

    for (int width = 750; width >= 630; width -= 60) {
        for (int height = 420; height >= -420; height -= 60) {
            glVertex2f(width, height);
            glVertex2f(width, height - 30);
        }
    }

    for (int width = 730; width >= 610; width -= 60) {
        for (int height = 450; height >= -450; height -= 60) {
            glVertex2f(width, height);
            glVertex2f(width, height - 30);
        }
    }

    glEnd();
    glLineWidth(2.0f);

    //gamescreen
    
    glColor3f(0, 0, 0);
    glRectf(-604, -254, -600, 385);

    glColor3f(0, 0, 0);
    glRectf(604, -254, 600, 385);
}

void gameScreen() {

    glColor3f(0, 0, 0);
    glRectf(-604, -254, 604, 385);

    for (int i = 1; i <= 630; i++) {
        glColor3ub(125 + i / 8, 100 - i / 8, 255 - i / 3);
        glBegin(GL_LINES);

        glVertex2f(-600, -250 + i);
        glVertex2f(600, -250 + i);
        glEnd();

    }
}

void bubbles(target_t* t) {


    glColor3f(203. / 255, 206. / 255, 207. / 255);
    circle(t[0].center.x, t[0].center.y, 40);

    glColor3f(0.7, 0.7, 0.7);
    circle(t[0].center.x -20, t[0].center.y + 15, 8);

    glColor3f(0.7, 0.7, 0.7);
    circle(t[0].center.x + 20, t[0].center.y - 15, 8);

    glColor3f(0.7, 0.7, 0.7);
    circle(t[0].center.x - 5, t[0].center.y - 3, 5);

    glColor3f(0.7, 0.7, 0.7);
    circle(t[0].center.x - 25, t[0].center.y - 6, 5);

    glColor3f(0.7, 0.7, 0.7);
    circle(t[0].center.x + 5, t[0].center.y + 3, 3);

    glColor3f(0.7, 0.7, 0.7);
    circle(t[0].center.x + 1, t[0].center.y + 25, 4);

    glColor3f(0.7, 0.7, 0.7);
    circle(t[0].center.x + 25, t[0].center.y + 6, 5);





    glColor3f(255 / 255.0, 188 / 255., 85 / 255.);
    circle(t[3].center.x, t[3].center.y, 40);

    glColor3f(255 / 255.0, 231 / 255., 68 / 255.);
    circle(t[3].center.x, t[3].center.y, 30);





    glColor3f(1, 1, 1);
    pie_filled(t[2].center.x, t[2].center.y, 40, 0, 90);

    glColor3f(1, 0, 0);
    pie_filled(t[2].center.x, t[2].center.y, 40, 90, 180);

    glColor3f(1, 1, 1);
    pie_filled(t[2].center.x, t[2].center.y, 40, 180, 270);

    glColor3f(1, 0, 0);
    pie_filled(t[2].center.x, t[2].center.y, 40, 270, 360);


    





   
    glColor3f(1, 1, 0);
    pie_filled(t[1].center.x, t[1].center.y, 40, 30, 330);

    glColor3f(0, 0, 0);
    circle(t[1].center.x + 10, t[1].center.y + 25, 5);









    glColor3f(1, 1, 0);
    circle(t[4].center.x, t[4].center.y, 40);

    glColor3f(0, 0, 0);
    circle_wire(t[4].center.x, t[4].center.y + 3, 28);

    glColor3f(0, 0, 0);
    circle_wire(t[4].center.x, t[4].center.y + 3, 27);

    glColor3f(1, 1, 0);
    pie_filled(t[4].center.x, t[4].center.y, 40, -10, 190);

    glColor3f(0, 0, 0);
    circle(t[4].center.x - 10, t[4].center.y + 15, 5);

    glColor3f(0, 0, 0);
    circle(t[4].center.x + 10, t[4].center.y + 15, 5);

}

void devil(player_t p) {

    glColor3f(0, 0, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(p.pos.x -20, -120);
    glVertex2f(p.pos.x -15, -145);
    glVertex2f(p.pos.x -40, -110);
    glEnd();

    glColor3f(0, 0, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(p.pos.x + 20, -120);
    glVertex2f(p.pos.x + 15, -145);
    glVertex2f(p.pos.x + 40, -110);
    glEnd();

    glColor3f(0, 0, 0);
    circle(p.pos.x, -140, 31);

    glColor3f(0.8, 0.2, 0.2);
    circle(p.pos.x, -140, 30);

    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(p.pos.x - 40, -165);
    glVertex2f(p.pos.x + 40, -165);
    glVertex2f(p.pos.x + 50, -240);
    glVertex2f(p.pos.x - 50, -240);
    glEnd();

    glColor3f(1, 0.9, 0);
    glBegin(GL_QUADS);
    glVertex2f(p.pos.x - 35, -240);
    glVertex2f(p.pos.x -10, -240);
    glVertex2f(p.pos.x - 10, -250);
    glVertex2f(p.pos.x - 45, -250);
    glEnd();

    glColor3f(1, 0.9, 0);
    glBegin(GL_QUADS);
    glVertex2f(p.pos.x + 35, -240);
    glVertex2f(p.pos.x + 10, -240);
    glVertex2f(p.pos.x + 10, -250);
    glVertex2f(p.pos.x + 45, -250);
    glEnd();

    glColor3f(0.8, 0.2, 0.2);
    circle(p.pos.x + 87, -125, 10);

    glColor3f(0, 0, 0);
    glRectf(p.pos.x + 40, -188, p.pos.x +95, -170);

    glColor3f(0, 0, 0);
    glRectf(p.pos.x + 80, -130, p.pos.x + 95, -170);

    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(p.pos.x +87, -125);
    glVertex2f(p.pos.x + 110, -123);
    glVertex2f(p.pos.x + 107, -110);
    glVertex2f(p.pos.x + 84, -113);
    glEnd();

    glColor3f(0, 0, 0.4);
    circle(p.pos.x + 105, -115, 8);

    glColor3f(0, 0, 0);
    glRectf(p.pos.x + 103, -115, p.pos.x + 107, -100);

    glColor3f(0, 0, 0);
    glRectf(p.pos.x + 100, -95, p.pos.x + 110, -100);

    glColor3f(0, 0, 0);
    glRectf(p.pos.x + 102, -95, p.pos.x + 108, -80);

    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(p.pos.x + 100, -80);
    glVertex2f(p.pos.x + 110, -80);
    glVertex2f(p.pos.x + 108, -76);
    glVertex2f(p.pos.x + 102, -76);
    glEnd();


    

    


}

void ammo() {

    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active) {

            glColor3f(0.3, 0.3, 0.3);
            glBegin(GL_TRIANGLES);
            glVertex2f(fr[i].pos.x, fr[i].pos.y);
            glVertex2f(fr[i].pos.x + 8, fr[i].pos.y - 15);
            glVertex2f(fr[i].pos.x - 8, fr[i].pos.y - 15);
            glEnd();

            glColor3f(0.4, 0.4, 0.4);
            glRectf(fr[i].pos.x - 2, fr[i].pos.y - 15, fr[i].pos.x + 2, fr[i].pos.y - 50);

            
        }
    }



}

void info() {

    glColor3f(0.6, 0.6, 0.6);
    glRectf(-477, -268, +477, -302);

    glColor3f(0.4, 0.4, 0.4);
    glRectf(-475, -270, +475, -300);

    glColor3f(1, 0, 0);
    glRectf(-475, -270, (-475 + (sec) * cntr), -300);

    glColor3f(0.6, 0.6, 0.6);
    glRectf(-102, -328, +102, -392);

    glColor3f(0.4, 0.4, 0.4);
    glRectf(-100, -330, +100, -390);

    glColor3ub(240, 240, 240);
    vprint(-80, -368, GLUT_BITMAP_TIMES_ROMAN_24, "TIME: ");

    glColor3f(0.6, 0.6, 0.6);
    glRectf(-452, -328, -248, -392);

    glColor3f(0.4, 0.4, 0.4);
    glRectf(-450, -330, -250, -390);

    glColor3f(0.6, 0.6, 0.6);
    glRectf(452, -328, 248, -392);

    glColor3f(0.4, 0.4, 0.4);
    glRectf(450, -330, 250, -390);

    glColor3ub(240, 240, 240);
    vprint(280, -368, GLUT_BITMAP_TIMES_ROMAN_24, "Points:");

    if (beg == 0) {
        glColor3ub(240, 240, 240);
        vprint(0, -368, GLUT_BITMAP_TIMES_ROMAN_24, "20:00");

        glColor3ub(240, 240, 240);
        vprint(360, -368, GLUT_BITMAP_TIMES_ROMAN_24, "00");

    }
    else {
        glColor3ub(255, 255, 255);
        vprint2(0, -368, 0.15, "%2d:%02d", sec, salise);

        glColor3ub(255, 255, 255);
        vprint2(360, -368, 0.15, "%02d", totalHit);
    }

    

    if (!activeTimer && sec == 19 && salise == 99)
    {
        glColor3ub(240, 240, 240);
        vprint(-440, -345, GLUT_BITMAP_HELVETICA_12, "Press F1 to start the game.");
    }
    else if (!activeTimer) {
        glColor3ub(240, 240, 240);
        vprint(-440, -345, GLUT_BITMAP_HELVETICA_12, "Press F1 to resume game.");
    }
    else
    {
        glColor3ub(240, 240, 240);
        vprint(-440, -345, GLUT_BITMAP_HELVETICA_12, "Press F1 to stop the game.");
    }

    glColor3ub(240, 240, 240);
    vprint(-440, -365, GLUT_BITMAP_HELVETICA_12, "Press Space to fire.");

    glColor3ub(240, 240, 240);
    vprint(-440, -385, GLUT_BITMAP_HELVETICA_12, "Press h to hide me.");

}

void displayEnd() {

    glColor3f(0, 0, 0);
    glRectf(-102, -42, 102, 92);

    glColor3f(1, 1, 1);
    glRectf(-100, -40, 100, 90);

    glColor3f(0, 0, 0);
    glRectf(-98, -38, 98, 88);

    glColor3ub(240, 240, 240);
    vprint(-70, 20, GLUT_BITMAP_TIMES_ROMAN_24, "Total Point: %d", disphit);

}

int findAvailableFire() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (fr[i].active == false) return i;
    }
    return -1;
}

void display() {

    glClearColor(0,0,0, 0);
    glClear(GL_COLOR_BUFFER_BIT);


    brick();
    gameScreen();
    bubbles(target);
    ammo();
    devil(p);
    brickleft();
    brickright();

    if (!hide)
        info();

    if (end)
    {
        displayEnd();
    }



    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//

void resetTarget(target_t target[], int i) {
    float yPos;
 
        do
        {
            yPos = rand() % 250 + 60;

        } while (yPos < target[0].center.y + 10 && yPos > target[0].center.y - 10 || yPos < target[1].center.y + 10 && yPos > target[1].center.y - 10 || yPos < target[2].center.y + 10 && yPos > target[2].center.y - 10 || yPos < target[3].center.y + 10 && yPos > target[3].center.y - 10 || yPos < target[4].center.y + 45 && yPos > target[4].center.y - 45);
        target[i].center = { -800, yPos };
        target[i].radius = TARGET_RADIUS;
        target[i].speed = 4;
    

}

void resetTargett(target_t target[], int i, float temp) {
    float yPos;
  
        yPos = rand() % 250 + 60;
        target[i].center = { -800 - temp, yPos };
        target[i].radius = TARGET_RADIUS;
        target[i].speed = 4;
}


bool testCollision(fire_t fr, target_t t) {
    float dx = t.center.x - fr.pos.x;
    float dy = t.center.y - fr.pos.y;
    float d = sqrt(dx * dx + dy * dy);
    return d <= t.radius + 10;
}


void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    if (key == ' ')
        spacebar = true;

    if (key == 'h') {
        if (!hide)
            hide = true;
        else
        {
            hide = false;
        }
    }





    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ') {
        spacebar = false;

    }


    // to refresh the window it calls display() function
    glutPostRedisplay();
}


void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP:
        up = true;
        break;
    case GLUT_KEY_DOWN:
        down = true;
        break;
    case GLUT_KEY_LEFT:
        left = true;
        break;
    case GLUT_KEY_RIGHT:
        right = true;
        break;
    }

    if (key == GLUT_KEY_LEFT) {
        if (p.pos.x > -550)
            p.pos.x -= 20;
    }

    if (key == GLUT_KEY_RIGHT) {
        if (p.pos.x < 500)
            p.pos.x += 20;
    }

    if (key == GLUT_KEY_F1) {
        if (end)
        {
            end = false;
        }
        if (!activeTimer)
            activeTimer = true;
        else
            activeTimer = false;
    }
    // to refresh the window it calls display() function
    glutPostRedisplay();
}


void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP:
        up = false;
        break;
    case GLUT_KEY_DOWN:
        down = false;
        break;
    case GLUT_KEY_LEFT:
        left = false;
        break;
    case GLUT_KEY_RIGHT:
        right = false;
        break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}


void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}


void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}


void onMove(int x, int y) {
    // Write your codes here.
    mouse.x = x - winWidth / 2;
    mouse.y = winHeight / 2 - y;


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);

    if (activeTimer)
    {
        beg = 1;
        salise--;
        if (salise == 0)
        {
            sec--;
            salise = 99;
        }
        if (sec == -1) {

            sec = 19;

            activeTimer = !activeTimer;
            beg = 0;
            float temp = 0;
            for (int i = 0; i < 5; i++) {
                resetTargett(target, i, temp);
                temp += 300;
            }

            end = true;
        }



        for (int i = 0; i < 5; i++) {
            target[i].center.x += target[i].speed;
            if (target[i].center.x > 650) {
                resetTarget(target, i);
            }

        }

        if (spacebar && fire_rate == 0) {
            int availFire = findAvailableFire();
            if (availFire != -1) {
                fr[availFire].pos.y = p.pos.y + 140;
                fr[availFire].pos.x = p.pos.x + 105;
                fr[availFire].active = true;
                fire_rate = FIRE_RATE;
            }
        }

        if (fire_rate > 0) fire_rate--;

        // Move all fires that are active.
        for (int i = 0; i < MAX_FIRE; i++) {
            if (fr[i].active) {
                fr[i].pos.y += 10;

                if (fr[i].pos.y > 500) {
                    fr[i].active = false;
                    spacebar = false;
                }

                for (int c = 0; c < 5; c++) {
                    if (testCollision(fr[i], target[c])) {
                        resetTarget(target, c);
                        fr[i].active = false;
                        spacebar = false;
                        totalHit++;
                        disphit = totalHit;
                    }
                }

            }

        }
       
    }

    if (!activeTimer) {
        
        totalHit = 0;
    }




    glutPostRedisplay();

}
#endif

void Init() {
    float temp = 0;
    for (int i = 0; i < 5; i++) {
        resetTargett(target, i, temp);
        temp += 300;
    }


    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("Bubble Devil by Orhan Kemal Koç");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}