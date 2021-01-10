//mst.cpp, Mert Kahraman, November 16, 2020
//Reads a graph from a file and calculates its minimum spanning tree.

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
using namespace std;

class Graph
{
public:
	Graph(string fileName){       //Constructor that can read in a graph from a file
		ifstream file;
		file.open(fileName);
		if (!file) {
			cerr << "Unable to open file data.txt";
			exit(1);   // call system to stop
		}
		int size;
		file >> size;
		
		vertices = size;
		cost = new int* [size];
		graph = new bool* [size];
		close = new bool[size];
		edges = 0;

		for (int i = 0; i < size; i++)
		{
			graph[i] = new bool[size];
			cost[i] = new int[size];
			close[i] = false;
		}

		for (int i = 0; i < size; i++)
		{
			for(int j =0; j < size; j++)
			{
				graph[i][j] = false;
			}
		}
		
		int dump, dump2, dump3;
		
		while (file >> dump)
		{
			file >> dump2;
			file >> dump3;
			graph[dump][dump2] = true;
			cost[dump][dump2] = dump3;
			if(!graph[dump2][dump])
			{
				edges++;
			}
		}

		sortedEdges = new int* [edges + 1];
		mstEdges = new int* [vertices - 1];
		
		for (int i = 0; i < edges; i++)
		{
			sortedEdges[i] = new int[3];
		}
		for (int i = 0; i < vertices-1; i++)
		{
			mstEdges[i] = new int[3];
		}
		
		int counter=0;

		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if(j>i&&graph[i][j])
				{
					sortedEdges[counter][0] = i;
					sortedEdges[counter][1] = j;
					sortedEdges[counter][2] = cost[i][j];
					counter++;
				}
			}
		}
		
		for(int i = 0; i < edges-1; i++)
		{
			for(int j = 0; j < edges-1; j++){
				if(sortedEdges[j][2] > sortedEdges[j+1][2])
				{
					sortedEdges[edges] = sortedEdges[j];
					sortedEdges[j] = sortedEdges[j + 1];
					sortedEdges[j + 1] = sortedEdges[edges];
				}
			}
		}
		cycleVisited = new bool[vertices];
		for(int i = 0; i < vertices;i++)
		{
			cycleVisited[i] = false;
		}
		
	}
	
	int* neighbors(int node);               //Standart get neighbor array function
	void print();                           //Prints graph
	int getVertices() { return vertices; }  
	int getEdges() { return edges; }
	int getEdgeCost(int node1, int node2);
	void minimumSpanningTree();            //Calculates mst
	bool cycle(int node, int i = 0);       //Checks cycles
	void cycleReset() { for (int i = 0; i < vertices;i++) cycleVisited[i] = false;} //Resets cycleVisited
	int* mstNeighbors(int node);          //Mst version of neighbor array function
	
private:
	int edges, vertices;
	bool** graph;
	int** cost;
	bool* close;
	int** sortedEdges;
	int** mstEdges;
	bool* cycleVisited;
	int cache;
};

int main()
{
	Graph graph("data.txt");
	graph.print();
	graph.minimumSpanningTree();
    return 0;
}

