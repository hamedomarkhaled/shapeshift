#include "cube.h"
#include <assert.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
int Cube::m_stepsPerTurn = 200;

Cube::Cube() :
    m_animStep(0),
    m_animFinalFace(0),
    m_animFinalAngle(0),
    m_animAngle(0.0),
    m_angleDiff(0.0)
{
    Cubelet *seed[FACELETID_MAX][9] = {
        {
            &m_cubelet[0][2][2], &m_cubelet[1][2][2], &m_cubelet[2][2][2],
            &m_cubelet[0][1][2], &m_cubelet[1][1][2], &m_cubelet[2][1][2],
            &m_cubelet[0][0][2], &m_cubelet[1][0][2], &m_cubelet[2][0][2],
        }, {
            &m_cubelet[2][2][2], &m_cubelet[2][2][1], &m_cubelet[2][2][0],
            &m_cubelet[2][1][2], &m_cubelet[2][1][1], &m_cubelet[2][1][0],
            &m_cubelet[2][0][2], &m_cubelet[2][0][1], &m_cubelet[2][0][0],
        }, {
            &m_cubelet[2][2][0], &m_cubelet[1][2][0], &m_cubelet[0][2][0],
            &m_cubelet[2][1][0], &m_cubelet[1][1][0], &m_cubelet[0][1][0],
            &m_cubelet[2][0][0], &m_cubelet[1][0][0], &m_cubelet[0][0][0],
        }, {
            &m_cubelet[0][2][0], &m_cubelet[0][2][1], &m_cubelet[0][2][2],
            &m_cubelet[0][1][0], &m_cubelet[0][1][1], &m_cubelet[0][1][2],
            &m_cubelet[0][0][0], &m_cubelet[0][0][1], &m_cubelet[0][0][2],
        }, {
            &m_cubelet[0][0][2], &m_cubelet[1][0][2], &m_cubelet[2][0][2],
            &m_cubelet[0][0][1], &m_cubelet[1][0][1], &m_cubelet[2][0][1],
            &m_cubelet[0][0][0], &m_cubelet[1][0][0], &m_cubelet[2][0][0],
        }, {
            &m_cubelet[0][2][0], &m_cubelet[1][2][0], &m_cubelet[2][2][0],
            &m_cubelet[0][2][1], &m_cubelet[1][2][1], &m_cubelet[2][2][1],
            &m_cubelet[0][2][2], &m_cubelet[1][2][2], &m_cubelet[2][2][2],
        },
    }; 
    memcpy(m_face, seed, sizeof(m_face));
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                Cubelet *cubelet = &m_cubelet[i][j][k];
                cubelet->setFaceletVisible(FACELETID_FRONT, k == 2);
                cubelet->setFaceletVisible(FACELETID_RIGHT, i == 2);
                cubelet->setFaceletVisible(FACELETID_BACK, k == 0);
                cubelet->setFaceletVisible(FACELETID_LEFT, i == 0);
                cubelet->setFaceletVisible(FACELETID_BOTTOM, j == 0);
                cubelet->setFaceletVisible(FACELETID_TOP, j == 2);
            }
        }
    }
    memset(m_rotAxis, 0, sizeof(m_rotAxis));
    memset(m_rotCondition, 0, sizeof(m_rotCondition));
	double identity[] = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	memcpy(m_rotMat, identity, sizeof(m_rotMat));
}

Cube::~Cube()
{

}

void Cube::resetRotation()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                m_cubelet[i][j][k].resetRotation();
}

void Cube::beginRotation(int whichFace, int whichAngle)
{
    assert(whichFace >= 0 && whichFace < FACELETID_MAX);
    assert(whichAngle >= 0 && whichAngle < ANGLEID_MAX);
    static const double axismap[][3] = {
        { 0.0, 0.0, 1.0, },
        { 1.0, 0.0, 0.0, },
        { 0.0, 0.0, -1.0, },
        { -1.0, 0.0, 0.0, },
        { 0.0, -1.0, 0.0, },
        { 0.0, 1.0, 0.0, },
    };
    static const int condmap[][3] =  {
        { 0, 0, 1, },
        { 1, 0, 0, },
        { 0, 0, -1, },
        { -1, 0, 0, },
        { 0, -1, 0, },
        { 0, 1, 0, },
    };
    static const double diffmap[3] = { 360.0 / m_stepsPerTurn, 360.0 / m_stepsPerTurn, -360.0 / m_stepsPerTurn };
    static const int stepmap[3] = { m_stepsPerTurn / 4, m_stepsPerTurn / 2, m_stepsPerTurn / 4 };

    if (m_animStep != 0) return; // ignore if another animation in progress
    m_animFinalFace = whichFace;
    m_animFinalAngle = whichAngle;
    m_animStep = stepmap[whichAngle];
    m_animAngle = 0.0;
    m_angleDiff = diffmap[whichAngle];
    memcpy(m_rotAxis, axismap[whichFace], sizeof(m_rotAxis));
    memcpy(m_rotCondition, condmap[whichFace], sizeof(m_rotCondition));
}

