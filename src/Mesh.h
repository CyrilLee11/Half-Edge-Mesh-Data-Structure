#ifndef MESH_H
#define MESH_H
#include <glm\glm\glm.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <list>
#include <map>
#include <direct.h>

class Half_Edge;

class Vertex
{
  public:
    Vertex(glm::vec3 p, int idx);
    ~Vertex();

	std::vector<Half_Edge*> Edges();

	glm::vec3 pos;
	Half_Edge* e;
	int index;
};

class Face
{
  public:
    Face(int idx, int size);
    ~Face();

	std::vector<Half_Edge*> Edges();

	Half_Edge* e;
	int* p;
	int num;
	int index;
};

class Half_Edge
{
  public:
    Half_Edge(int idx);
    ~Half_Edge();

    Half_Edge* opposite;
    Half_Edge* next;
	Half_Edge* prev;
    Vertex* vertex;
    Face* face;
    int index;
	bool is_boundary;
};

class Mesh
{
  public:
    Mesh();
    ~Mesh();

	void BuildFromObj();
	bool load(const char* path);
	std::vector<std::vector<Half_Edge*>> Boundaries();
	void Vertex_Valence();
	void Faces_Degree();
	void Bounday_Length();
	std::vector<Half_Edge*> Shorteset_Path(int idx1, int idx2);

	std::list<Vertex*> vertices;
	std::list<Face*> faces;
	std::list<Half_Edge*> half_edges;
	std::list<Half_Edge*> boundary_edges;
	std::map<int, std::list<Half_Edge*>> outgoing_edges_map;

	std::list<Vertex*>::iterator p_iter;
	std::list<Face*>::iterator f_iter;
	std::list<Half_Edge*>::iterator e_iter;
	std::list<Vertex*>::reverse_iterator p_iter_r;
	std::list<Face*>::reverse_iterator f_iter_r;
	std::list<Half_Edge*>::reverse_iterator e_iter_r;

	std::vector<glm::vec3> out_vertices;
	std::vector<std::vector<int>> out_face;
}; 

float distance(Vertex* v1, Vertex* v2);


#endif