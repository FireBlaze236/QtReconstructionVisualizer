#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QMatrix4x4>



enum class CameraMovement{
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT,
    NONE
};

class Camera{
private:
    QMatrix4x4 *_viewMatrix;
    QMatrix4x4 *_projPerspectiveMatrix;

    QVector3D _position;
    QVector3D _upDirection;
    QVector3D _target;

    QVector3D _cameraForward;


    float yaw = -90.0f;
    float pitch = 0;

public:
    Camera(QVector3D& position, QVector3D& up, QVector3D& lookTarget)
        : _position(position), _upDirection(up), _target(lookTarget)
    {
        _viewMatrix = new QMatrix4x4();
        _projPerspectiveMatrix = new QMatrix4x4();



        _cameraForward = _target;

        _viewMatrix->setToIdentity();
        _viewMatrix->lookAt(_position, _position+_cameraForward, _upDirection);


    };

    ~Camera()
    {
        delete _viewMatrix;
        delete _projPerspectiveMatrix;
    };
    QMatrix4x4 getViewMatrix()
    {
        return *_viewMatrix;
    };

    QMatrix4x4 getPerspectiveProjection()
    {
        return *_projPerspectiveMatrix;
    };


    void moveCamera(float speed = 1.0f, CameraMovement movement = CameraMovement::NONE)
    {
        if(movement== CameraMovement::NONE) return;


        QVector3D left = QVector3D::crossProduct(_upDirection.normalized(), _cameraForward);

        if(movement== CameraMovement::LEFT)
        {
            _position += left * speed;

        }
        else if(movement == CameraMovement::RIGHT)
        {
            _position += -left * speed;
        }
        else if(movement == CameraMovement::FORWARD)
        {
            _position += _cameraForward * speed;
        }

        else if(movement == CameraMovement::BACKWARD)
        {
            _position += -_cameraForward * speed;
        }


        _viewMatrix->setToIdentity();
        _viewMatrix->lookAt(_position, _position+_cameraForward, _upDirection);

    };

    void setPerspectiveProjection(float fov, float aspect, float near, float far)
    {
        _projPerspectiveMatrix->setToIdentity();
        _projPerspectiveMatrix->perspective(fov, aspect, 0.01f, 10000.0f);
    };


    void setCameraLook(float xoffset, float yoffset)
    {
        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if(pitch > 89.0f)
          pitch =  89.0f;
        if(pitch < -89.0f)
          pitch = -89.0f;

        float ryaw = qDegreesToRadians(yaw);
        float rpitch = qDegreesToRadians(pitch);

        QVector3D direction;
        direction.setX(cos(ryaw) * cos(rpitch));
        direction.setY(sin(rpitch));
        direction.setZ(sin(ryaw) * cos(rpitch));

        _cameraForward = direction.normalized();

        _viewMatrix->setToIdentity();
        _viewMatrix->lookAt(_position, _position+_cameraForward, _upDirection);
    }



};

#endif // CAMERA_H
