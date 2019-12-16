/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtOpenGL>
#include <fstream>
using namespace std;

#include "glwidget.h"

GLWidget::GLWidget(Robot *rob, QWidget *parent, QGLWidget *shareWidget)
    : QGLWidget(parent, shareWidget)
{

    robot = rob;
    clearColor = Qt::black;
    xRot = 0;
    yRot = 0;
    zRot = 0;

    x = 0;
    y = 0;

    nx = 10;
    ny = 7;

    scale = 1;

    offx = 0;
    offy = 0;

#ifdef QT_OPENGL_ES_2
    program = 0;
#endif
}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(200, 200);
}

void GLWidget::rotateBy(int xAngle, int yAngle, int zAngle)
{
    xRot += xAngle;
    yRot += yAngle;
    zRot += zAngle;
    updateGL();
}

void GLWidget::setClearColor(const QColor &color)
{
    clearColor = color;
    updateGL();
}

void GLWidget::initializeGL()
{
    makeObject();

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

}


void GLWidget::upd()
{
    paintGL();
}

const QColor linec   = Qt::yellow;
const QColor backc   = Qt::darkGreen;
const QColor textc   = Qt::black;
const QColor fillc   = Qt::darkGray;
const QColor rc      = Qt::yellow;
const QColor borderc = QColor(160, 82, 45); //brown
const QColor leftc   = Qt::cyan;
const QColor rightc  = Qt::yellow;
const QColor upc     = Qt::red;
const QColor downc   = Qt::blue;


