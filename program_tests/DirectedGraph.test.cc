

#include "DirectedGraph.h"
#include <cassert>
#include <vector>
#include <iostream>
#include <utility>


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

void testAbsorbingGraphs()
{
  std::cout << "Testing with normal graph\n";
  {
    DirectedGraph<int> graph1{};
    graph1.addConnection(1, 3);
    graph1.addConnection(2, 3);
    graph1.addConnection(3, 10);
    graph1.addConnection(10, 11);
  
    DirectedGraph<int> graph2{};
    graph2.addConnection(3, 5);
    graph2.addConnection(3, 6);
    graph2.addConnection(5, 7);
    graph2.addConnection(6, 7);
    graph1.absorb(graph2);
    std::cout << "Graph 1\n-----------\n";
    graph1.printGraph();
    std::cout << "Graph 2\n-----------\n";
    graph2.printGraph();
  }

  std::cout << "Now testing with a linked graph\n";
  {
    DirectedGraph<int> graph1{};
    graph1.addConnection(1, 2);
    graph1.addConnection(2, 3);
    graph1.addConnection(3, 4);
  
    DirectedGraph<int> graph2{};
    graph2.addConnection(4, 5);
    graph2.addConnection(5, 6);
    graph2.addConnection(6, 7);
    graph2.addConnection(7, 1);
    graph1.absorb(graph2);
    std::cout << "Graph 1\n-----------\n";
    graph1.printGraph();
    std::cout << "Graph 2\n-----------\n";
    graph2.printGraph();
  }
}

void testAssociationAbsorb()
{
  std::cout << "Testing association absorb with normal graph\n";
  {
    DirectedGraph<int> graph1{};
    graph1.addConnection(1, 3);
    graph1.addConnection(2, 3);
    graph1.addConnection(3, 10);
    graph1.addConnection(10, 11);
  
    DirectedGraph<int> graph2{};
    graph2.addConnection(4, 5);
    graph2.addConnection(4, 6);
    graph2.addConnection(5, 7);
    graph2.addConnection(6, 7);

    // We make the association 3 <=> 4
    std::vector<std::pair<int, int>> av{ std::make_pair(3, 4) };
    
    graph1.absorbDisjoint(graph2, av);
    std::cout << "Graph 1\n-----------\n";
    graph1.printGraph();
    std::cout << "Graph 2\n-----------\n";
    graph2.printGraph();
  }

  std::cout << "Now testing with a linked graph and association \n";
  {
    DirectedGraph<int> graph1{};
    graph1.addConnection(0, 1);
    graph1.addConnection(1, 2);
    graph1.addConnection(2, 3);
  
    DirectedGraph<int> graph2{};
    graph2.addConnection(4, 5);
    graph2.addConnection(5, 6);
    graph2.addConnection(6, 7);
    graph2.addConnection(7, 8);

    std::vector<std::pair<int, int>> av{ std::make_pair(0, 8), std::make_pair(3, 4) };
    
    graph1.absorbDisjoint(graph2, av);
    std::cout << "Graph 1\n-----------\n";
    graph1.printGraph();
    std::cout << "Graph 2\n-----------\n";
    graph2.printGraph();
  }
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
  testAbsorbingGraphs();
  testAssociationAbsorb();
  return 0;
}



