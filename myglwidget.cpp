#include "myglwidget.h"
#include <QOpenGLFunctions>
#include <QWheelEvent>
#include <QTimer>

#include <happly.h>

#include <vector>

#include<Filepaths.h>

MyGLWidget::MyGLWidget(QWidget *parent)
    :QOpenGLWidget(parent), m_program(0), m_textureProgram(0), m_pointCloud(0)
{
}

MyGLWidget::~MyGLWidget()
{
    initializeOpenGLFunctions();
    makeCurrent();

    delete m_program;
    delete m_textureProgram;
    delete m_camera;
    delete m_pointCloud;

    //delete frustums
    delete m_frustumOne;
    delete m_frustumTwo;


    m_vbo_pointCloud.destroy();
    m_vao.destroy();

    doneCurrent();
}

void MyGLWidget::initializeGL()
{

    m_vao.create();
    if(m_vao.isCreated())
    {
        m_vao.bind();
    }

    m_vbo_pointCloud.create();
    m_vbo_frustum.create();


    m_program = new QOpenGLShaderProgram(this);
    m_program->create();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, VERTEX_SHADER_BASIC);
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, FRAGMENT_SHADER_COLOR);

    m_program->link();
    m_program->bind();




    m_projectionMatrixUniform = m_program->uniformLocation("projectionMatrix");
    m_viewMatrixUniform = m_program->uniformLocation("viewMatrix");
    m_modelMatrixUniform = m_program->uniformLocation("modelMatrix");
    m_inputColorUniform = m_program->uniformLocation("inputColor");


    //Texture shader as well
    m_textureProgram = new QOpenGLShaderProgram(this);
    m_textureProgram->create();
    m_textureProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, VERTEX_SHADER_TEX);
    m_textureProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, FRAGMENT_SHADER_TEX);

    m_textureProgram->link();
    m_textureProgram->bind();

    m_texturedShaderModelUniform = m_textureProgram->uniformLocation("modelMatrix");
    m_texturedShaderViewUniform = m_textureProgram->uniformLocation("viewMatrix");
    m_texturedShaderProjUniform = m_textureProgram->uniformLocation("projectionMatrix");


    //Setup all matrices
    m_modelMatrix.setToIdentity();
    m_program->setUniformValue(m_modelMatrixUniform, m_modelMatrix);
    m_textureProgram->setUniformValue(m_texturedShaderModelUniform, m_modelMatrix);


    QVector3D pos(0, 0, -1);
    QVector3D target(0, 0, 1);
    QVector3D up(0, 1, 0);

    m_camera = new Camera(pos, up, target);
    m_camera->setPerspectiveProjection(45.0f, 1.33f, 0.1f, 10000.0f);

    m_program->setUniformValue(m_viewMatrixUniform, m_camera->getViewMatrix());
    m_program->setUniformValue(m_projectionMatrixUniform, m_camera->getPerspectiveProjection());


    m_textureProgram->setUniformValue(m_texturedShaderViewUniform, m_camera->getViewMatrix());
    m_textureProgram->setUniformValue(m_projectionMatrixUniform, m_camera->getPerspectiveProjection());


    m_pointCloud = new PointCloud(POINT_CLOUD_TEST);

    m_frustumOne = new Frustum(TEXTUREPATH_VIEW_1, m_modelMatrix,
                               QVector3D(-0.639768, -0.00425937, -0.0355112), QVector3D(-0.000964061,-0.136274,-0.00401575)) ;
    m_frustumTwo = new Frustum(TEXTUREPATH_VIEW_2, m_modelMatrix,
                               QVector3D(-0.267663,-0.00128758,-0.0993487), QVector3D(-0.000356492,-0.0603128,-0.002191)) ;

    m_frustumThree = new Frustum(TEXTUREPATH_VIEW_3, m_modelMatrix,
                               QVector3D(0.0114874,-0.00175259,0.0102903), QVector3D(-0.000550294,0.00500292,-0.000902102)) ;
    m_frustumFour = new Frustum(TEXTUREPATH_VIEW_4, m_modelMatrix,
                               QVector3D(-1.04132,-0.010242,0.00996433), QVector3D(-0.00237686,-0.15328,-0.00354582)) ;


    m_vbo_frustum.release();
    m_vbo_pointCloud.release();

    m_program->release();
    m_textureProgram->release();
}

void MyGLWidget::paintGL()
{
    drawPointCloud();

    drawCameraFrustum(*m_frustumOne);
    drawCameraFrustum(*m_frustumTwo);

    drawCameraFrustum(*m_frustumThree);
    drawCameraFrustum(*m_frustumFour);

    //qDebug()<< vertices.size();

}

void MyGLWidget::resizeGL(int w, int h)
{
    m_camera->setPerspectiveProjection(45.0, (float)w/ h, 0.1f, 10000.0f);

    m_program->bind();
    m_program->setUniformValue(m_projectionMatrixUniform, m_camera->getPerspectiveProjection());

    update();
}

