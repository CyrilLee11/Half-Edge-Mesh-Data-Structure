#include "Mesh.h"


Vertex::Vertex(glm::vec3 p, int idx)
{
	pos = p;
	index = idx;
	e = nullptr;
}

Vertex::~Vertex()
{
}

std::vector<Half_Edge*> Vertex::Edges()
{
	Half_Edge* edge = e;
	std::vector<Half_Edge*> edges;
	edges.push_back(edge);
	edge = edge->next;
	edge = edge->opposite;

	while (edge != e)
	{
		edges.push_back(edge);
		edge = edge->next;
		edge = edge->opposite;
	}

	return edges;
}


Face::Face(int idx, int size)
{
	e = nullptr;
	index = idx;
	num = size;
	p =(int*) malloc(size * sizeof(int));
}

Face::~Face()
{
}

std::vector<Half_Edge*> Face::Edges()
{
	Half_Edge* edge = e;
	std::vector<Half_Edge*> edges;
	edges.push_back(edge);
	edge = edge->next;
	while (edge != e)
	{
		edges.push_back(edge);
		edge = edge->next;
	}

	return edges;
}


Half_Edge::Half_Edge(int idx)
{
	index = idx;
	opposite = nullptr;
	next = nullptr;
	prev = nullptr;
	vertex = nullptr;
	face = nullptr;
	is_boundary = true;
}

Half_Edge::~Half_Edge()
{
}


Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	p_iter = vertices.begin();
	while(p_iter != vertices.end())
	{
		Vertex* e = *p_iter;
		p_iter++;
		delete e;
	}

	f_iter = faces.begin();
	while (f_iter != faces.end())
	{
		Face* f = *f_iter;
		f_iter++;
		delete f;
	}

	e_iter = half_edges.begin();
	while (e_iter != half_edges.end())
	{
		Half_Edge* e = *e_iter;
		e_iter++;
		delete e;
	}

	e_iter = boundary_edges.begin();
	while (e_iter != boundary_edges.end())
	{
		Half_Edge* e = *e_iter;
		e_iter++;
		delete e;
	}
}

bool Mesh::load(const char* path)
{
	/* output the PATH */
	std::cout << path << std::endl;

	char buf1[256];
	_getcwd(buf1, sizeof(buf1));
	printf("%s\n", buf1);
	/* All the vertices are put into vectors */

	FILE* file;
	file = fopen(path, "r");
	if (file == nullptr)
	{
		std::cout << "Path Error, Fail to load obj" << std::endl;
		return false;
	}

	int flag = 0;
	//int k = 0;
	while (1)
	{
		//std::cout << k << std::endl;
		//k++;

		char lineHeader[64];
		int res = fscanf(file, "%s", lineHeader, 64);

		if (res == EOF)
		{
			break;
		}

		if (strcmp(lineHeader, "#") == 0)
		{
			flag = 0;
		}
		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf(file, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			out_vertices.push_back(vertex);
			flag = 1;
		}

		if (strcmp(lineHeader, "f") == 0)
		{
			std::vector<int> face;

			out_face.push_back(face);
			flag = 1;
		}

		if (atoi(lineHeader) > 0 && flag == 1)
		{
			//std::cout << atoi(lineHeader) << std::endl;
			out_face[out_face.size() - 1].push_back(atoi(lineHeader));
		}

	}

	std::cout << "Vertices:" << out_vertices.size() << std::endl;
	std::cout << "Faces:" << out_face.size() << std::endl;
	for (int i = 0; i < out_vertices.size(); i++)
	{
		Vertex* p = new Vertex(out_vertices[i], i);
		vertices.push_back(p);
	}

	return true;
}