void GLWidget::drawTorus()
{
    nx = robot->nx;
    ny = robot->ny;
    x = robot->x;
    y = robot->y;


    //Torus radii


    double r1 = 0.3;
    double r2 = 0.1;
    double P = 3.1415926;
    double PI = 2*P;

    /*
    ifstream f ("in.txt");
    f >> r1 >> r2;
    f.close();
    */

    glColor3d(backc.redF(), backc.greenF(), backc.blueF());

//Torus
    glBegin(GL_QUADS);
    for (int i = 0; i <= nx; i++)
        for (int j = 0; j <= ny; j++) {
            if (robot->field[(j%ny)*nx+(i%nx)].filled)
                glColor3d(fillc.redF(), fillc.greenF(), fillc.blueF());
            else
                glColor3d(backc.redF(), backc.greenF(), backc.blueF());
            glVertex3d((r1 + r2*cos(j*PI/ny))*cos(i*PI/nx),
                       (r1 + r2*cos(j*PI/ny))*sin(i*PI/nx),
                       r2*sin(j*PI/ny));
            glVertex3d((r1 + r2*cos(j*PI/ny))*cos((i + 1)*PI/nx),
                       (r1 + r2*cos(j*PI/ny))*sin((i + 1)*PI/nx),
                       r2*sin(j*PI/ny));
            glVertex3d((r1 + r2*cos((j + 1)*PI/ny))*cos((i + 1)*PI/nx),
                       (r1 + r2*cos((j + 1)*PI/ny))*sin((i + 1)*PI/nx),
                       r2*sin((j + 1)*PI/ny));
            glVertex3d((r1 + r2*cos((j + 1)*PI/ny))*cos(i*PI/nx),
                       (r1 + r2*cos((j + 1)*PI/ny))*sin(i*PI/nx),
                       r2*sin((j + 1)*PI/ny));
        }
    glEnd();

//Square borders
    glBegin(GL_LINES);
    glColor3d(linec.redF(), linec.greenF(), linec.blueF());
    for (int i = 0; i <= nx; i++)
        for (int j = 0; j <= ny; j++) {
            glVertex3d((r1 + r2*cos(j*PI/ny))*cos((i + 1)*PI/nx),
                       (r1 + r2*cos(j*PI/ny))*sin((i + 1)*PI/nx),
                       r2*sin(j*PI/ny));
            glVertex3d((r1 + r2*cos(j*PI/ny))*cos(i*PI/nx),
                       (r1 + r2*cos(j*PI/ny))*sin(i*PI/nx),
                       r2*sin(j*PI/ny));
            glVertex3d((r1 + r2*cos((j + 1)*PI/ny))*cos(i*PI/nx),
                       (r1 + r2*cos((j + 1)*PI/ny))*sin(i*PI/nx),
                       r2*sin((j + 1)*PI/ny));
            glVertex3d((r1 + r2*cos(j*PI/ny))*cos(i*PI/nx),
                       (r1 + r2*cos(j*PI/ny))*sin(i*PI/nx),
                       r2*sin(j*PI/ny));
        }
    glEnd();

// ROBOT

        double  rs1 = 0.8; // robot size miltipliers
        double  rs2 = 1 - rs1;

        double  rh1 = 0.5; // robot height
        double  rh2 = 1 - rh1;
        double  rh = 0.3;

        QVector3D x0y0( (r1 + 1.*r2*cos(y*PI/ny))*cos(x*PI/nx),
                        (r1 + 1.*r2*cos(y*PI/ny))*sin(x*PI/nx),
                        1.*r2*sin(y*PI/ny));
        QVector3D x0y1( (r1 + 1.*r2*cos((y + 1)*PI/ny))*cos(x*PI/nx),
                        (r1 + 1.*r2*cos((y + 1)*PI/ny))*sin(x*PI/nx),
                        1.*r2*sin((y + 1)*PI/ny));
        QVector3D x1y0( (r1 + 1.*r2*cos(y*PI/ny))*cos((x + 1)*PI/nx),
                        (r1 + 1.*r2*cos(y*PI/ny))*sin((x + 1)*PI/nx),
                        1.*r2*sin(y*PI/ny));
        QVector3D x1y1( (r1 + 1.*r2*cos((y + 1)*PI/ny))*cos((x + 1)*PI/nx),
                        (r1 + 1.*r2*cos((y + 1)*PI/ny))*sin((x + 1)*PI/nx),
                        1.*r2*sin((y + 1)*PI/ny));
        QVector3D xy ((r1 + 1.*r2*(1 + 1.*rh*PI/ny)*cos((y + 0.5)*PI/ny))*cos((x + 0.5)*PI/nx),
                      (r1 + 1.*r2*(1 + 1.*rh*PI/ny)*cos((y + 0.5)*PI/ny))*sin((x + 0.5)*PI/nx),
                      r2*(1 + 1.*rh*PI/ny)*sin((y + 0.5)*PI/ny));
        QVector3D p1 = rs1*x0y0 + rs2*x1y1;
        QVector3D p2 = rs1*x0y1 + rs2*x1y0;
        QVector3D p3 = rs2*x0y0 + rs1*x1y1;
        QVector3D p4 = rs2*x0y1 + rs1*x1y0;

        glBegin(GL_QUADS);
	glColor3d(leftc.redF(), leftc.greenF(), leftc.blueF());
        glVertex3d(p1.x(), p1.y(), p1.z());
        glVertex3d(p2.x(), p2.y(), p2.z());
        glVertex3d((rh2*xy + rh1*p2).x(), (rh2*xy + rh1*p2).y(), (rh2*xy + rh1*p2).z());
        glVertex3d((rh2*xy + rh1*p1).x(), (rh2*xy + rh1*p1).y(), (rh2*xy + rh1*p1).z());
	glColor3d(downc.redF(), downc.greenF(), downc.blueF());
        glVertex3d(p2.x(), p2.y(), p2.z());
        glVertex3d(p3.x(), p3.y(), p3.z());
        glVertex3d((rh2*xy + rh1*p3).x(), (rh2*xy + rh1*p3).y(), (rh2*xy + rh1*p3).z());
        glVertex3d((rh2*xy + rh1*p2).x(), (rh2*xy + rh1*p2).y(), (rh2*xy + rh1*p2).z());
	glColor3d(rightc.redF(), rightc.greenF(), rightc.blueF());
        glVertex3d(p3.x(), p3.y(), p3.z());
        glVertex3d(p4.x(), p4.y(), p4.z());
        glVertex3d((rh2*xy + rh1*p4).x(), (rh2*xy + rh1*p4).y(), (rh2*xy + rh1*p4).z());
        glVertex3d((rh2*xy + rh1*p3).x(), (rh2*xy + rh1*p3).y(), (rh2*xy + rh1*p3).z());
	glColor3d(upc.redF(), upc.greenF(), upc.blueF());
        glVertex3d(p4.x(), p4.y(), p4.z());
        glVertex3d(p1.x(), p1.y(), p1.z());
        glVertex3d((rh2*xy + rh1*p1).x(), (rh2*xy + rh1*p1).y(), (rh2*xy + rh1*p1).z());
        glVertex3d((rh2*xy + rh1*p4).x(), (rh2*xy + rh1*p4).y(), (rh2*xy + rh1*p4).z());

        QColor roofc = Qt::lightGray;
        glColor3d(roofc.redF(), roofc.greenF(), roofc.blueF());
        glVertex3d((rh2*xy + rh1*p1).x(), (rh2*xy + rh1*p1).y(), (rh2*xy + rh1*p1).z());
        glVertex3d((rh2*xy + rh1*p2).x(), (rh2*xy + rh1*p2).y(), (rh2*xy + rh1*p2).z());
        glVertex3d((rh2*xy + rh1*p3).x(), (rh2*xy + rh1*p3).y(), (rh2*xy + rh1*p3).z());
        glVertex3d((rh2*xy + rh1*p4).x(), (rh2*xy + rh1*p4).y(), (rh2*xy + rh1*p4).z());

        glEnd();

//Tricolor
        glBegin(GL_LINES);
        //central offset
        QVector3D coff(r2*cos((y + 0.5)*PI/ny)*cos((x + 0.5)*PI/nx),
                       r2*cos((y + 0.5)*PI/ny)*sin((x + 0.5)*PI/nx),
                       r2*sin((y + 0.5)*PI/ny));

        //horizontal offset
        QVector3D hoff = QVector3D( -r2*cos((y + 0.5)*PI/ny)*sin((x + 0.5)*PI/nx),
                                    r2*cos((y + 0.5)*PI/ny)*cos((x + 0.5)*PI/nx),
                                    0).normalized();


        //flag pos multiplyers
        double rd1 = 0.8;
        double rd2 = 1 - rd1;
        double rr = 0.5; // flag height

        QVector3D l2 = rd1*(rh2*xy + rh1*p1) + rd2*(rh2*xy + rh1*p3);
        QVector3D l1 = l2 + coff;

        glVertex3d (l1.x(), l1.y(), l1.z());
        glVertex3d (l2.x(), l2.y(), l2.z());

        glEnd();

        glBegin(GL_QUADS);

        double d1 = 0.9;
        double d2 = 0.8;
        double d3 = 0.7;

        double sd = 0.5*(p1 - p3).length();

        hoff *= sd;

        glColor3d(1, 1, 1); // white
        glVertex3d (l1.x(), l1.y(), l1.z());
        glVertex3d ((l1 + hoff).x(), (l1 + hoff).y(), (l1 + hoff).z());
        glVertex3d ((l2 + d1*coff + hoff).x(), (l2 + d1*coff + hoff).y(), (l2 + d1*coff + hoff).z());
        glVertex3d ((l2 + d1*coff).x(), (l2 + d1*coff).y(), (l2 + d1*coff).z());

        glColor3d(0, 0, 1); // blue
        glVertex3d (l1.x(), l1.y(), l1.z());
        glVertex3d ((l1 + hoff).x(), (l1 + hoff).y(), (l1 + hoff).z());
        glVertex3d ((l2 + d2*coff + hoff).x(), (l2 + d2*coff + hoff).y(), (l2 + d2*coff + hoff).z());
        glVertex3d ((l2 + d2*coff).x(), (l2 + d2*coff).y(), (l2 + d2*coff).z());

        glColor3d(1, 0, 0); //red
        glVertex3d (l1.x(), l1.y(), l1.z());
        glVertex3d ((l1 + hoff).x(), (l1 + hoff).y(), (l1 + hoff).z());
        glVertex3d ((l2 + d3*coff + hoff).x(), (l2 + d3*coff + hoff).y(), (l2 + d3*coff + hoff).z());
        glVertex3d ((l2 + d3*coff).x(), (l2 + d3*coff).y(), (l2 + d3*coff).z());

// Walls
    glBegin(GL_QUADS);

    const double d = 1.15; //wall height
    for (int i = 0; i <= nx; i++)
        for (int j = 0; j <= ny; j++) {
            if (robot->field[(j%ny)*nx+(i%nx)].wallUp) {
                glColor3d(borderc.redF(), borderc.greenF(), borderc.blueF());
                glVertex3d((r1 + r2*cos(j*PI/ny))*cos(i*PI/nx),
                            (r1 + r2*cos(j*PI/ny))*sin(i*PI/nx),
                            r2*sin(j*PI/ny));
                glVertex3d((r1 + d*r2*cos(j*PI/ny))*cos(i*PI/nx),
                            (r1 + d*r2*cos(j*PI/ny))*sin(i*PI/nx),
                            d*r2*sin(j*PI/ny));
                glVertex3d((r1 + d*r2*cos(j*PI/ny))*cos((i + 1)*PI/nx),
                            (r1 + d*r2*cos(j*PI/ny))*sin((i + 1)*PI/nx),
                            d*r2*sin(j*PI/ny));
                glVertex3d((r1 + r2*cos(j*PI/ny))*cos((i + 1)*PI/nx),
                            (r1 + r2*cos(j*PI/ny))*sin((i + 1)*PI/nx),
                            r2*sin(j*PI/ny));
            }

            if (robot->field[(j%ny)*nx+(i%nx)].wallLeft) {
                glColor3d(borderc.redF(), borderc.greenF(), borderc.blueF());
                glVertex3d((r1 + r2*cos(j*PI/ny))*cos(i*PI/nx),
                            (r1 + r2*cos(j*PI/ny))*sin(i*PI/nx),
                            r2*sin(j*PI/ny));
                glVertex3d((r1 + d*r2*cos(j*PI/ny))*cos(i*PI/nx),
                            (r1 + d*r2*cos(j*PI/ny))*sin(i*PI/nx),
                            d*r2*sin(j*PI/ny));
                glVertex3d((r1 + d*r2*cos((j + 1)*PI/ny))*cos(i*PI/nx),
                            (r1 + d*r2*cos((j + 1)*PI/ny))*sin(i*PI/nx),
                            d*r2*sin((j + 1)*PI/ny));
                glVertex3d((r1 + r2*cos((j + 1)*PI/ny))*cos(i*PI/nx),
                            (r1 + r2*cos((j + 1)*PI/ny))*sin(i*PI/nx),
                            r2*sin((j + 1)*PI/ny));
            }
        }
    glEnd();
}

