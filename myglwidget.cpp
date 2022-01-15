#include "myglwidget.h"
#include <QOpenGLFunctions>
#include <QWheelEvent>
#include <QTimer>

#include <happly.h>

#include <vector>

MyGLWidget::MyGLWidget(QWidget *parent)
    :QOpenGLWidget(parent), m_program(0), m_textureProgram(0)
{
}

MyGLWidget::~MyGLWidget()
{
    initializeOpenGLFunctions();
    makeCurrent();

    delete m_program;
    delete m_textureProgram;
    delete m_camera;

    m_vbo.destroy();
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

    m_vbo.create();


    m_program = new QOpenGLShaderProgram(this);
    m_program->create();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "C:/Shaders/vertex.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "C:/Shaders/fragment.glsl");

    m_program->link();
    m_program->bind();

    //Texture shader as well
    m_textureProgram = new QOpenGLShaderProgram(this);


    m_projectionMatrixUniform = m_program->uniformLocation("projectionMatrix");
    m_viewMatrixUniform = m_program->uniformLocation("viewMatrix");
    m_modelMatrixUniform = m_program->uniformLocation("modelMatrix");
    m_inputColorUniform = m_program->uniformLocation("inputColor");


    m_modelMatrix.setToIdentity();
    m_program->setUniformValue(m_modelMatrixUniform, m_modelMatrix);

    QVector3D pos(0, 0, -1);
    QVector3D target(0, 0, 1);
    QVector3D up(0, 1, 0);

    m_camera = new Camera(pos, up, target);
    m_camera->setPerspectiveProjection(45.0f, 1.33f, 0.1f, 10000.0f);

    m_program->setUniformValue(m_viewMatrixUniform, m_camera->getViewMatrix());
    m_program->setUniformValue(m_projectionMatrixUniform, m_camera->getPerspectiveProjection());
}

void MyGLWidget::paintGL()
{
    drawPointCloud();

    drawCameraFrustum(QVector3D(-0.639768, -0.00425937, -0.0355112),
                      QVector3D(-0.000964061,-0.136274,-0.00401575));

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


    happly::PLYData plyIn("C:/Shaders/PointCloud.ply");

    std::vector<float> vertex_x = plyIn.getElement("vertex").getProperty<float>("x");
    std::vector<float> vertex_y = plyIn.getElement("vertex").getProperty<float>("y");
    std::vector<float> vertex_z = plyIn.getElement("vertex").getProperty<float>("z");

    std::vector<uchar> color_r = plyIn.getElement("vertex").getProperty<uchar>("red");
    std::vector<uchar> color_g = plyIn.getElement("vertex").getProperty<uchar>("green");
    std::vector<uchar> color_b = plyIn.getElement("vertex").getProperty<uchar>("blue");

    std::vector<float> vertices;

    for(int i = 0; i < vertex_x.size();i++)
    {
        vertices.push_back(vertex_x[i]);
        vertices.push_back(vertex_y[i]);
        vertices.push_back(vertex_z[i]);

        vertices.push_back(color_r[i] / 255.0f);
        vertices.push_back(color_g[i] / 255.0f);
        vertices.push_back(color_b[i] / 255.0f);
    }


    m_program->bind();
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate(180, QVector3D(1, 0, 0)); //flip along y,  format specific
    m_program->setUniformValue(m_modelMatrixUniform, m_modelMatrix);

    QVector3D green(0, 1, 0);
    m_program->setUniformValue(m_inputColorUniform, green);


    m_vao.bind();

    m_vbo.allocate(&vertices[0], sizeof(float)* vertices.size());
    m_vbo.bind();
    f->glBufferData(GL_ARRAY_BUFFER, vertices.size(), &vertices[0], GL_STATIC_DRAW);

    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
    f->glEnableVertexAttribArray(0);

    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3* sizeof(float)));
    f->glEnableVertexAttribArray(1);

    m_program->bind();
    glPointSize(1.0);

    f->glDrawArrays(GL_POINTS, 0, vertex_x.size());

}

