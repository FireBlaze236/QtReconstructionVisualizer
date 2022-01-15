#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

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
    void drawQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3);

    void drawCameraFrustum(QVector3D pos, QVector3D rotation);


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
    QOpenGLBuffer m_vbo;

    QOpenGLShaderProgram *m_program;

    QOpenGLShaderProgram *m_textureProgram;
    QOpenGLTexture *m_texture;


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


    //Timer
    QTimer timer;

};

#endif // MYGLWIDGET_H
