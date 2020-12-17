#pragma once

// Std. Includes
#include <vector>

#include<glad/include/glad/glad.h>
#include<GLFW/glfw3.h>
// GL Includes
//#include <GL/glew.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>



// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    AROUND
};

// Default camera values
const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  10.0f;
const GLfloat ZOOM_SPEED =  4.0f;
const GLfloat SENSITIVTY =  0.15f;
const GLfloat ZOOM       =  45.0f;
const GLfloat MOUSE_PRESSED_MOVE_THRESHOLD = 0.0f;
const GLfloat DRAG_MOVE_COEFFICIENT = 0.05f;

const glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);
const glm::vec3 WORLD_RIGHT(1.0f, 0.0f, 0.0f);
const glm::vec3 WORLD_FRONT(0.0f, 0.0f, -1.0f);


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    glm::vec3 Camera_World_Center;
    // Eular Angles
    GLfloat Yaw;
    GLfloat Pitch;
    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;


    // Constructor with vectors

    Camera(glm::vec3 position = glm::vec3(0.0f, 30.0f, 30.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 world_center = glm::vec3(0.0f, 0.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        this->Position = position;
        this->Camera_World_Center = world_center;
        this->WorldUp = up;
        this->Yaw = yaw;
        this->Pitch = -45.0f;
        this->updateCameraVectorsUsingEulerAngle();
    }
    // Constructor with scalar values
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        this->Position = glm::vec3(posX, posY, posZ);
        this->WorldUp = glm::vec3(upX, upY, upZ);
        this->Yaw = yaw;
        this->Pitch = pitch;
        //this->MovementSpeed = 0.01f;
        this->updateCameraVectorsUsingEulerAngle();
    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->MovementSpeed * deltaTime;
        if (direction == FORWARD) {
            //this->Position += WORLD_FRONT * velocity;    
            this->Position += this->Up * velocity;
            this->Camera_World_Center += this->Up * velocity;
        }

        if (direction == BACKWARD) {
            //this->Position -= WORLD_FRONT * velocity;
            this->Position -= this->Up * velocity;
            this->Camera_World_Center -= this->Up * velocity;
        }
            
        if (direction == LEFT) {
            //this->Position -= WORLD_RIGHT * velocity;
            this->Position -= this->Right * velocity;
            this->Camera_World_Center -= this->Right * velocity;
        }
            
        if (direction == RIGHT) {
            //this->Position += WORLD_RIGHT * velocity;
            this->Position += this->Right * velocity;
            this->Camera_World_Center += this->Right * velocity;
        }
            
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(GLfloat yoffset)
    {
        if (this->Zoom >= 1.0f && this->Zoom <= 100.0f)
            this->Zoom -= ZOOM_SPEED * yoffset;
        if (this->Zoom <= 1.0f)
            this->Zoom = 1.0f;
        if (this->Zoom >= 100.0f)
            this->Zoom = 100.0f;
    }


    void RotateAlongCenterRaxis(glm::vec3 axis, GLfloat rad) {
        glm::mat3 rot(cos(rad) + pow(axis.x, 2) * (1 - cos(rad)), axis.x * axis.y * (1 - cos(rad)) - axis.z * sin(rad), axis.x * axis.z * (1 - cos(rad)) + axis.y * sin(rad),
            axis.y * axis.x * (1 - cos(rad)) + axis.z * sin(rad), cos(rad) + pow(axis.y, 2) * (1 - cos(rad)), axis.y * axis.z * (1 - cos(rad)) - axis.x * sin(rad),
            axis.z * axis.x * (1 - cos(rad)) - axis.y * sin(rad), axis.z * axis.y * (1 - cos(rad)) + axis.x * sin(rad), cos(rad) + pow(axis.z, 2) * (1 - cos(rad)));

        glm::vec3 diff_vec = this->Position - this->Camera_World_Center;

        this->Position = rot * diff_vec + this->Camera_World_Center;

        this->Front = glm::normalize(this->Camera_World_Center - this->Position);
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
        this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    }

    // Mouse right button pressed.
    void ProcessMouseDragRight(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        GLfloat offset = sqrt(pow(xoffset, 2) + pow(yoffset, 2));
        glm::vec3 axis = glm::normalize(glm::vec3(-yoffset, xoffset, 0));
        GLfloat rotation_angle_r = glm::radians(offset);

        RotateAlongCenterRaxis(axis, rotation_angle_r);

    }

    // Mouse middle button pressed.
    void ProcessMouseDragMiddle(GLfloat xoffset, GLfloat yoffset) {
        if (abs(xoffset) < MOUSE_PRESSED_MOVE_THRESHOLD) xoffset = 0.0f;
        if (abs(xoffset) < MOUSE_PRESSED_MOVE_THRESHOLD) yoffset = 0.0f;
        //this->Position -= WORLD_UP * yoffset * DRAG_MOVE_COEFFICIENT;
        this->Position -= this->Up * yoffset * DRAG_MOVE_COEFFICIENT;
        this->Camera_World_Center -= this->Up * yoffset * DRAG_MOVE_COEFFICIENT;
        //this->Position -= RIGHT * xoffset * DRAG_MOVE_COEFFICIENT;
        this->Position -= this->Right * xoffset * DRAG_MOVE_COEFFICIENT;
        this->Camera_World_Center -= this->Right * xoffset * DRAG_MOVE_COEFFICIENT;
    }

private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectorsUsingEulerAngle()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        this->Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
    }
};