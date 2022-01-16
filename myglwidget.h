#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include "mystructs.h"

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <Camera.h>

#include <QTimer>

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    MyGLWidget(QWidget *parent);
    ~MyGLWidget();

    void updateFragmentShader(QOpenGLShader& fragment);
    void updateVertexShader(QOpenGLShader& vertex);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    //Drawing functions to be called within paintGL
    void drawPointCloud();
    void drawLine(QVector3D start, QVector3D end);
    void drawQuad(Frustum& frustum);

    void drawCameraFrustum(Frustum& frustum);


    //Mouse
    void wheelEvent(QWheelEvent * event);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    //Keyboard

    void keyPressEvent(QKeyEvent *event);

    QQuaternion ToQuaternion(QVector3D rot);

private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo_pointCloud;

    QOpenGLBuffer m_vbo_frustum;

    QOpenGLShaderProgram *m_program;
    QOpenGLShaderProgram *m_textureProgram;



    //Input
    bool m_mousePressed = false;
    QPoint m_mousePos;

    //Camera
    Camera *m_camera;
    QMatrix4x4 m_modelMatrix;

    int m_projectionMatrixUniform;
    int m_viewMatrixUniform;
    int m_modelMatrixUniform;
    int m_inputColorUniform;

    //For the frustums
    int m_texturedShaderProjUniform;
    int m_texturedShaderModelUniform;
    int m_texturedShaderViewUniform;

    //Point Cloud

    PointCloud *m_pointCloud;

    Frustum *m_frustumOne;
    Frustum *m_frustumTwo;
    Frustum *m_frustumThree;
    Frustum *m_frustumFour;

};

#endif // MYGLWIDGET_H