void GLWidget::drawCyllinder()
{
    nx = robot->nx;
    ny = robot->ny;
    x = robot->x;
    y = robot->y;

    double r1 = 0.3;
    double r2 = 0.05;
    double P = 3.1415926;
    double PI = 2*P;
    double h = r2*sin(PI/ny);

    glColor3d(backc.redF(), backc.greenF(), backc.blueF());

//Cyllinder
    glBegin(GL_QUADS);
    for (int j = 0; j < ny; j++) {
	for (int i = 0; i < nx; i++) {
	    if (robot->field[(j%ny)*nx+(i%nx)].filled)
		glColor3d(fillc.redF(), fillc.greenF(), fillc.blueF());
	    else
		glColor3d(backc.redF(), backc.greenF(), backc.blueF());
	    glVertex3d(r2*cos(i*PI/nx),
		       r2*sin(i*PI/nx),
		       j*h);
	    glVertex3d(r2*cos((i + 1)*PI/nx),
		       r2*sin((i + 1)*PI/nx),
		       j*h);
	    glVertex3d(r2*cos((i + 1)*PI/nx),
		       r2*sin((i + 1)*PI/nx),
		       (j + 1)*h);
	    glVertex3d(r2*cos(i*PI/nx),
		       r2*sin(i*PI/nx),
		       (j + 1)*h);
	}
    }
    glEnd();

//Square borders
    glBegin(GL_LINES);
    glColor3d(linec.redF(), linec.greenF(), linec.blueF());
    for (int i = 0; i < nx; i++)
	for (int j = 0; j < ny; j++) {
	glVertex3d(r2*cos(i*PI/nx),
		   r2*sin(i*PI/nx),
		   j*h);
	glVertex3d(r2*cos((i + 1)*PI/nx),
		   r2*sin((i + 1)*PI/nx),
		   j*h);
	glVertex3d(r2*cos(i*PI/nx),
		   r2*sin(i*PI/nx),
		   j*h);
	glVertex3d(r2*cos(i*PI/nx),
		   r2*sin(i*PI/nx),
		   (j + 1)*h);
    }
    glEnd();

    // ROBOT

            double  rs1 = 0.8; // robot size miltipliers
            double  rs2 = 1 - rs1;

            double  rh1 = 0.5; // robot height
            double  rh2 = 1 - rh1;
            double  rh = 0.3;
            r1 = 0;

            QVector3D x0y0( r2*cos(x*PI/nx),
                            r2*sin(x*PI/nx),
                            y*h);
            QVector3D x0y1( r2*cos(x*PI/nx),
                            r2*sin(x*PI/nx),
                            (y + 1)*h);
            QVector3D x1y0( r2*cos((x + 1)*PI/nx),
                            r2*sin((x + 1)*PI/nx),
                            y*h);
            QVector3D x1y1( r2*cos((x + 1)*PI/nx),
                            r2*sin((x + 1)*PI/nx),
                            (y + 1)*h);

            QVector3D xy ( (r2 + h/3)*cos((x + 0.5)*PI/nx),
                           (r2 + h/3)*sin((x + 0.5)*PI/nx),
                           (y + 0.5)*h);

            QVector3D p1 = rs1*x0y0 + rs2*x1y1;
            QVector3D p2 = rs1*x0y1 + rs2*x1y0;
            QVector3D p3 = rs2*x0y0 + rs1*x1y1;
            QVector3D p4 = rs2*x0y1 + rs1*x1y0;

            glBegin(GL_QUADS);
            glColor3d(leftc.redF(), leftc.greenF(), leftc.blueF());
            glVertex3d(p1.x(), p1.y(), p1.z());
            glVertex3d(p2.x(), p2.y(), p2.z());
            glVertex3d((rh2*xy + rh1*p2).x(), (rh2*xy + rh1*p2).y(), (rh2*xy + rh1*p2).z());
            glVertex3d((rh2*xy + rh1*p1).x(), (rh2*xy + rh1*p1).y(), (rh2*xy + rh1*p1).z());
            glColor3d(downc.redF(), downc.greenF(), downc.blueF());
            glVertex3d(p2.x(), p2.y(), p2.z());
            glVertex3d(p3.x(), p3.y(), p3.z());
            glVertex3d((rh2*xy + rh1*p3).x(), (rh2*xy + rh1*p3).y(), (rh2*xy + rh1*p3).z());
            glVertex3d((rh2*xy + rh1*p2).x(), (rh2*xy + rh1*p2).y(), (rh2*xy + rh1*p2).z());
            glColor3d(rightc.redF(), rightc.greenF(), rightc.blueF());
            glVertex3d(p3.x(), p3.y(), p3.z());
            glVertex3d(p4.x(), p4.y(), p4.z());
            glVertex3d((rh2*xy + rh1*p4).x(), (rh2*xy + rh1*p4).y(), (rh2*xy + rh1*p4).z());
            glVertex3d((rh2*xy + rh1*p3).x(), (rh2*xy + rh1*p3).y(), (rh2*xy + rh1*p3).z());
            glColor3d(upc.redF(), upc.greenF(), upc.blueF());
            glVertex3d(p4.x(), p4.y(), p4.z());
            glVertex3d(p1.x(), p1.y(), p1.z());
            glVertex3d((rh2*xy + rh1*p1).x(), (rh2*xy + rh1*p1).y(), (rh2*xy + rh1*p1).z());
            glVertex3d((rh2*xy + rh1*p4).x(), (rh2*xy + rh1*p4).y(), (rh2*xy + rh1*p4).z());

            QColor roofc = Qt::lightGray;
            glColor3d(roofc.redF(), roofc.greenF(), roofc.blueF());
            glVertex3d((rh2*xy + rh1*p1).x(), (rh2*xy + rh1*p1).y(), (rh2*xy + rh1*p1).z());
            glVertex3d((rh2*xy + rh1*p2).x(), (rh2*xy + rh1*p2).y(), (rh2*xy + rh1*p2).z());
            glVertex3d((rh2*xy + rh1*p3).x(), (rh2*xy + rh1*p3).y(), (rh2*xy + rh1*p3).z());
            glVertex3d((rh2*xy + rh1*p4).x(), (rh2*xy + rh1*p4).y(), (rh2*xy + rh1*p4).z());

            glEnd();

    //Tricolor
            glBegin(GL_LINES);
            //central offset
            QVector3D coff(r2*cos((x + 0.5)*PI/nx),
                           r2*sin((x + 0.5)*PI/nx),
                           0);

            //horizontal offset
            QVector3D hoff = QVector3D(0,
                                        -r2*sin((x + 0.5)*PI/nx),
                                        r2*cos((x + 0.5)*PI/nx)).normalized();


            //flag pos multiplyers
            double rd1 = 0.8;
            double rd2 = 1 - rd1;
            double rr = 0.5; // flag height

            QVector3D l2 = rd1*(rh2*xy + rh1*p1) + rd2*(rh2*xy + rh1*p3);
            QVector3D l1 = l2 + coff;

            glVertex3d (l1.x(), l1.y(), l1.z());
            glVertex3d (l2.x(), l2.y(), l2.z());

            glEnd();

            glBegin(GL_QUADS);

            double d1 = 0.9;
            double d2 = 0.8;
            double d3 = 0.7;

            double sd = 0.5*(p1 - p3).length();

            hoff *= sd;

            glColor3d(1, 1, 1); // white
            glVertex3d (l1.x(), l1.y(), l1.z());
            glVertex3d ((l1 + hoff).x(), (l1 + hoff).y(), (l1 + hoff).z());
            glVertex3d ((l2 + d1*coff + hoff).x(), (l2 + d1*coff + hoff).y(), (l2 + d1*coff + hoff).z());
            glVertex3d ((l2 + d1*coff).x(), (l2 + d1*coff).y(), (l2 + d1*coff).z());

            glColor3d(0, 0, 1); // blue
            glVertex3d (l1.x(), l1.y(), l1.z());
            glVertex3d ((l1 + hoff).x(), (l1 + hoff).y(), (l1 + hoff).z());
            glVertex3d ((l2 + d2*coff + hoff).x(), (l2 + d2*coff + hoff).y(), (l2 + d2*coff + hoff).z());
            glVertex3d ((l2 + d2*coff).x(), (l2 + d2*coff).y(), (l2 + d2*coff).z());

            glColor3d(1, 0, 0); //red
            glVertex3d (l1.x(), l1.y(), l1.z());
            glVertex3d ((l1 + hoff).x(), (l1 + hoff).y(), (l1 + hoff).z());
            glVertex3d ((l2 + d3*coff + hoff).x(), (l2 + d3*coff + hoff).y(), (l2 + d3*coff + hoff).z());
            glVertex3d ((l2 + d3*coff).x(), (l2 + d3*coff).y(), (l2 + d3*coff).z());



// Walls
    glBegin(GL_QUADS);

    const double d = 1.15; //wall height
    for (int i = 0; i <= nx; i++)
        for (int j = 0; j <= ny; j++) {
	if (robot->field[(j%ny)*nx+(i%nx)].wallUp) {
	    glColor3d(borderc.redF(), borderc.greenF(), borderc.blueF());
	    glVertex3d(r2*cos(i*PI/nx),
		       r2*sin(i*PI/nx),
		       j*h);
	    glVertex3d(d*r2*cos(i*PI/nx),
		       d*r2*sin(i*PI/nx),
		       j*h);
	    glVertex3d(d*r2*cos((i + 1)*PI/nx),
		       d*r2*sin((i + 1)*PI/nx),
		       j*h);
	    glVertex3d(r2*cos((i + 1)*PI/nx),
		       r2*sin((i + 1)*PI/nx),
		       j*h);
	}

	if (robot->field[(j%ny)*nx+(i%nx)].wallLeft) {
	    glColor3d(borderc.redF(), borderc.greenF(), borderc.blueF());
	    glVertex3d(r2*cos(i*PI/nx),
		       r2*sin(i*PI/nx),
		       j*h);
	    glVertex3d(d*r2*cos(i*PI/nx),
		       d*r2*sin(i*PI/nx),
		       j*h);
	    glVertex3d(d*r2*cos(i*PI/nx),
		       d*r2*sin(i*PI/nx),
		       (j + 1)*h);
	    glVertex3d(r2*cos(i*PI/nx),
		       r2*sin(i*PI/nx),
		       (j + 1)*h);
	}
    }
    glEnd();

}

