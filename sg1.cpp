/*
  This program plots different 2D functions.
*/

#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <iostream>
#include "glut.h" 

double circle = atan(1) * 8;
double halfCircle = atan(1) * 4;
double tau = circle; // 2 * PI = TAU
double pi = halfCircle; // TAU / 2 = PI

//How often should the drawing algorithm sample the function.
double step = 0.05;


int defaultW = 1000, defaultH = 1000;

unsigned char prevKey;

/*
   Nicomedes' Conchoid
   $x = a + b \cdot cos(t), y = a \cdot tg(t) + b \cdot sin(t)$. or
   $x = a - b \cdot cos(t), y = a \cdot tg(t) - b \cdot sin(t)$. where
   $t \in (-\pi / 2, \pi / 2)$
*/
void Display1() {
    double xmax, ymax, xmin, ymin;
    /*
      Nicomedes' Conchoid is a family of functions. These two parameters,
      a and b, choose a specific 2D function from that family.
      It has two branches, so, for each y, we have two x values.
      Therefore, we're actually drawing two lines, not one.
     */
    double a = 1, b = 2;

    /*
      We shadow the global variable with a different step size,
      as values different from 0.05
      would yield a significantly different plot.
    */
    double step = 0.05;

    /*
      First, we compute the points of the function, so we can determine
      the maximal extend of the drawing.
     */
    xmax = a - b - 1;
    xmin = a + b + 1;
    ymax = ymin = 0;
    for (double t = -pi / 2 + step; t < pi / 2; t += step) {
        double x1, y1, x2, y2;
        x1 = a + b * cos(t);
        xmax = (xmax < x1) ? x1 : xmax;
        xmin = (xmin > x1) ? x1 : xmin;

        x2 = a - b * cos(t);
        xmax = (xmax < x2) ? x2 : xmax;
        xmin = (xmin > x2) ? x2 : xmin;

        y1 = a * tan(t) + b * sin(t);
        ymax = (ymax < y1) ? y1 : ymax;
        ymin = (ymin > y1) ? y1 : ymin;

        y2 = a * tan(t) - b * sin(t);
        ymax = (ymax < y2) ? y2 : ymax;
        ymin = (ymin > y2) ? y2 : ymin;
    }
    //We care about the maximal extent on each axis (from the origin).
    xmax = (fabs(xmax) > fabs(xmin)) ? fabs(xmax) : fabs(xmin);
    ymax = (fabs(ymax) > fabs(ymin)) ? fabs(ymax) : fabs(ymin);

    /*
      Since we have, for x and for y, the maximal absolute values,
      dividing the coordinates of the points by these values will
      ensure we draw the whole function inside [-1, 1]^2, the default
      OpenGL screen.
     */

    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINE_STRIP);
    for (double t = -pi / 2 + step; t < pi / 2; t += step) {
        double x1, y1, x2, y2; //You might get some warnings in your IDE. Why?
        x1 = (a + b * cos(t)) / xmax;
        x2 = (a - b * cos(t)) / xmax;
        y1 = (a * tan(t) + b * sin(t)) / ymax;
        y2 = (a * tan(t) - b * sin(t)) / ymax;

        glVertex2d(x1, y1);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (double t = -pi / 2 + step; t < pi / 2; t += step) {
        double x1, y1, x2, y2;
        x1 = (a + b * cos(t)) / xmax;
        x2 = (a - b * cos(t)) / xmax;
        y1 = (a * tan(t) + b * sin(t)) / ymax;
        y2 = (a * tan(t) - b * sin(t)) / ymax;

        glVertex2d(x2, y2);
    }
    glEnd();
}

// $f(x) = \left| sin(x) \right| \cdot e^{-sin(x)}, x \in \left[ 0, 8 \cdot \pi \right]$, 
void Display2() {
    /*
      We can determine how far the function extends
      (and thus the needed scaling factors)
      by looking at the function and doing a bit of Calculus.
    */
    double xmax = 8 * pi;
    double ymax = exp(1.1); //Why 1.1?

    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINE_STRIP);
    for (double x = 0; x < xmax; x += step) {
        double x1, y1;
        x1 = x / xmax;
        y1 = (fabs(sin(x)) * exp(-sin(x))) / ymax;
        glVertex2d(x1, y1);
    }
    glEnd();
}

