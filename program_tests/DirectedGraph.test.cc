

#include "DirectedGraph.h"
#include <cassert>
#include <vector>
#include <iostream>


int constexpr someNodes[]{2, 4, 6, 8, 10, 100, -10, 42};


void printLine()
{
  std::cout << "\n------------------------\n";
}


void createPrintEmptyIntGraph(DirectedGraph<int>& graph)
{
  printLine();

  // Test the empty graph
  for (auto node : someNodes)
    {
      graph.addNode(node);
    }
  graph.printGraph();
}


void createPrintLinkedListIntGraph(DirectedGraph<int>& graph)
{
  printLine();
  constexpr int size{ sizeof(someNodes)/sizeof(int) };

  // Add the nodes
  for (auto node : someNodes)
    {
      graph.addNode(node);
    }

  // Add the edges
  for (int i{0}; i < size; ++i)
    {
      int node {someNodes[i]};
      int nextNode {someNodes[(i+1)%size]};
      // Also testing the edge class.
      graph.addEdge(Edge<int>{nextNode, node});
    }
  graph.printGraph();
}


void createPrintAllToAllIntGraph(DirectedGraph<int>& graph)
{
  printLine();
  constexpr int size{ sizeof(someNodes)/sizeof(int) };

  // Add the nodes
  for (auto node : someNodes)
    {
      graph.addNode(node);
    }

  // Add the edges
  for (auto source : someNodes)
    {
      for (auto head : someNodes)
	{
	  graph.addEdge(Edge<int>{head, source});
	}
    }
  
  graph.printGraph();
}


void testPruning()
{
  printLine();
  DirectedGraph<int> graph{};
  createPrintAllToAllIntGraph(graph);

  // Prune away all the non-fitting numbers.
  std::vector<int> vp {100, -10, 42};
  // No need to test single pruning since multiple uses single pruning.
  graph.pruneMultiple(vp);

  std::cout << "Graph after pruning is now:\n";
  graph.printGraph();    
  
}


void testAddEdges()
{
  DirectedGraph<int> graph{};
  constexpr int nodes[] = {1, 2, 3};
  for (auto node : nodes)
    graph.addNode(node);

  std::vector<Edge<int>> edges {};
  edges.push_back(Edge<int>{1, 2});
  edges.push_back(Edge<int>{1, 3});
  edges.push_back(Edge<int>{2, 3});

  graph.addEdges(edges);

  graph.printGraph();
}


void testGetConsumersInputs()
{
  DirectedGraph<int> graph{};
  constexpr int nodes[] = {1, 2, 3};
  for (auto node : nodes)
    graph.addNode(node);

  std::vector<Edge<int>> edges {};
  edges.push_back(Edge<int>{1, 2});
  edges.push_back(Edge<int>{1, 3});
  edges.push_back(Edge<int>{2, 3});

  graph.addEdges(edges);

  assert(graph.getNodeInputs(3).size() == 2);
  assert(graph.getNodeInputs(3)[0] == 1);
  assert(graph.getNodeInputs(3)[1] == 2);
  assert(graph.getNodeConsumers(1).size() == 2);
  assert(graph.getNodeConsumers(1)[0] == 2);
  assert(graph.getNodeConsumers(1)[1] == 3);
}


int main()
{
  // Here we also test the flush function.
  [[maybe_unused]] DirectedGraph<int> graph{};
  createPrintEmptyIntGraph(graph);
  createPrintLinkedListIntGraph(graph.flush());
  createPrintAllToAllIntGraph(graph.flush());
  testPruning();
  testAddEdges();
  testGetConsumersInputs();
  
  return 0;
}



