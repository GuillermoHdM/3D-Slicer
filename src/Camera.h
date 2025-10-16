#pragma once
#include "Math.hpp"
#include "Window.hpp"
class Camera
{
	float m_Fov = glm::radians(45.0f);
	float m_AspectRatio = 16.0f / 9.0f;
	float m_Near = 0.1f;
	float m_Far = 1000.0f;

	glm::vec3 m_Target = {0,0,0};//where its lookin at
	glm::vec2 m_LastMousePos = {0,0};

	float m_RotationSpeed = 0.005f;
	float m_ZoomSpeed = 1.0f;
	float m_Radius = 10.0f;
	float m_Theta = glm::radians(45.0f);//azimutal (horizontal)
	float m_Phi = glm::radians(45.0f);//polar (vertical)
	bool m_RightPressed = false;
public:

	glm::mat4 m_View;
	glm::mat4 m_Projection;
	void Update(GLFWwindow* window);
	void Scroll(double offset);
};


//GLFW handles scroll only through callbacks, and it expects regular non member functions so here we are
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);