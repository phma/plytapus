#include <iostream>

#define PLYTAPUS_BEING_BUILT
#include "plytapus.h"

#define PHI 1.6180339887498948482

bool areClose(double a, double b)
{
	const double EPSILON = 1.0e-1;
	return abs(a - b) < EPSILON;
}

struct Vertex
{
	Vertex(double x, double y, double z) : x(x), y(y), z(z) {};
	bool operator==(const Vertex& other) const { return areClose(x, other.x) && areClose(y, other.y) && areClose(z, other.z); };
	double x, y, z;
};

struct Mesh
{
	typedef unsigned int VertexIndex;
	typedef std::array<VertexIndex, 3> TriangleIndices;
	typedef std::vector<Vertex> VertexList;
	typedef std::vector<TriangleIndices> TriangleIndicesList;

	Mesh(const VertexList& vertices, const TriangleIndicesList& triangles) : vertices(vertices), triangles(triangles) {};
	Mesh(VertexList&& vertices, TriangleIndicesList&& triangles) : vertices(std::move(vertices)), triangles(std::move(triangles)) {};

	VertexList vertices;
	TriangleIndicesList triangles;
};

void readply(PATH_STRING filename, Mesh::VertexList& vertices, Mesh::TriangleIndicesList& triangles)
{
	plytapus::File file(filename);
	const auto& definitions = file.definitions();

	const auto vertexDefinition = definitions.at(0);
	const size_t vertexCount = vertexDefinition.size;
	vertices.reserve(vertexCount);
	plytapus::ElementReadCallback vertexCallback = [&vertices](plytapus::ElementBuffer& e)
		{
			vertices.emplace_back(e[0], e[1], e[2]);
		};

	const auto triangleDefinition = definitions.at(1);
	const size_t triangleCount = triangleDefinition.size;
	triangles.reserve(triangleCount);
	plytapus::ElementReadCallback triangleCallback = [&triangles](plytapus::ElementBuffer& e)
	{
		triangles.emplace_back(std::move(Mesh::TriangleIndices{ e[0], e[1], e[2]}));
	};

	file.setElementReadCallback("vertex", vertexCallback);
	file.setElementReadCallback("face", triangleCallback);
	file.read();
}

void writeply(PATH_STRING filename, const plytapus::ElementsDefinition& definitions, Mesh::VertexList& vertices, Mesh::TriangleIndicesList& triangles, plytapus::File::Format format)
{
	plytapus::FileOut file(filename, format);
	file.setElementsDefinition(definitions);
	
	plytapus::ElementWriteCallback vertexCallback = [&vertices](plytapus::ElementBuffer& e, size_t index)
		{
			const auto& v = vertices[index];
			e[0] = v.x;
			e[1] = v.y;
			e[2] = v.z;
		};
	plytapus::ElementWriteCallback triangleCallback = [&triangles](plytapus::ElementBuffer& e, size_t index)
		{
			e.reset(3);
			const auto& t = triangles[index];
			e[0] = t[0];
			e[1] = t[1];
			e[2] = t[2];
		};

	file.setElementWriteCallback("vertex", vertexCallback);
	file.setElementWriteCallback("face", triangleCallback);
	file.write();
}

bool compare_vertices(const Mesh::VertexList& left, const Mesh::VertexList& right)
{
	if (left.size() != right.size())
	{
		std::cout << "Length mismatch" << std::endl;
		return false;
	}

	int errors = 0;
	for (unsigned int i = 0; i < left.size(); ++i)
	{
		if (!(left[i] == right[i]))
		{
			std::cout << "vertex " << i << " is different" << std::endl;
			++errors;
		}
	}
	return errors == 0;
}

bool compare_triangles(const Mesh::TriangleIndicesList& left, const Mesh::TriangleIndicesList& right)
{
	if (left.size() != right.size())
	{
		std::cout << "Length mismatch" << std::endl;
		return false;
	}

	int errors = 0;
	for (unsigned int i = 0; i < left.size(); ++i)
	{
		if (!(left[i] == right[i]))
		{
			std::cout << "triangle " << i << " is different" << std::endl;
			++errors;
		}
	}
	return errors == 0;
}

void addTriangle(Mesh::TriangleIndicesList &tris,int a,int b,int c)
{
  Mesh::TriangleIndices tri;
  tri[0]=a;
  tri[1]=b;
  tri[2]=c;
  tris.push_back(tri);
}