void MyGLWidget::drawPointCloud()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    //f->glClearColor(0.8, 0.4, 0.4, 1);
    f->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_program->bind();
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate(180, QVector3D(1, 0, 0)); //flip along y,  format specific
    m_program->setUniformValue(m_modelMatrixUniform, m_modelMatrix);

    QVector3D green(0, 1, 0);
    m_program->setUniformValue(m_inputColorUniform, green);


    m_vao.bind();

    m_vbo_pointCloud.allocate(&m_pointCloud->vertices[0], sizeof(float)* m_pointCloud->vertices.size());
    m_vbo_pointCloud.bind();
    f->glBufferData(GL_ARRAY_BUFFER, m_pointCloud->vertices.size(), &m_pointCloud->vertices[0], GL_STATIC_DRAW);

    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
    f->glEnableVertexAttribArray(0);

    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3* sizeof(float)));
    f->glEnableVertexAttribArray(1);

    m_program->bind();
    glPointSize(1.0);

    f->glDrawArrays(GL_POINTS, 0, m_pointCloud->points);

    m_vbo_pointCloud.release();
    m_program->release();

}

void MyGLWidget::drawLine(QVector3D start, QVector3D end)
{
    m_vao.bind();

    m_program->bind();

    QVector3D white(0, 0, 1);

    m_program->setUniformValue(m_inputColorUniform, white);

    glBegin(GL_LINES);
    glVertex3f(start.x(), start.y(), start.z());
    glVertex3f(end.x(), end.y(), end.z());
    glEnd();

    m_program->release();
}

void MyGLWidget::drawQuad(Frustum& frustum)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    m_vao.bind();




    m_textureProgram->bind();
    m_textureProgram->setUniformValue(m_texturedShaderViewUniform, m_camera->getViewMatrix());
    m_textureProgram->setUniformValue(m_texturedShaderProjUniform, m_camera->getPerspectiveProjection());

    m_textureProgram->setUniformValue(m_texturedShaderModelUniform, m_modelMatrix);

    frustum.texture->bind();

    m_vbo_frustum.allocate(&frustum.quadVertex[0], frustum.quadVertex.size()*sizeof(float));
    m_vbo_frustum.bind();

    f->glBufferData(GL_ARRAY_BUFFER, frustum.quadVertex.size()*sizeof(float), &frustum.quadVertex[0], GL_STATIC_DRAW);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    f->glEnableVertexAttribArray(0);

    f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float)* 3));
    f->glEnableVertexAttribArray(1);

    f->glDrawArrays(GL_TRIANGLES,0, 6);


    f->glClear(GL_DEPTH_BUFFER_BIT);

    frustum.texture->release();

    m_textureProgram->release();
    m_vbo_frustum.release();
}

void MyGLWidget::drawCameraFrustum(Frustum& frustum)
{

    drawQuad(frustum);

    drawLine(frustum.v[0], frustum.v[1]);
    drawLine(frustum.v[0], frustum.v[2]);
    drawLine(frustum.v[3], frustum.v[1]);
    drawLine(frustum.v[3], frustum.v[2]);

    drawLine(frustum.v[4], frustum.v[5]);
    drawLine(frustum.v[4], frustum.v[6]);
    drawLine(frustum.v[7], frustum.v[5]);
    drawLine(frustum.v[7], frustum.v[6]);

    drawLine(frustum.v[0], frustum.v[4]);
    drawLine(frustum.v[1], frustum.v[5]);
    drawLine(frustum.v[3], frustum.v[7]);
    drawLine(frustum.v[2], frustum.v[6]);



}

void MyGLWidget::wheelEvent(QWheelEvent *event)
{


}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    this->setCursor(Qt::BlankCursor);
    m_mousePressed = true;
    m_mousePos = event->pos();
    setFocus();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_mousePressed)
    {
        QPoint now = event->pos() - m_mousePos;
        m_mousePos = event->pos();


        m_camera->setCameraLook(now.x(), now.y());

        m_program->bind();
        m_program->setUniformValue(m_viewMatrixUniform, m_camera->getViewMatrix());



        update();
    }
}



void MyGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(Qt::ArrowCursor);
    m_mousePressed = false;
}

void MyGLWidget::keyPressEvent(QKeyEvent *event)
{
    //qDebug() << event->key() << Qt::RightArrow;

    if (event->key() == Qt::Key_D)
    {
        m_camera->moveCamera(0.1f, CameraMovement::RIGHT);

    }
    else if (event->key() == Qt::Key_A)
    {
        m_camera->moveCamera(0.1f, CameraMovement::LEFT);
    }

    else if (event->key() == Qt::Key_W)
    {
        m_camera->moveCamera(0.1f, CameraMovement::FORWARD);
    }
    else if (event->key() == Qt::Key_S)
    {
        m_camera->moveCamera(0.1f, CameraMovement::BACKWARD);
    }



    m_program->bind();
    m_program->setUniformValue(m_viewMatrixUniform, m_camera->getViewMatrix());

    update();

}