/* 1)
   \( f(x) =
     \left\{
       \begin{array}{cl}
         1              & x = 0   \\
         \frac{d(x)}{x} & x \gt 0 \\
       \end{array}
     \right.
   \)
 */

double distance(double x) {
    if (x < int(x) + 0.5) {
        return x - int(x);
    }
    else {
        return int(x) + 1 - x;
    }
}
void Display3() {
    double xmax = 20; // xmax = 100
    double ymax = 1.1;
    double ratia = 0.5;

    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINE_STRIP);
    for (double x = 0; x < xmax; x += ratia) {
        double x1, y1;
        if (x == 0) {
            x1 = 0;
            y1 = 1 / ymax;
        }
        else {
            x1 = x / xmax;
            y1 = distance(x) / x / ymax;
        }
        glVertex2f(x1, y1);
    }
    glEnd();
}

//3) function arguments e.g.: f(a, b, t), where a and b are function family parameters, and the is the driving variables.
void plot(
    double (*x)(double, double, double),
    double (*y)(double, double, double),
    double a, double b,
    double intervalStart, double intervalEnd,
    double step = 0.01, double scaleX = 1, double scaleY = 1,
    GLint primitive = GL_LINE_STRIP)
{
    glBegin(primitive);
    for (double t = intervalStart; t <= intervalEnd; t += step) {
        double xVal = x(a, b, t) * scaleX;
        double yVal = y(a, b, t) * scaleY;

        glVertex2f(xVal, yVal);
    }
    glEnd();
    glFlush();
}

/*
  2) Circle Concoid (Limaçon, Pascal's Snail):
  \(x = 2 \cdot (a \cdot cos(t) + b) \cdot cos(t), \; y = 2 \cdot (a \cdot cos(t) + b) \cdot sin(t), \; t \in (-\pi, \pi)\) .
  For this plot, \(a = 0.3, \; b = 0.2\) .
*/
void Display4() {
    double pi = 4 * atan(1.0);
    double ratia = 0.05;
    double tmax = 3 * pi;
    double a = 0.3;
    double b = 0.2;


    glBegin(GL_LINE_STRIP);
    for (double t = -tmax; t < tmax; t += ratia) {
        double x1, y1;
        x1 = 2 * (a * cos(t) + b) * cos(t);
        y1 = 2 * (a * cos(t) + b) * sin(t);


        glColor3f(1, 0, 0);

        glVertex2f(x1, y1);
    }
    glEnd();
}



/*
  2) Cicloid:
  \(x = a \cdot t - b \cdot sin(t), \; y = a - b \cdot cos(t), \; t \in \mathbb{R} \) .
  For this plot, \(a = 0.1, \; b = 0.2\) .
*/
void Display5() {
    double pi = 4 * atan(1.0);
    double ratia = 0.05;
    double tmax = 3 * pi;
    double a = 0.1;
    double b = 0.2;

    glBegin(GL_LINE_STRIP);
    for (double t = -tmax; t < tmax; t += ratia) {
        double x1, y1;
        x1 = a * t - b * sin(t);
        y1 = a - b * cos(t);


        double factor = (t + tmax) / (2 * tmax);
        glColor3f(factor, 0, 0);

        glVertex2f(x1, y1);
    }
    glEnd();
}


/*
  2) Epicicloid:
  \(x = (a + b) \cdot cos\left( \frac{b}{a} \cdot t \right) - b \cdot cos\left(t + \frac{b}{a}\cdot t \right) \)
  \(y = (a + b) \cdot sin\left( \frac{b}{a} \cdot t \right) - b \cdot sin\left(t + \frac{b}{a}\cdot t \right) \)
  \( t \in \left[ 0, 2\pi \right] \) .
  For this plot, \(a = 0.1, \; b = 0.3\) .
*/
void Display6() {
    double pi = 4 * atan(1.0);
    double a = 0.1;
    double b = 0.3;
    double ratia = 0.01;

    glBegin(GL_LINE_STRIP);
    for (double t = 0; t <= 2 * pi; t += ratia) {
        double x1, y1;

        x1 = (a + b) * cos((b / a) * t) - b * cos(t + (b / a) * t);
        y1 = (a + b) * sin((b / a) * t) - b * sin(t + (b / a) * t);

        double factor = t / (2 * pi);
        glColor3f(factor, 0, 0);

        glVertex2f(x1, y1);
    }
    glEnd();
}


