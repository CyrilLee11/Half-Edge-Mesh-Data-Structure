#include "Mesh.h"
#include <iostream>

int main()
{
	Mesh roof, cow, lilium;

	if (!roof.load("..\\..\\data\\roof.obj")) {
		system("pause");
		return 0;
	}

	if (!cow.load("..\\..\\data\\cow.obj")) {
		system("pause");
		return 0;
	}

	if (!lilium.load("..\\..\\data\\lilium.obj")) {
		system("pause");
		return 0;
	}
	std::cout << "cow ----------------------" << std::endl;
	cow.BuildFromObj();
	cow.Vertex_Valence();
	cow.Faces_Degree();
	cow.Bounday_Length();
	std::cout << "shortese path" << std::endl;
	std::vector<Half_Edge*> s_h;

	s_h = cow.Shorteset_Path(1700, 1778);
	for (int i = 0; i < s_h.size(); i++)
	{
		if (i == 0)
		{
			std::cout << s_h[i]->opposite->vertex->index + 1 << "----";
		}
		else if (i == s_h.size() - 1)
		{
			std::cout << s_h[i]->vertex->index + 1 << std::endl;
		}
		else {
			std::cout << s_h[i]->vertex->index + 1 << "----";
		}
	}
	//std::cout << s_h.size() << std::endl;
	//std::cout << s_h[0]->index << " " << s_h[1]->index << std::endl;

	std::cout << "roof ----------------------" << std::endl;
	roof.BuildFromObj();
	roof.Vertex_Valence();
	roof.Faces_Degree();
	roof.Bounday_Length();

	std::cout << "lilium ----------------------" << std::endl;
	lilium.BuildFromObj();
	lilium.Vertex_Valence();
	lilium.Faces_Degree();
	lilium.Bounday_Length();
	//std::list<Vertex*>::iterator p;

	//p = roof.vertices.begin();
	//Vertex* v1 = *p;
	//p++;
	//Vertex* v = *p;
	//std::cout << "test:" << v->Edges().size() << std::endl;
	//std::cout << "test:" << distance(v1,v) << std::endl;




	
	std::cout << sizeof(int) << " " << sizeof(Half_Edge*) << std::endl;


	system("pause");
	return 0;
}