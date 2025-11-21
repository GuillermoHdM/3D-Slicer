#pragma once
#include "Window.hpp"
#include "MyImgui.h"
#include "Grid.h"
#include "Camera.h"
#include "Object.h"
#include "Slicing.h"

#include <vector>
#include <utility>
#include <string>


class Editor
{

	ImguiNode MyImgui;
	float Dt = 0.016f;
	void UpdateImGui();
	//Renderer things vvv
	void R_Init();
	void R_Update();
	void DrawSliceDebug(const MeshSlice& slice, GLuint shader, float zOffset);
	GLuint MyShader;
	GLuint VtxShader;
	GLuint FragShader;

	GLuint SlicesShader;
	GLuint SlicesVtxShader;
	GLuint SlicesFragShader;

	GLuint SliceDebuTex;
	GLuint SliceDebugFBO;
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
	void AddNewObject(std::vector<Triangle>& in_triangles, std::string name);
	bool Update();
	struct Config
	{
		bool m_Wireframe = false;
		bool m_SliceDebug = false;
		int m_CurrSlice = 0;
		int m_TotSlices = -1;
		int m_SelectedObject = 0;
		float layerHeight = 0.1f;
		std::vector<MeshSlice> DebugSlices;
	} m_Config;
};