void drawIcosahedron()
// This is an example of how to make a mesh from scratch.
{
  Mesh::VertexList icosaVertices;
  Mesh::TriangleIndicesList icosaTriangles;
  std::vector<plytapus::Property> vertexProperties,faceProperties;
  vertexProperties.push_back(plytapus::Property("x",plytapus::Type::DOUBLE,false));
  vertexProperties.push_back(plytapus::Property("y",plytapus::Type::DOUBLE,false));
  vertexProperties.push_back(plytapus::Property("z",plytapus::Type::DOUBLE,false));
  faceProperties.push_back(plytapus::Property("vertex_index",plytapus::Type::INT,true));
  plytapus::Element vertexElement("vertex",12,vertexProperties);
  plytapus::Element faceElement("face",20,faceProperties);
  plytapus::ElementsDefinition elements;
  elements.push_back(vertexElement);
  elements.push_back(faceElement);

  icosaVertices.emplace_back(0,1,PHI);
  icosaVertices.emplace_back(0,1,-PHI);
  icosaVertices.emplace_back(0,-1,-PHI);
  icosaVertices.emplace_back(0,-1,PHI);
  icosaVertices.emplace_back(1,PHI,0);
  icosaVertices.emplace_back(1,-PHI,0);
  icosaVertices.emplace_back(-1,-PHI,0);
  icosaVertices.emplace_back(-1,PHI,0);
  icosaVertices.emplace_back(PHI,0,1);
  icosaVertices.emplace_back(-PHI,0,1);
  icosaVertices.emplace_back(-PHI,0,-1);
  icosaVertices.emplace_back(PHI,0,-1);

  addTriangle(icosaTriangles,0,3,8);
  addTriangle(icosaTriangles,3,0,9);
  addTriangle(icosaTriangles,2,1,11);
  addTriangle(icosaTriangles,1,2,10);

  addTriangle(icosaTriangles,4,7,0);
  addTriangle(icosaTriangles,7,4,1);
  addTriangle(icosaTriangles,6,5,3);
  addTriangle(icosaTriangles,5,6,2);

  addTriangle(icosaTriangles,8,11,4);
  addTriangle(icosaTriangles,11,8,5);
  addTriangle(icosaTriangles,10,9,7);
  addTriangle(icosaTriangles,9,10,6);

  addTriangle(icosaTriangles,0,8,4);
  addTriangle(icosaTriangles,1,4,11);
  addTriangle(icosaTriangles,3,5,8);
  addTriangle(icosaTriangles,2,11,5);
  addTriangle(icosaTriangles,0,7,9);
  addTriangle(icosaTriangles,1,10,7);
  addTriangle(icosaTriangles,3,9,6);
  addTriangle(icosaTriangles,2,6,10);

  writeply(Str("icosahedron.ply"),elements,icosaVertices,icosaTriangles,plytapus::File::Format::BINARY_LITTLE_ENDIAN);
}

int main()
{
	Mesh::VertexList ascii_vertices;
	Mesh::TriangleIndicesList ascii_triangles;
	readply(Str("test.ply"), ascii_vertices, ascii_triangles);

	Mesh::VertexList bin_vertices;
	Mesh::TriangleIndicesList bin_triangles;
	readply(Str("test_bin.ply"), bin_vertices, bin_triangles);

	compare_vertices(ascii_vertices, bin_vertices);
	compare_triangles(ascii_triangles, bin_triangles);

	plytapus::File refFile(Str("test.ply"));
	writeply(Str("write_ascii.ply"), refFile.definitions(), ascii_vertices, ascii_triangles, plytapus::File::Format::ASCII);
	writeply(Str("write_bin.ply"), refFile.definitions(), ascii_vertices, ascii_triangles, plytapus::File::Format::BINARY_LITTLE_ENDIAN);
	writeply(Str("write_binbe.ply"), refFile.definitions(), ascii_vertices, ascii_triangles, plytapus::File::Format::BINARY_BIG_ENDIAN);

	Mesh::VertexList readback_ascii_vertices;
	Mesh::TriangleIndicesList readback_ascii_triangles;
	readply(Str("write_ascii.ply"), readback_ascii_vertices, readback_ascii_triangles);
	compare_vertices(ascii_vertices, readback_ascii_vertices);
	compare_triangles(ascii_triangles, readback_ascii_triangles);

	Mesh::VertexList readback_bin_vertices;
	Mesh::TriangleIndicesList readback_bin_triangles;
	readply(Str("write_bin.ply"), readback_bin_vertices, readback_bin_triangles);
	compare_vertices(bin_vertices, readback_bin_vertices);
	compare_triangles(bin_triangles, readback_bin_triangles);

	drawIcosahedron();
	std::cout << "Finished" << std::endl;
}