void Mesh::BuildFromObj()
{
	/* Build Face Infomation */
	int count = 0;
	for (int i = 0; i < out_face.size(); i++)
	{

		Face* f = new Face(i, out_face[i].size());
		for (int q = 0; q < out_face[i].size(); q++)
		{
			f->p[q] = out_face[i][q] - 1;
		}
		
		
		/* Create Half-edge */
		/* Assume 1,0 means 1-->0 */

		//Half_Edge* e1[4];
		Half_Edge** edges_new;
		edges_new = (Half_Edge**)malloc(f->num * sizeof(Half_Edge*));
		for (int j = 0; j < f->num; j++)
		{
			edges_new[j] = new Half_Edge(count);
			
			/* face */
			edges_new[j]->face = f;
			p_iter = vertices.begin();

			for (int n = 0; n < f->p[j]; n++)
			{
				p_iter++;
			}
			Vertex* p = *p_iter;

			/* vertex */
			edges_new[j]->vertex = p;
			p->e = edges_new[j];

			count++;
		}
		
		for (int j = 0; j < f->num; j++)
		{
			if (j == f->num - 1)
			{
				edges_new[j]->next = edges_new[0];
				edges_new[j]->prev = edges_new[j - 1];
			}
			else if (j == 0)
			{
				edges_new[j]->prev = edges_new[f->num - 1];
				edges_new[j]->next = edges_new[j + 1];
			}
			else
			{
				edges_new[j]->next = edges_new[j + 1];
				edges_new[j]->prev = edges_new[j - 1];
			}
			
		}


		/* Save Outgoing Edges */
		for (int j = 0; j < f->num; j++)
		{

			if (outgoing_edges_map.find(f->p[j]) == outgoing_edges_map.end())
			{
				/* No such key*/
				std::list<Half_Edge*> edges;
				edges.push_back(edges_new[j]->next);
				outgoing_edges_map[f->p[j]] = edges;
			}
			else
			{
				outgoing_edges_map[f->p[j]].push_back(edges_new[j]->next);
			}
		}



		/* Set Face Half-Edge */
		f->e = edges_new[0];

		/* Set Opposite */
		for (int j = 0; j < f->num; j++)
		{
			int idx = edges_new[j]->vertex->index;

			e_iter = outgoing_edges_map[idx].begin();
			while (e_iter != outgoing_edges_map[idx].end())
			{
				Half_Edge* e = *e_iter;
				if (e->prev->vertex->index == edges_new[j]->vertex->index)
				{
					std::list<Half_Edge*>::iterator iter;
					int to_point = e->vertex->index;
					iter = outgoing_edges_map[to_point].begin();
					while (iter != outgoing_edges_map[to_point].end())
					{
						Half_Edge* e_prime = *iter;
						if (e_prime->index == edges_new[j]->index)
						{
							e->opposite = edges_new[j];
							e->is_boundary = false;
							edges_new[j]->opposite = e;
							edges_new[j]->is_boundary = false;

						}
						iter++;
					}

				}
				e_iter++;
			}
			/* Push the Half-edge to the list */
			half_edges.push_back(edges_new[j]);
		}

		/* Push the face to the list */
		faces.push_back(f);
	}

	/* Check for Boundary Edges */
	e_iter = half_edges.begin();
	while (e_iter != half_edges.end())
	{
		Half_Edge* e = *e_iter;
		/* Boundary */
		if (e->opposite == nullptr)
		{
			Half_Edge* boundary_edge = new Half_Edge(count);
			count++;
			boundary_edge->vertex = e->prev->vertex;
			boundary_edge->opposite = e;
			e->opposite = boundary_edge;
			e->is_boundary = false;
			boundary_edges.push_back(boundary_edge);
		}
		e_iter++;
	}

	/* Bind next for half-edges */
	e_iter = boundary_edges.begin();
	while (e_iter != boundary_edges.end())
	{
		Half_Edge* e = *e_iter;
		Half_Edge* next = e;
		next = next->opposite->prev->opposite;
		if (next->is_boundary == true)
		{
			e->next = next;
			next->prev = e;
		}

		while (e->next == nullptr)
		{
			next = next->prev->opposite;
			if (next->is_boundary == true)
			{
				e->next = next;
				next->prev = e;
			}
		}

		e_iter++;
	}
}

/* Get All the Boundary Edges */
std::vector<std::vector<Half_Edge*>> Mesh::Boundaries()
{
	std::vector<std::vector<Half_Edge*>> bound;
	if (boundary_edges.size() == 0) return bound;

	int size = boundary_edges.size();
	std::vector<Half_Edge*> b;
	b.reserve(size);

	e_iter = boundary_edges.begin();
	while (e_iter != boundary_edges.end())
	{
		Half_Edge* e = *e_iter;
		b.push_back(e);
		e_iter++;
	}

	bound.push_back(b);

	return bound;
}


void Mesh::Vertex_Valence()
{
	p_iter = vertices.begin();
	std::vector<int> valence;
	int max = 0;
	while (p_iter != vertices.end())
	{
		Vertex* v = *p_iter;
		int t = v->Edges().size();
		valence.push_back(t);
		if (max < t) max = t;

		p_iter++;
	}


	for (int i = 0; i < max+1; i++)
	{
		int num = 0;
		for (int j = 0; j < valence.size(); j++)
		{

			if (valence[j] == i) num++;
		}
		std::cout << "Valence-" << i << ": " << num << std::endl;
	}

}

