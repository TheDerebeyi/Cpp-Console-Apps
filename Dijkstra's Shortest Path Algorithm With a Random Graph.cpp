//Dijkstra's Shortest Path Algorithm With a Random Graph.
//Created by Mert Kahraman, October 2, 2020.
#include <ctime>
#include <iostream>
#include <random>
const int SIZE = 50;
const int max_cost = 101;
const double DENSITY = 0.40;
using namespace std;
inline double prob(int density_or_cost) //0 for density, 1 for cost 
{
	if (density_or_cost == 0)
	{
		return ((rand() % 100) / 100.0);  //0, 0.01, 0.02 ... 0.98, 0.99
	}
	return ((rand() % max_cost)/10.0); //0, 0.1 ... 9.9, 10.0
}

class Graph
{
public:
	Graph(int node_size = SIZE, double density = DENSITY) : edges(0), vertices(SIZE), graph(new bool* [node_size]), cost(new double* [node_size]), close(new bool[SIZE]), open(new bool[SIZE]), cost_to(new double[SIZE]) //Constructor.
	{

		cout << "Creating a random graph with " << DENSITY << " density...\t";

		for (int i = 0; i < node_size; i++)
		{
			graph[i] = new bool[node_size];
			cost[i] = new double[node_size];
			cost_to[i] = 999.0; //Before calculating every nodes distance is infinite (999.0 in my program)
		}

		
		for (int i = 0; i < node_size; i++)
		{
			for (int j = 0; j < node_size; j++)
			{
				if (i == j)
				{
					graph[i][j] = false;
				}
				else if (graph[i][j] != true)
				{
					graph[i][j] = graph[j][i] = (prob(0) < density);
					if (graph[i][j] == true) cost[i][j] = cost[j][i] = prob(1);
					edges++; //If this block is executed, one edge is created.
				}
			}
		}
		cout << "Done..." << endl << endl;;
	}
	
	int v() { return vertices; }
	int e() { return edges; }
	int* neighbors(int node);
	double get_edge_value(int node1, int node2);
	bool is_connected();
	void print();
	void shortest_path(int node1);
	void open_close_reset();
	double return_cost_to(int node2)	{ return cost_to[node2]; }
	void average_shortest_path(int node1);
	
private:
	int edges, vertices;
	bool** graph;
	double** cost;
	bool* close;
	bool* open;
	double* cost_to;
};

void Graph::average_shortest_path(int node1)
{
	cost_to[node1] = 0;
	cout << "Calculating the shortest path from " << node1 << " to other nodes...\t";
	open_close_reset(); //Sets every close[x] to false.
	shortest_path(node1);    //Node 1 is starting point.
	cout << "Done..." << endl <<"Calculating average shortest path...\t";
	double sum = 0;
	double decreasement = 1.0;
	for (int i = 0; i < SIZE; i++)
	{
		if (cost_to[i] == 999.0) //If there isn't any path to node i, exclude that node.
		{
			cost_to[i] = 0; decreasement++;
		}
		sum += cost_to[i];
	}
	cout <<"Done..."<<endl<< endl<<"Average shortest path is " << sum / static_cast<double>(SIZE - decreasement) << "." << endl << endl;
}

void Graph::open_close_reset()
{
	for (int i = 0; i < SIZE; i++)
	{
		open[i] = close[i] = false;
	}
	open[0] = true;
}

int* Graph::neighbors(int node)
{
	int* adj = new int[SIZE];
	int index = 1, count = 0; //We will use adj[0] as numbers of neighbors. Placement of nodes will start with index 1. 
	for (int i = 0; i < SIZE; i++)
	{
		if (graph[node][i])
		{
			adj[index] = i;
			index++;
			count++;
		}
	}
	adj[0] = count;
	return adj;
}

double Graph::get_edge_value(int node1, int node2)
{
	if (graph[node1][node2]) //Check if there is an edge or not.
		return cost[node1][node2];
}

void Graph::shortest_path(int node1)
{
	int* neighbors_of_node = neighbors(node1);
	double temp;
	
	for (int i = 1; i <= neighbors_of_node[0];i++)
	{
		temp = cost_to[node1] + cost[node1][neighbors_of_node[i]];
		if(cost_to[neighbors_of_node[i]] > temp) //At start every nodes distance is infinite. If algorithm finds a lower distance it will replace it.
		{
			cost_to[neighbors_of_node[i]] = temp;
		}
	}
	
	close[node1] = true; //Taking the used node to the close set.
	
	for (int i = 1; i <= neighbors_of_node[0];i++)
	{
		if(close[neighbors_of_node[i]] == false)
		{
			shortest_path(neighbors_of_node[i]); //Using algorithm for every neighbor in open set.
		}
	}
}

void Graph::print()
{
	cout << "Current graph has " << edges << " edges and " << vertices << " nodes." << endl << "Graph is " << (is_connected() ? "connected." : "unconnected.") << endl << "Printing graph..." << endl << endl;
	for(int i = 0; i < SIZE;i++)
	{
		for(int j = 0; j < SIZE; j++)
		{
			cout << graph[i][j] << " ";
		}
		cout<< endl;
	}
}

bool Graph::is_connected()
{
	int old_size = 0, c_size = 0; //c_size is closed set size.

	for (int i = 0; i < SIZE; i++)
	{
		open[i] = close[i] = false;
	}
	open[0] = true;
	while(c_size < SIZE) //Nothing is yet in the closed set, and each iteration will add one node to the closed set and possibly many nodes to the open set, or maybe not.
	{
		for (int i = 0; i < SIZE; i++)
		{
			old_size = c_size;
			if (open[i] && (close[i] == false))
			{
				close[i] = true;
				c_size++;
				for (int j = 0; j < SIZE; j++) //Calculating whether node j is in open set or not.
				{
					open[j] = open[j] || graph[i][j]; //graph[i][j] is true when there is a edge from i to j.
				}
			}
		}
		if (c_size == SIZE) return true; //All the nodes are in closed set. So I'm connected.
		if (old_size == c_size) return false; // I can't reach all the nodes, in which case, the open set is exhausted, and the closed set has yet to contain size n nodes, all the nodes in the map. So I'm not connected.
	}
}

int main()
{
	srand(time(0)); //Seed initializer.
	Graph graph; //Creates a default graph.
	graph.print(); //Prints it.
	cout << endl;
	graph.average_shortest_path(0); //Prints average shortest path to the screen.
	return 0;
}