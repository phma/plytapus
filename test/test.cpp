#include <iostream>

#include "plytapus.h"

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
	std::cout << "Finished" << std::endl;
}