void GLWidget::paintGL()
{
    qglClearColor(clearColor);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(float(offx), float(offy), -10.0f);
    glRotatef(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(zRot / 16.0f, 0.0f, 0.0f, 1.0f);
    glScaled(scale, scale, scale);

    glLineWidth(3);
    switch (robot->type)    {
        case Robot::torus	: drawTorus(); break;
        case Robot::cyllinder   : drawCyllinder(); break;
        default : break;
    }


}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

#if !defined(QT_OPENGL_ES_2)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifndef QT_OPENGL_ES
    glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
#else
    glOrthof(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
#endif

}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        rotateBy(8 * dy, 8 * dx, 0);
    } else if (event->buttons() & Qt::RightButton) {
        rotateBy(8 * dy, 0, 8 * dx);
    }
    lastPos = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0)
	if (scale > 0.1) scale *= 0.66;
    if (event->delta() < 0)
	if (scale < 5) scale *= 1.5;
    updateGL();
}


void GLWidget::keyPress(QKeyEvent *event)
{

    if (event->key() == Qt::Key_Minus)
        if (scale > 0.1) scale *= 0.66;  
    if (event->key() == Qt::Key_Plus)
        if (scale < 5) scale *= 1.5;

    double d = 0.1;
    if (event->key() == Qt::Key_Left)
        offx -= d;
    if (event->key() == Qt::Key_Right)
        offx += d;
    if (event->key() == Qt::Key_Up)
        offy -= d;
    if (event->key() == Qt::Key_Down)
        offy += d;

    updateGL();
}

void GLWidget::makeObject()
{


}