void MyGLWidget::drawLine(QVector3D start, QVector3D end)
{

    m_vao.bind();

    QVector3D white(1, 1, 1);
    m_program->setUniformValue(m_inputColorUniform, white);

    glBegin(GL_LINES);

    glVertex3f(start.x(), start.y(), start.z());
    glVertex3f(end.x(), end.y(), end.z());
    glEnd();
}

void MyGLWidget::drawQuad(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    m_vao.bind();

    float vertices[] = {
        p0.x(), p0.y(), p0.z(),
        p1.x(), p1.y(), p1.z(),
        p2.x(), p2.y(), p2.z(),

        p2.x(), p2.y(), p2.z(),
        p3.x(), p3.y(), p3.z(),
        p0.x(), p0.y(), p0.z()
    };

    QVector3D red(0.5, 0, 0);
    m_program->bind();
    m_program->setUniformValue(m_inputColorUniform, red);

    m_program->setUniformValue(m_viewMatrixUniform, m_camera->getViewMatrix());
    m_program->setUniformValue(m_projectionMatrixUniform, m_camera->getPerspectiveProjection());


    m_vbo.allocate(&vertices[0], sizeof(vertices));
    m_vbo.bind();
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    f->glEnableVertexAttribArray(0);

    f->glDrawArrays(GL_TRIANGLES,0, 6);
}

void MyGLWidget::drawCameraFrustum(QVector3D pos, QVector3D rotation)
{
    QMatrix4x4 inv; //The inverse of a view matrix of camera, in our case this is already producable
    inv.setToIdentity();
    inv.translate(pos);
    inv.rotate(ToQuaternion(rotation));


    float ar = 2032.0f/ 1520.f; //aspect
    float fov = 0.82f; // calculated from focal length assumed to be mm
    float n = 0.1f; // random
    float f = 1.0f; // to make it small
    float halfHeight = tanf(fov / 2.0f);
    float halfWidth = halfHeight * ar;

    float xn = halfWidth * n;
    float xf = halfWidth * f;
    float yn = halfHeight * n;
    float yf = halfHeight * f;

    std::vector<QVector4D> farr = {
        QVector4D(xn, yn, n, 1.0f),
        QVector4D(-xn, yn, n, 1.0f),
        QVector4D(xn, -yn, n, 1.0f),
        QVector4D(-xn, -yn, n, 1.0f),

        QVector4D(xf, yf, f, 1.0f),
        QVector4D(-xf, yf, f, 1.0f),
        QVector4D(xf, -yf, f, 1.0f),
        QVector4D(-xf, -yf, f, 1.0f)
    };

    QVector3D v[8];
    for (int i = 0; i < 8; i++)
    {
        QVector4D ff = inv * farr[i];
        v[i].setX( ff.x() / ff.w() );
        v[i].setY( ff.y() / ff.w() );
        v[i].setZ( ff.z() / ff.w() );


    }

    drawQuad(v[7], v[5], v[4], v[6]);

    drawLine(v[0], v[1]);
    drawLine(v[0], v[2]);
    drawLine(v[3], v[1]);
    drawLine(v[3], v[2]);

    //drawLine(v[4], v[5]);
    //drawLine(v[4], v[6]);
    //drawLine(v[7], v[5]);
    //drawLine(v[7], v[6]);

    drawLine(v[0], v[4]);
    drawLine(v[1], v[5]);
    drawLine(v[3], v[7]);
    drawLine(v[2], v[6]);



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


QQuaternion MyGLWidget::ToQuaternion(QVector3D rot) // yaw (Z), pitch (Y), roll (X)
{
    // Abbreviations for the various angular functions
    double cy = cos(rot.z()* 0.5);
    double sy = sin(rot.z() * 0.5);
    double cp = cos(rot.y() * 0.5);
    double sp = sin(rot.y() * 0.5);
    double cr = cos(rot.x() * 0.5);
    double sr = sin(rot.x() * 0.5);

    QQuaternion q;
    q.setScalar(cr * cp * cy + sr * sp * sy);
    q.setX(sr * cp * cy - cr * sp * sy);
    q.setY(cr * sp * cy + sr * cp * sy);
    q.setZ(cr * cp * sy - sr * sp * cy);

    return q;
}