int* Graph::neighbors(int node)
{
	int* adj = new int[vertices];
	int index = 1, count = 0; //We will use adj[0] as numbers of neighbors. Placement of nodes will start with index 1. 
	for (int i = 0; i < vertices; i++)
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

void Graph::print()
{
	cout << "Current graph has " << edges << " edges and " << vertices << " nodes." << endl << "Printing graph..." << endl << endl;
	for (int i = 0; i < vertices;i++)
	{
		for (int j = 0; j < vertices; j++)
		{
			cout << graph[i][j] << " ";
		}
		cout << endl;
	}
}

int Graph::getEdgeCost(int node1, int node2)
{
	if (graph[node1][node2]) //Check if there is an edge or not.
		return cost[node1][node2];
}

void Graph::minimumSpanningTree()
{
	cout << "Calculating minimum spanning tree..."<< endl;

	cout << "Sorted edges: " << endl;
	cout << "index " << "cost " << "src " << "dest" << endl;
	for (int i = 0; i < edges; i++)
	{
		cout << setw(5) << i << setw(5) << sortedEdges[i][2] << setw(4) << sortedEdges[i][0] << setw(5) << sortedEdges[i][1] << endl;
	}
	
	int counter =0, weight = 0;
	bool* visited = new bool[vertices];
	for (int i = 0; i < vertices;i++)
		visited[i] = false;
	for(int i = 0; i < edges; i++)
	{
		if(!visited[sortedEdges[i][0]] || !visited[sortedEdges[i][1]]) //If none or one of the nodes has been visitied, algorithm doesn't need to check if it creates a loop or not.
		{
			mstEdges[counter][0] = sortedEdges[i][0];
			mstEdges[counter][1] = sortedEdges[i][1];
			mstEdges[counter][2] = sortedEdges[i][2];
			visited[sortedEdges[i][0]] = true;
			visited[sortedEdges[i][1]] = true;
			counter++;
			weight += sortedEdges[i][2];
		}else if (visited[sortedEdges[i][0]] && visited[sortedEdges[i][1]]) //If both are visited algorith needs to check if there is a cycle or not.
		{
			mstEdges[counter][0] = sortedEdges[i][0];  //We need to assign these new values to check if they create a cycle.
			mstEdges[counter][1] = sortedEdges[i][1];
			mstEdges[counter][2] = sortedEdges[i][2];
			if(cycle(sortedEdges[i][0]))   //Cycle function will return true if there is a cycle and this if statement delete this assignments.
			{
				//Debug cout << endl << endl << "DELETING mstedges[" << counter << "]." << endl << endl;
				mstEdges[counter][0] = NULL;
				mstEdges[counter][1] = NULL;
				mstEdges[counter][2] = NULL;
			}
			else                         //If cycle function returns false then we can proceed with other values, no deletion required. 
			{
				//DEBUG cout << endl << endl << "THIS mstedges[" << counter << "] maid without a cycle." << endl << endl;
				visited[sortedEdges[i][0]] = true;
				visited[sortedEdges[i][1]] = true;
				counter++;
				weight += sortedEdges[i][2];
			}
		}
		if(counter ==vertices-1) //There will be vertices -1 edges in mst so we don't need no more if we reach this limit.
		{
			break;
		}
	}

	cout << "Minimum spanning tree edges: " << endl;
	cout << "index " << "cost " << "src " << "dest" << endl;
	for (int i = 0; i < 16; i++)
	{
		cout << setw(5) << i << setw(5) << mstEdges[i][2] << setw(4) << mstEdges[i][0] << setw(5) << mstEdges[i][1] << endl;
	}
	cout << "Minimum spanning tree's cost is " << weight;
}


bool Graph::cycle(int node, int i)  //recursive
{
	if(i == 0){   //To check if this cycle function called firstly. We need to keep node end reset visitedCycles.
		//DEBUG cout << "Function cycle "<<"Executed if i == 0 for node " << node<<endl;
		cache = node;
		cycleReset();
	}
	//DEBUG cout << "Function cycle " << node << endl;
	int* neighbors = mstNeighbors(node);
	cycleVisited[node] = true;
	for(int i = 1; i <= neighbors[0]; i++)
	{
		if(i > 1 && neighbors[i] == cache)   //Edges are undirected so we need to check if it sees starting node as a neighbor after 2 recursive.
		{
			/*DEBUG cout << " sending " << i << " indexed neighbor " << neighbors[i] << " node is " << node << " btw." << endl;
			cout << endl << "Function cycle " << node <<" true" << " cache was " << cache<<endl;*/
			return true;
		}
		//DEBUG cout << " checking " << i << " indexed neighbor " << neighbors[i] << " node is " << node << " btw." << endl;
		if(!cycleVisited[neighbors[i]]) //If it is not visited we can proceed recursively, checking each neighbor one by one.
		{
			//DEBUG cout << " sending " << i << " indexed neighbor " << neighbors[i] << " node is "<<node<< " btw."<<endl;
			cycle(neighbors[i], i+1);
		}
	}
}

int* Graph::mstNeighbors(int node)
{
	int* adj = new int[100];
	int index = 1, count = 0; //We will use adj[0] as numbers of neighbors. Placement of nodes will start with index 1. 
	for (int i = 0; i < vertices - 1; i++)
	{
		if (mstEdges[i][0] == node)
		{
			//DEBUG cout << "Function mstNeigh " << "source is "<< mstEdges[i][0] <<" dest is " <<mstEdges[i][1]<<endl;
			adj[index] = mstEdges[i][1];
			index++;
			count++;
		}else if(mstEdges[i][1] == node)
		{
			//DEBUG cout << "Function mstNeigh " << "source is " << mstEdges[i][1] << " dest is " << mstEdges[i][0]<<endl;
			adj[index] = mstEdges[i][0];
			index++;
			count++;
		}
	}
	adj[0] = count;
	return adj;
}