/*
  2) Hipocicloid:
  \(x = (a - b) \cdot cos\left( \frac{b}{a} \cdot t \right) - b \cdot cos\left(t - \frac{b}{a}\cdot t \right) \)
  \(y = (a - b) \cdot sin\left( \frac{b}{a} \cdot t \right) - b \cdot sin\left(t - \frac{b}{a}\cdot t \right) \)
  \( t \in \left[ 0, 2\pi \right] \) .
  For this plot, \(a = 0.1, \; b = 0.3\) .
 */
void Display7() {
    double pi = 4 * atan(1.0);
    double a = 0.1;
    double b = 0.3;
    double ratia = 0.1;

    glBegin(GL_LINE_STRIP);
    for (double t = 0; t <= 2 * pi; t += ratia) {
        double x1 = (a - b) * cos(t * (b / a)) - b * cos(t - (b / a) * t);
        double y1 = (a - b) * sin(b / a * t) - b * sin(t - (b / a) * t);


        double factor = t / (2 * pi);
        glColor3f(1 - factor, 0, 0);

        glVertex2f(x1, y1);
    }
    glEnd();
    glFlush();
}




/*
 4) Logarithmic spiral (in polar coordinates):
 \( r = a \cdot e^{1+t}, \; t \in (0, \infty) \) .
 For this plot, \(a = 0.02\) .
*/
void Display8() {
    double pi = 4 * atan(1.0);
    double a = 0.02;
    double tmax = 5 * pi;
    double ratia = 0.01;

    // Setăm matricea ModelView și o inițializăm cu identitatea
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Rotire cu 90° în jurul axei Z, anti-orar
    glRotatef(50.0f, 0.0f, 0.0f, 1.0f);

    glBegin(GL_LINE_STRIP);
    for (double t = 0; t <= tmax; t += ratia) {
        // Formula spiralei logaritmice
        double r = a * exp(1 + t);

        // Coordonatele inițiale (fără rotire)
        double x = r * cos(t);
        double y = r * sin(t);

        // Factor de culoare pe roșu, variază între 0 și 1
        float colorFactor = static_cast<float>(t / tmax);
        glColor3f(colorFactor, 0.0f, 0.0f);

        // Desenăm punctul (OpenGL deja îl va roti prin glRotatef)
        glVertex2f(static_cast<GLfloat>(x), static_cast<GLfloat>(y));
    }
    glEnd();

    glFlush();
}






/*
  4) Sine polar plot flower:
  \( r = sin(a \cdot t), \; t \in (0, \infty)  \) .
  For this plot, \(a = 10\), and the number 'petals' is \( 2 \cdot a \). Think about why.
*/
void Display9() {
    double pi = 4 * atan(1.0);
    double a = 10;
    double ratia = 0.01;
    double tmax = 2 * pi;

    glBegin(GL_LINE_STRIP);
    for (double t = 0; t <= tmax; t += ratia) {
        double r = sin(a * t);
        double x = r * cos(t);
        double y = r * sin(t);


        double factor = (t / tmax);
        glColor3f(factor, 0, 0);

        glVertex2f(x, y);
    }
    glEnd();
}


/*
5) Longchamps' Trisectrix:
\(
x = \frac{a}{4 \cdot cos^2(t) - 3}, \;
y = \frac{a \cdot tg(t)}{4 \cdot cos^2(t) - 3}, \;
t \in (-\pi/2, \pi/2) \setminus \{ -\pi/6, \pi/6 \} \) .
For this plot, \(a = 0.2\) .
 */