void Cube::updateRotation()
{
    if (m_animStep > 0)
    {
        m_animStep--;
        m_animAngle += m_angleDiff;
        if (m_animStep == 0)
            rotate(m_animFinalFace, m_animFinalAngle);
        glutPostRedisplay();
    }
}

bool Cube::isRotating() const
{
    return m_animStep != 0;
}

void Cube::rotateCube(int whichAxis, double angle)
{
    assert(whichAxis >= 0 && whichAxis < ROTAXIS_MAX);
    double axes[3][3];

    glPushMatrix();
    glLoadMatrixd(m_rotMat);
    getAxes(axes);
    glRotated(angle, axes[whichAxis][0], axes[whichAxis][1], axes[whichAxis][2]);
    glGetDoublev(GL_MODELVIEW_MATRIX, m_rotMat);
    glPopMatrix();
}

void Cube::rotate(int whichFace, int whichAngle)
{
    assert(whichFace >= 0 && whichFace < FACELETID_MAX);
    assert(whichAngle >= 0 && whichAngle < ANGLEID_MAX);
    static const int mapped[9] = {
        2, 5, 8,
        1, 4, 7,
        0, 3, 6,
    };
    int temp[9][FACELETID_MAX];
    for (int k = 0; k <= whichAngle; k++)
    {
        for (int i = 0; i < 9; i++)
            m_face[whichFace][i]->rotate(whichFace, ANGLEID_90);
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < FACELETID_MAX; j++)
                temp[i][j] = m_face[whichFace][i]->getFaceletColorID(j);
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < FACELETID_MAX; j++)
            {
                int replace = mapped[i];
                m_face[whichFace][i]->setFaceletColorID(j, temp[replace][j]);
            }
        }
    }
}

void Cube::getAxes(double axes[][3])
{
    double modelView[16], projection[16];
    int viewport[4];
    double p[4][3];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    gluUnProject(viewport[0], viewport[1], 0.0,
            modelView, projection, viewport,
            &p[0][0], &p[0][1], &p[0][2]);
    gluUnProject(viewport[2] - viewport[0], viewport[1], 0.0,
            modelView, projection, viewport,
            &p[1][0], &p[1][1], &p[1][2]);
    gluUnProject(viewport[0], viewport[3] - viewport[1], 0.0,
            modelView, projection, viewport,
            &p[2][0], &p[2][1], &p[2][2]);
    for (int i = 0; i < 3; i++)
    {
        axes[ROTAXIS_X][i] = p[1][i] - p[0][i];
        axes[ROTAXIS_Y][i] = p[2][i] - p[0][i];
    }

    for (int i = 0; i < 2; i++)
    {
        double len = sqrt(axes[i][0] * axes[i][0] + axes[i][1] * axes[i][1] + axes[i][2] * axes[i][2]);
        axes[i][0] /= len;
        axes[i][1] /= len;
        axes[i][2] /= len;
    }
    axes[2][0] = axes[0][1] * axes[1][2] - axes[0][2] * axes[1][1];
    axes[2][1] = axes[0][2] * axes[1][0] - axes[0][0] * axes[1][2];
    axes[2][2] = axes[0][0] * axes[1][1] - axes[0][1] * axes[1][0];
}

void Cube::render() const
{
    glPushMatrix();
    glMultMatrixd(m_rotMat);
    for (int i = -1; i <= 1; i++)
    {
        glPushName(i);
        for (int j = -1; j <= 1; j++)
        {
            glPushName(j);
            for (int k = -1; k <= 1; k++)
            {
                glPushName(k);
                glPushMatrix();
                if (m_animStep != 0 &&
                        (m_rotCondition[0] == 0 || i == m_rotCondition[0]) &&
                        (m_rotCondition[1] == 0 || j == m_rotCondition[1]) &&
                        (m_rotCondition[2] == 0 || k == m_rotCondition[2]))
                {
                    glRotated(m_animAngle, m_rotAxis[0], m_rotAxis[1], m_rotAxis[2]);
                }
                glTranslated((double)i, (double)j, (double)k);
                m_cubelet[i + 1][j + 1][k + 1].render();
                glPopMatrix();
                glPopName();
            }
            glPopName();
        }
        glPopName();
    }
    glPopMatrix();
}
