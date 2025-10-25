#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include "FileLoader.h"
#include "Object.h"
#include "../demo/Editor.h"

bool IsBinary(const std::string& path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file)
	{
		throw std::runtime_error("No se pudo abrir el archivo.");
	}
	//File OK

	//take the possible header
	char header[80];
	file.read(header, 80);

	//take the possible number of triangles
	uint32_t numTriangles = 0;
	file.read(reinterpret_cast<char*>(&numTriangles), sizeof(numTriangles));

	//size of the file
	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();

	//this should be the size of a binary file
	std::streamsize expectedSize = 84 + static_cast<std::streamsize>(numTriangles) * 50;
	if (expectedSize == fileSize)
	{
		return true; //educated guess that this is binary
	}
	else
	{
		std::string headerStr(header, header + 5);
		if (headerStr == "solid")//if it doesnt look like binary, more often than not it starts with solir
			return false;
	}
	return true;
}

void DropCallback(GLFWwindow* window, int count, const char** paths)
{
	//for debug vvv
	std::cout << "Dropped File:\n";
	std::vector<std::string> FileNames;//to support loading several objects
	for (int i = 0; i < count; ++i) 
	{
		std::string path = paths[i];
		std::cout << "  " << path << "\n";
		FileNames.push_back(GetFileName(path));
	}
	//^^^^^^^^^^^^
	Editor* editor = static_cast<Editor*>(glfwGetWindowUserPointer(window));//recover the editor pointer
	if (!editor)
	{
		return;
	}
	for (int i = 0; i < count; ++i) 
	{
		std::string path = paths[i];
		if (path.ends_with(".stl") || path.ends_with(".STL")) 
		{
			std::vector<Triangle> triangles;

			if (IsBinary(path))
			{
				std::cout << "Binary" << std::endl;
				LoadBinarySTL(path, triangles);
			}
			else
			{
				std::cout << "ASCII" << std::endl;
				LoadAsciiSTL(path, triangles);
			}
			editor->AddNewObject(triangles, FileNames[i]);
		}
		else 
		{
			std::cout << "Formato no soportado: " << path << "\n";
		}
	}
}


void LoadBinarySTL(const std::string& path, std::vector<Triangle> &out_triangles)
{
	std::ifstream file(path, std::ios::binary);
	if (!file)
	{
		throw std::runtime_error("Cant open STL file: " + path);
	}
	// Header of 80 bytes, I think it can be ingnored
	char header[80];
	file.read(header, 80);

	//how many triangles to load
	uint32_t numTriangles = 0;
	file.read(reinterpret_cast<char*>(&numTriangles), sizeof(numTriangles));

	if (!file)
	{
		throw std::runtime_error("Invalid STL Binary file: " + path);
	}
	if (numTriangles == 0)
	{
		throw std::runtime_error("Empty STL Binary file: " + path);
	}

	if (out_triangles.empty() == false)//not empty output?
	{
		out_triangles = std::vector<Triangle>();//empty it
	}
	out_triangles.resize(numTriangles);
	for (uint32_t i = 0; i < numTriangles; i++)
	{
		//for each trianle: 50 bytes
		//normal(12), 3v(36), attribute(2)
		file.read(reinterpret_cast<char*>(&out_triangles[i].n), sizeof(glm::vec3));
		file.read(reinterpret_cast<char*>(&out_triangles[i].A), sizeof(glm::vec3));
		file.read(reinterpret_cast<char*>(&out_triangles[i].B), sizeof(glm::vec3));
		file.read(reinterpret_cast<char*>(&out_triangles[i].C), sizeof(glm::vec3));

		file.ignore(2);//inore the attribute
	}
	if (!file)
	{
		throw std::runtime_error("Something went wrong reading STL Binary: " + path);
	}
}


void LoadAsciiSTL(const std::string& path, std::vector<Triangle>& out_triangles)
{
	std::ifstream file(path, std::ios::binary);
	if (!file)
	{
		throw std::runtime_error("Cant open ASCII STL file: " + path);
	}
	std::string line;
	Triangle tri;
	int vertexCount = 0;

	while (std::getline(file, line))//read each line
	{
		std::istringstream ss(line);
		std::string word;
		ss >> word;
		if (word == "facet")
		{
			std::string normalWord;
			ss >> normalWord >> tri.n.x >> tri.n.y >> tri.n.z;
			vertexCount = 0;
		}
		else if (word == "vertex")
		{
			glm::vec3 v;
			ss >> v.x >> v.y >> v.z;

			if (vertexCount == 0)
			{
				tri.A = v;
			}
			else if (vertexCount == 1)
			{
				tri.B = v;
			}
			else if (vertexCount == 2)
			{
				tri.C = v;
			}

			vertexCount++;
		}
		else if (word == "endfacet")//end of the face
		{
			//if invalid normal:
			if (glm::length(tri.n) < 0.0001f)
			{
				tri.n = glm::normalize(glm::cross(tri.B - tri.A, tri.C - tri.A));
			}
			out_triangles.push_back(tri);
		}
	}
	if (out_triangles.empty())
	{
		throw std::runtime_error("No triangles in STL ASCII: " + path);
	}
}


std::string GetFileName(const std::string& path)
{
	//skip the path part
	size_t slashPos = path.find_last_of("/\\");
	//avoid the extension
	size_t dotPos = path.find_last_of('.');

	if (slashPos == std::string::npos)
	{
		slashPos = -1;
	}
	if (dotPos == std::string::npos || dotPos < slashPos)
	{
		dotPos = path.size();
	}
	return path.substr(slashPos + 1, dotPos - slashPos - 1);
}