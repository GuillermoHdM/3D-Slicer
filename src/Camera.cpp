#include "Camera.h"
#include "Editor.h"

void Camera::Update(GLFWwindow* window)
{
    m_RightPressed = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    m_MiddlePressed = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    static bool firstMouse = true;
    if (firstMouse) 
    {
        m_LastMousePos = glm::vec2(xpos, ypos);
        firstMouse = false;
    }
    glm::vec2 currentMouse(xpos, ypos);
    glm::vec2 delta = currentMouse - m_LastMousePos;
    if (m_RightPressed)
    {
        m_Theta -= delta.x * m_RotationSpeed;
        m_Phi -= delta.y * m_RotationSpeed;
        m_Phi = glm::clamp(m_Phi, 0.1f, glm::pi<float>() - 0.1f);
    }
    if (m_MiddlePressed)
    {
        glm::vec3 position = m_Target + m_Radius * glm::vec3(sin(m_Phi) * sin(m_Theta), cos(m_Phi), sin(m_Phi) * cos(m_Theta));
        glm::vec3 forward = glm::normalize(m_Target - position);
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 up = glm::normalize(glm::cross(right, forward));
        float panSpeed = m_PanSpeed * m_Radius;//the farther we are the faster we move

        m_Target -= right * delta.x * panSpeed;
        m_Target += up * delta.y * panSpeed;
    }
    m_LastMousePos = glm::vec2(xpos, ypos);


    glm::vec3 position = m_Target + m_Radius * glm::vec3(sin(m_Phi) * sin(m_Theta),cos(m_Phi),sin(m_Phi) * cos(m_Theta));
    m_View = glm::lookAt(position, m_Target, glm::vec3(0, 1, 0));
    m_Projection = glm::perspective(m_Fov, m_AspectRatio, m_Near,m_Far);

}

void Camera::Scroll(double offset)
{
    m_Radius -= static_cast<float>(offset) * m_ZoomSpeed;
    m_Radius = glm::clamp(m_Radius, 1.0f, 1000.0f);
    
}


//GLFW handles scroll only through callbacks, and it expects regular non member functions so here we are
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Editor* editor = static_cast<Editor*>(glfwGetWindowUserPointer(window));
    if (editor)
        editor->m_Camera.Scroll(yoffset);
    
}