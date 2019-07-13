/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QOpenGLWidget>
#include <QVector3D>

#include <qmath.h>

#include "QtSpline.h"

namespace Fragmentarium
{
namespace GUI
{

static inline void qSetColor(GLfloat colorVec[], const QColor c)
{
    colorVec[0] = c.redF();
    colorVec[1] = c.greenF();
    colorVec[2] = c.blueF();
    colorVec[3] = c.alphaF();
}

void Geometry::loadArrays() const
{
    glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
}

void Geometry::appendVertex(const QVector3D &a)
{
    vertices.append(a);
}

Patch::Patch(Geometry *g) : start(0), count(0), geom(g)
{
    pointSize = 1;
}

/// n = number of control points p = the selected control point
void Patch::draw(int n, int p) const
{

    glDepthMask(GL_TRUE); // Write to depth buffer for control points
    glPointSize(pointSize);
    if (start == 0) { /// drawing control points
        const QVector3D *v = geom->vertices.constData();
        for (uint i = 0; i < count; i++) {
            int objIndex = i + (n * count);
            if (p == objIndex) {
                glColor4f(1.0, 1.0, 0.0, 1.0);
            } else {
                glColor4fv(color);
            }
            glBegin(GL_POINTS);
            glVertex3f(v[i].x(), v[i].y(), v[i].z());
            glEnd();
        }
        glDepthMask(GL_FALSE); // no Write to depth buffer for spline points

    } else { /// drawing curve points

        glColor4fv(color);

        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_LINE_STRIP, start, count);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

void Patch::addVertex(const QVector3D &a)
{
    geom->appendVertex(a);
    count++;
}

class Vectoid
{
public:
    // No special Vectoid destructor - the parts are fetched out of this member
    // variable, and destroyed by the new owner
    QList<Patch *> parts;
};

class VectControlPoints : public Vectoid
{
public:
    VectControlPoints(Geometry *g, int num_ctrlpoints, QVector3D *ctrlpoints);
};

VectControlPoints::VectControlPoints(Geometry *g, int num_ctrlpoints, QVector3D *ctrlpoints)
{
    auto *cp = new Patch(g);
    // control points
    for (int i = 0; i < num_ctrlpoints; ++i) {
        cp->addVertex(ctrlpoints[i]);
    }
    cp->pointSize = 6.0;

    cp->start = g->vertices.count() - num_ctrlpoints;

    parts << cp;
}

class VectSpline : public Vectoid
{
public:
    VectSpline(Geometry *g, int num_ctrlpoints, int num_segments);

    // modified from
    // http://www.iquilezles.org/www/articles/minispline/minispline.htm
    void spline(const QVector3D *cP, int num, double t, QVector3D *v)
    {
        static double coefs[4][4] = {{-1.0, 2.0, -1.0, 0.0},
            {3.0, -5.0, 0.0, 2.0},
            {-3.0, 4.0, 1.0, 0.0},
            {1.0, -1.0, 0.0, 0.0}
        };

        // find control point
        double ki = (1.0 / (double)num);
        int k = 0;
        while (ki * k < t) {
            k++;
        }
        // interpolant
        double kj = ki * k;
        double kk = kj - ki;
        double h = (t - kk) / (kj - kk);
        // add basis functions
        for (int i = 0; i < 4; i++) {
            int kn = k + i - 2;
            if (kn < 0) {
                kn = 0;
            } else if (kn > (num - 1)) {
                kn = num - 1;
            }
            double b = 0.5f * (((coefs[i][0] * h + coefs[i][1]) * h + coefs[i][2]) * h + coefs[i][3]);
            v->setX(v->x() + (b * cP[kn].x()));
            v->setY(v->y() + (b * cP[kn].y()));
            v->setZ(v->z() + (b * cP[kn].z()));
        }
    }
};

VectSpline::VectSpline(Geometry *g, int num_ctrlpoints, int num_segments)
{
    /// spline points
    auto *sp = new Patch(g);
    /// this bit of fudge lets the end points land on their respective
    /// controlpoints
    double enD =
        (1.0 / ((num_segments - 1.0) + ((num_segments - 1.0) * (1.0 / (num_ctrlpoints - 1.0)))));
    for (int i = 0; i < num_segments; i++) {
        QVector3D s(0.0, 0.0, 0.0);
        spline(g->vertices.constData(), num_ctrlpoints, enD * i, &s);
        sp->addVertex(s);
    }
    sp->pointSize = 1.0;
    sp->start = g->vertices.count() - num_segments;

    parts << sp;
}

// TODO add start frame end frame for control points
QtSpline::QtSpline(QOpenGLWidget *parent, int nctrl, int nsegs, QVector3D *cv)
    : prnt(parent), geom(new Geometry())
{
    buildGeometry(nctrl, nsegs, cv);
}

QtSpline::~QtSpline()
{
    qDeleteAll(parts);
    delete geom;
}

void QtSpline::buildGeometry(int nctrl, int nsegs, QVector3D *cv)
{
    VectControlPoints ctrl(geom, nctrl, cv);
    parts << ctrl.parts;
    num_c = nctrl;
    num_s = nsegs;
    VectSpline curve(geom, nctrl, nsegs);
    parts << curve.parts;
}

void QtSpline::drawControlPoints(int n) const
{
    //     prnt->makeCurrent();
    geom->loadArrays();
    parts[0]->draw(objectName().toInt(), n);
}

void QtSpline::setControlColor(QColor c) const
{
    qSetColor(parts[0]->color, c);
}

void QtSpline::drawSplinePoints() const
{
    //     prnt->makeCurrent();
    geom->loadArrays();
    parts[1]->draw();
}

void QtSpline::setSplineColor(QColor c) const
{
    qSetColor(parts[1]->color, c);
}

QVector3D QtSpline::getControlPoint(int n)
{
    return geom->vertices[n];
}

QVector3D QtSpline::getSplinePoint(int n)
{
    return geom->vertices[n + num_c - 1];
}

void QtSpline::setControlPoint(int n, QVector3D *p)
{

    /// new control point position
    geom->vertices[n].setX(p->x());
    geom->vertices[n].setY(p->y());
    geom->vertices[n].setZ(p->z());
    /// this bit of fudge lets the end points land on their respective
    /// controlpoints
    double enD = (1.0 / ((num_s - 1.0) + ((num_s - 1.0) * (1.0 / (parts[0]->count - 1.0)))));
    /// new spline curve
    for (int i = 0; i < num_s; i++) {
        QVector3D s(0.0, 0.0, 0.0);
        ((VectSpline *)parts[1])
        ->spline(geom->vertices.constData(), num_c, enD * i, &s);
        geom->vertices[i + num_c] = s;
    }
}

void QtSpline::recalc(int nc, int ns, QVector3D *cv)
{
    parts.clear();
    buildGeometry(nc, ns, cv);
}
} // namespace GUI
} // namespace Fragmentarium
