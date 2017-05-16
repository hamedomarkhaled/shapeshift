#include "gamescene.h"
#include <GL/glut.h>

std::set<GameScene*> GameScene::s_scenes;

GameScene::GameScene() :
    m_mouseRotation(false)
{
    m_cube.rotateCube(ROTAXIS_Y, 45.0);
    m_cube.rotateCube(ROTAXIS_X, 30.0);
    memset(m_keyDown, 0, sizeof(m_keyDown));
    memset(m_specialDown, 0, sizeof(m_specialDown));
    s_scenes.insert(this);
    s_timer(reinterpret_cast<int>(this));
}

GameScene::~GameScene()
{
    s_scenes.erase(this);
}

void GameScene::render()
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(m_projection);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslated(0.0, 0.0, -7.0);
    m_cube.render();
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void GameScene::keyboard(unsigned char key, int x, int y)
{
    m_keyDown[key] = true;
    if (key == 'r')
        m_cube.beginRotation(FACELETID_FRONT, ANGLEID_270);
    else if (key == 'g')
        m_cube.beginRotation(FACELETID_RIGHT, ANGLEID_270);
    else if (key == 'o')
        m_cube.beginRotation(FACELETID_BACK, ANGLEID_270);
    else if (key == 'b')
        m_cube.beginRotation(FACELETID_LEFT, ANGLEID_270);
    else if (key == 'w')
        m_cube.beginRotation(FACELETID_BOTTOM, ANGLEID_270);
    else if (key == 'y')
        m_cube.beginRotation(FACELETID_TOP, ANGLEID_270);
}

void GameScene::keyboardUp(unsigned char key, int x, int y)
{
    m_keyDown[key] = false;
}

void GameScene::special(int key, int x, int y)
{
    m_specialDown[key] = true;
}

void GameScene::specialUp(int key, int x, int y)
{
    m_specialDown[key] = false;
}

void GameScene::reshape(int width, int height)
{
    int viewport[] = { 0, 0, width, height };
    int cur;

    memcpy(m_viewport, viewport, sizeof(m_viewport));
    glGetIntegerv(GL_MATRIX_MODE, &cur);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(70.0, 1.0 * width / height, 1.0, 50.0);
    glGetDoublev(GL_PROJECTION_MATRIX, m_projection);
    glPopMatrix();
    glMatrixMode(cur);
}

void GameScene::mouse(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        m_mouseRotation = true;
        m_mouseX = x;
        m_mouseY = y;
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        m_mouseRotation = false;
    }
}

void GameScene::motion(int x, int y)
{
    if (!m_mouseRotation) return;
    double diffx = x - m_mouseX;
    double diffy = y - m_mouseY;
    m_cube.rotateCube(ROTAXIS_Y, 0.5 * diffx);
    m_cube.rotateCube(ROTAXIS_X, 0.5 * diffy);
    m_mouseX = x; m_mouseY = y;
    glutPostRedisplay();
}

void GameScene::s_timer(int x)
{
    GameScene *that = reinterpret_cast<GameScene*>(x);
    if (s_scenes.find(that) != s_scenes.end())
        that->timer();
}

void GameScene::timer()
{
    if (m_specialDown[GLUT_KEY_LEFT])
    {
        m_cube.rotateCube(ROTAXIS_Y, -1.0);
        glutPostRedisplay();
    }
    else if (m_specialDown[GLUT_KEY_RIGHT])
    {
        m_cube.rotateCube(ROTAXIS_Y, 1.0);
        glutPostRedisplay();
    }
    if (m_specialDown[GLUT_KEY_UP])
    {
        m_cube.rotateCube(ROTAXIS_X, -1.0);
        glutPostRedisplay();
    }
    else if (m_specialDown[GLUT_KEY_DOWN])
    {
        m_cube.rotateCube(ROTAXIS_X, 1.0);
        glutPostRedisplay();
    }
    if (m_keyDown['/'])
    {
        m_cube.rotateCube(ROTAXIS_Z, -1.0);
        glutPostRedisplay();
    }
    else if (m_keyDown['.'])
    {
        m_cube.rotateCube(ROTAXIS_Z, 1.0);
        glutPostRedisplay();
    }
    m_cube.updateRotation();
    glutTimerFunc(10, s_timer, reinterpret_cast<int>(this));
}
