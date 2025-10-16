#pragma once
#include "Window.hpp"
#include "MyImgui.h"
#include "Grid.h"
#include "Camera.h"
#include <vector>
#include <utility>


class Editor
{

	ImguiNode MyImgui;
	float Dt = 0.016f;
	void UpdateImGui();

	struct Renderer
	{
		void Init();
		void Update();
		GLuint MyShader;
		GLuint VtxShader;
		GLuint FragShader;
		GLuint quadVAO;
		GLuint quadVBO;
	};
	Renderer MyRenderer;
	Grid m_Grid;
	Camera m_Camera;
public:
	Window W;
	Editor();
	Editor(Editor& s) : W(ivec2(1920 * 0.75f, 1080 * 0.75f), true) { Dt = s.Dt; };
	Editor& operator=(const Editor& rhs) {Dt = rhs.Dt; return *this;};
	~Editor();

	bool Update();
};