void Display10() {
    double pi = 4 * atan(1.0);
    double tmin = (-1) * pi / 2 + 0.07;
    double tmax = (-1) * pi / 6 - 0.06;
    double a = 0.2;
    double ratia = 0.001;
    double xmin = DBL_MAX;
    double ymax = DBL_MIN;

    // Determinăm xmin și ymax
    for (double t = tmin + ratia; t < tmax; t += ratia) {
        double x1 = a / (4 * cos(t) * cos(t) - 3);
        double y1 = a * tan(t) / (4 * cos(t) * cos(t) - 3);
        if (x1 < xmin) xmin = x1;
        if (y1 > ymax) ymax = y1;
    }

    // Desenăm triunghiurile mai late
    glPolygonMode(GL_FRONT, GL_FILL);

    for (double t = tmax; t > tmin + ratia * 10; t -= ratia * 20) { // Pas mai mare pentru triunghiuri mai late
        if (t == pi / 6 || t == -pi / 6)
            continue;

        double x1, y1, x2, y2;
        x1 = a / (4 * cos(t) * cos(t) - 3);
        y1 = a * tan(t) / (4 * cos(t) * cos(t) - 3);

        if (t - ratia * 10 <= tmin)
            continue;

        t -= ratia * 10; // Facem triunghiul mai lat
        x2 = a / (4 * cos(t) * cos(t) - 3);
        y2 = a * tan(t) / (4 * cos(t) * cos(t) - 3);

        // Gradient de culoare: roșu spre interior
        double factor = (t - tmin) / (tmax - tmin);
        glColor3f(1.0 - factor, 0.1, 0.1);

        glBegin(GL_TRIANGLES);
        glVertex2f(x1, y1);
        glVertex2f(xmin, ymax);
        glVertex2f(x2, y2);
        glEnd();
    }

    // Conturul
    glPolygonMode(GL_FRONT, GL_LINE);
    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(xmin, ymax);
    for (double t = tmin + ratia; t < tmax; t += ratia) {
        if (t == pi / 6 || t == -pi / 6)
            continue;

        double x1, y1;
        x1 = a / (4 * cos(t) * cos(t) - 3);
        y1 = a * tan(t) / (4 * cos(t) * cos(t) - 3);
        glVertex2f(x1, y1);
    }
    glVertex2f(xmin, ymax);
    glEnd();
}



void init(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glLineWidth(2);
    glPointSize(1);
    //glPolygonMode(GL_FRONT, GL_LINE);
    //Enabling blending and smoothing
    glEnable(GL_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_NICEST, GL_POINT_SMOOTH_HINT);
    glHint(GL_NICEST, GL_LINE_SMOOTH_HINT);
    glHint(GL_NICEST, GL_POLYGON_SMOOTH_HINT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
}

void Display(void) {
    std::cout << ("Call Display") << std::endl;
    // Clear the buffer. See init();
    glClear(GL_COLOR_BUFFER_BIT);

    switch (prevKey) {
    case '1':
        Display1();
        break;
    case '2':
        Display2();
        break;
    case '3':
        Display3();
        break;
    case '4':
        Display4();
        break;
    case '5':
        Display5();
        break;
    case '6':
        Display6();
        break;
    case '7':
        Display7();
        break;
    case '8':
        Display8();
        break;
    case '9':
        Display9();
        break;
    case '0':
        Display10();
        break;
    default:
        break;
    }
    glFlush();
}

void Reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}
void KeyboardFunc(unsigned char key, int x, int y) {
    prevKey = key;
    if (key == 27) // escape
        exit(0);
    //The proper way to ask glut to redraw the window.
    glutPostRedisplay();
}

/*
  Callback upon mouse press or release.
  The button can be:
  GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
  (and further for mousewheel and other mouse buttons)
  The state can be either GLUT_DOWN or  GLUT_UP, for
  a pressed or released button.
  (x, y) are the coordinates of the mouse.
*/
void MouseFunc(int button, int state, int x, int y) {
    std::cout << "Mouse button ";
    std::cout << ((button == GLUT_LEFT_BUTTON) ? "left" : ((button == GLUT_RIGHT_BUTTON) ? "right" : "middle")) << " ";
    std::cout << ((state == GLUT_DOWN) ? "pressed" : "released");
    std::cout << " at coordinates: " << x << " x " << y << std::endl;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(defaultW, defaultH);
    glutInitWindowPosition(-1, -1);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow(argv[0]);
    init();
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(KeyboardFunc);
    glutMouseFunc(MouseFunc);
    glutDisplayFunc(Display);
    //glutIdleFunc(Display);
    glutMainLoop();

    return 0;
}