void Mesh::Faces_Degree()
{
	f_iter = faces.begin();
	std::vector<int> degree;

	int max = 0;
	while (f_iter != faces.end())
	{
		Face* f = *f_iter;
		int t = f->num;
		degree.push_back(t);
		if (max < t) max = t;

		f_iter++;
	}
	for (int i = 3; i < max + 1; i++)
	{
		int num = 0;
		for (int j = 0; j < degree.size(); j++)
		{
			//std::cout << valence[j] << std::endl;
			if (degree[j] == i) num++;
		}
		std::cout << "Degree-" << i << ": " << num << std::endl;
	}
}

void Mesh::Bounday_Length()
{
	if (boundary_edges.size() == 0)
	{
		std::cout << "Length:" << 0 << std::endl;
		return;
	}
	e_iter = boundary_edges.begin();
	std::vector<Vertex*> V;
	V.reserve(boundary_edges.size());
	Half_Edge* e = *e_iter;
	Half_Edge* cur = e;
	V.push_back(cur->vertex);
	cur = cur->next;
	while (cur != e)
	{
		V.push_back(cur->vertex);
		cur = cur->next;
	}

	float length = 0;
	for (int i = 0; i < V.size(); i++)
	{
		int p = (i+1) % V.size();
		float t = distance(V[i], V[p]);
		//std::cout << t << std::endl;
		length += t;
	}
	std::cout << "Boundary Length: " << length << std::endl;
}

/* Shortest Path using Dijkstra */
std::vector<Half_Edge*> Mesh::Shorteset_Path(int idx1, int idx2)
{
	p_iter = vertices.begin();
	Vertex* v1;
	Vertex* v2;

	if (idx1 < idx2)
	{
		for (int i = 0; i < idx2-1; i++)
		{
			if (i == idx1-1) v1 = *p_iter;
			p_iter++;
		}
		v2 = *p_iter;
	}
	else
	{
		for (int i = 0; i < idx1-1; i++)
		{
			if (i == idx2-1) v2 = *p_iter;
			p_iter++;
		}
		v1 = *p_iter;
	}
	//std::cout << v1->index << " " << v2->index << std::endl;
	std::map<int, bool> P;
	std::vector<std::list<Half_Edge*>> paths;
	std::map<int, float> p_distance;
	std::vector<int> visited;
	for (int i = 0; i < vertices.size(); i++)
	{
		std::list<Half_Edge*> path;
		paths.push_back(path);
		P[i] = false;
		p_distance[i] = 999;
	}

	/* Initialize */
	
	visited.push_back(idx1-1);
	p_distance[idx1-1] = 0.0f;
	P[idx1-1] = true;
	int index = idx1-1;
	Vertex* cur = v1;

	while (index != idx2-1)
	{
		float min = 999;
		int min_index = -1;

		Vertex* temp = nullptr;
		std::vector<Half_Edge*> edges = cur->Edges();
		
		for (int i = 0; i < edges.size(); i++)
		{

			Half_Edge* e = edges[i];
			Vertex* v = e->opposite->vertex;
			if (P[v->index] == false)
			{
				float dist = distance(cur, v);

				if (p_distance[v->index] > dist + p_distance[cur->index])
				{

					p_distance[v->index] = dist + p_distance[cur->index];
					e_iter = paths[cur->index].begin();
					paths[v->index].clear();
					for (int j = 0; j < paths[cur->index].size(); j++)
					{
						Half_Edge* shortest_edge = *e_iter;
						paths[v->index].push_back(shortest_edge);
						e_iter++;
					}
					paths[v->index].push_back(e);
					
				}
			}
		}
		for (int i = 0; i < p_distance.size(); i++)
		{
			if (P[i] == false)
			{
				if (p_distance[i] < min)
				{
					min = p_distance[i];
					min_index = i;
				}
			}
		}
		p_iter = vertices.begin();
		for (int i = 0; i < min_index; i++)
		{
			p_iter++;
		}
		
		temp = *p_iter;
		P[temp->index] = true;
		cur = temp;
		index = cur->index;




	}

	std::vector<Half_Edge*> shortest_path;
	shortest_path.reserve(paths[idx2-1].size());

	e_iter = paths[idx2-1].begin();
	while (e_iter != paths[idx2-1].end())
	{
		Half_Edge* e = *e_iter;

		shortest_path.push_back(e->opposite);
		e_iter++;
	}

	return shortest_path;
	

}

float distance(Vertex* v1, Vertex* v2)
{
	glm::vec3 pos1 = v1->pos;
	glm::vec3 pos2 = v2->pos;

	glm::vec3 dis = pos1 - pos2;
	float length = sqrt(dis[0] * dis[0] + dis[1] * dis[1] + dis[2] * dis[2]);

	return length;
}