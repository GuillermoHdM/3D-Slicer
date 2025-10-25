#pragma once
#include "Window.hpp"
#include "MyImgui.h"
#include "Grid.h"
#include "Camera.h"
#include "Object.h"
#include <vector>
#include <utility>


class Editor
{

	ImguiNode MyImgui;
	float Dt = 0.016f;
	void UpdateImGui();
	//Renderer things vvv
	void R_Init();
	void R_Update();
	GLuint MyShader;
	GLuint VtxShader;
	GLuint FragShader;
	//^^^^^^^^^^^^^^^^^
	Grid m_Grid;
	std::vector<Object> m_Objects;


public:
	Camera m_Camera;
	Window W;
	Editor();
	Editor(Editor& s) : W(ivec2(1920 * 0.75f, 1080 * 0.75f), true) { Dt = s.Dt; };
	Editor& operator=(const Editor& rhs) {Dt = rhs.Dt; return *this;};
	~Editor();
	void AddNewObject(std::vector<Triangle>& in_triangles);
	bool Update();
	struct Config
	{
		bool m_Wireframe = false;
	} m_Config;
};


