#ifndef MYSTRUCTS_H
#define MYSTRUCTS_H

#include <vector>
#include <string>
#include <happly.h>

#include <QImage>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <uchar.h>

typedef unsigned char uchar;

struct PointCloud
{
    //3 floats position, 3 bytes color
    std::vector<float> vertices;
    int points = 0;

public:
    PointCloud(std::string ply_filename)
    {
        happly::PLYData plyIn(ply_filename);

        std::vector<float> vertex_x = plyIn.getElement("vertex").getProperty<float>("x");
        std::vector<float> vertex_y = plyIn.getElement("vertex").getProperty<float>("y");
        std::vector<float> vertex_z = plyIn.getElement("vertex").getProperty<float>("z");

        std::vector<uchar> color_r = plyIn.getElement("vertex").getProperty<uchar>("red");
        std::vector<uchar> color_g = plyIn.getElement("vertex").getProperty<uchar>("green");
        std::vector<uchar> color_b = plyIn.getElement("vertex").getProperty<uchar>("blue");



        for(int i = 0; i < vertex_x.size();i++)
        {
            vertices.push_back(vertex_x[i]);
            vertices.push_back(vertex_y[i]);
            vertices.push_back(vertex_z[i]);

            vertices.push_back(color_r[i] / 255.0f);
            vertices.push_back(color_g[i] / 255.0f);
            vertices.push_back(color_b[i] / 255.0f);
        }

        points = vertex_x.size();
    }
};


struct Frustum{

    QOpenGLTexture *texture;
    QVector3D v[8];
    QMatrix4x4 modelMat;

    std::vector<float> quadVertex;

    Frustum(){

    }

    Frustum(QString filename, QMatrix4x4 model, QVector3D pos, QVector3D rotation){
        QImage image(filename);
        texture = new QOpenGLTexture(image);

        modelMat = model;

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


        for (int i = 0; i < 8; i++)
        {
            QVector4D ff = inv * farr[i];
            v[i].setX( ff.x() / ff.w() );
            v[i].setY( ff.y() / ff.w() );
            v[i].setZ( ff.z() / ff.w() );
        }


        setQuadVertex(v[7], v[5], v[4], v[6]);
    }

    ~Frustum()
    {
        delete texture;
    }
private:
    void setQuadVertex(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3)
    {
        quadVertex.clear();
        quadVertex = {
            p0.x(), p0.y(), p0.z(), 0.0f, 0.0f,
            p1.x(), p1.y(), p1.z(), 0.0f, 1.0f,
            p2.x(), p2.y(), p2.z(), 1.0f, 1.0f,

            p2.x(), p2.y(), p2.z(), 1.0f, 1.0f,
            p3.x(), p3.y(), p3.z(), 1.0f, 0.0f,
            p0.x(), p0.y(), p0.z(), 0.0f, 0.0f
        };

    }

    QQuaternion ToQuaternion(QVector3D rot) // yaw (Z), pitch (Y), roll (X)
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




};

#endif
