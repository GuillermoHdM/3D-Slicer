#include <vector>
#include "iostream"
#include "FileLoader.h"

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
	for (int i = 0; i < count; ++i) 
	{
		std::string path = paths[i];
		std::cout << "  " << path << "\n";
	}
	//^^^^^^^^^^^^
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
				//triangles = LoadAsciiSTL(path)
			}
			//CreateMeshFromTriangles(triangles);
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
		throw std::runtime_error("Invalid STL file: " + path);
	}
	if (numTriangles == 0)
	{
		throw std::runtime_error("Empty STL file: " + path);
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
		throw std::runtime_error("Something went wrong reading: " + path);
	}
}