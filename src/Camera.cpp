#include "Camera.h"

void Camera::Update(GLFWwindow* window)
{
    m_RightPressed = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    static bool firstMouse = true;
    if (firstMouse) 
    {
        m_LastMousePos = glm::vec2(xpos, ypos);
        firstMouse = false;
    }
    if (m_RightPressed)
    {
        glm::vec2 currentMouse(xpos, ypos);
        glm::vec2 delta = currentMouse - m_LastMousePos;

        m_Theta -= delta.x * m_RotationSpeed;
        m_Phi -= delta.y * m_RotationSpeed;
        m_Phi = glm::clamp(m_Phi, 0.1f, glm::pi<float>() - 0.1f);
    }

    m_LastMousePos = glm::vec2(xpos, ypos);


    glm::vec3 position = m_Target + m_Radius * glm::vec3(
        sin(m_Phi) * sin(m_Theta),
        cos(m_Phi),
        sin(m_Phi) * cos(m_Theta));

    m_View = glm::lookAt(position, m_Target, glm::vec3(0, 1, 0));
    m_Projection = glm::perspective(m_Fov, m_AspectRatio, m_Near,m_Far);

}

void Camera::Scroll(double offset)
{
    //REGULAR "HARSH" SCROLL
    m_Radius -= static_cast<float>(offset) * m_ZoomSpeed;
    m_Radius = glm::clamp(m_Radius, 1.0f, 100.0f);
    
    //smoother scroll
    /*
    float zoomFactor = 1.0f - static_cast<float>(offset) * 0.1f;
    m_Radius *= zoomFactor;
    m_Radius = glm::clamp(m_Radius, 1.0f, 100.0f);
    */
}


//GLFW handles scroll only through callbacks, and it expects regular non member functions so here we are
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Camera* camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (camera)
        camera->Scroll(yoffset);
}