
#include "DirectedGraph.h"
#include <cassert>
#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>

#define PRINT 0

int constexpr someNodes[]{2, 4, 6, 8, 10, 100, -10, 42};


static void printLine()
{
  std::cout << "\n------------------------\n";
}

template <typename T>
static bool findIn(const std::vector<T>& vector, T el)
{
  return (std::find(vector.begin(), vector.end(), el) != vector.end()) ? true : false;
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
#if PRINT
  std::cout << "Testing absorb\n";
#endif
  {
    // For these graphs some elements must be equal. Those elements will be merged.
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
    const auto& c3{graph1.getNodeConsumers(3)};
    const auto& i3{graph1.getNodeInputs(3)};

    // All this should hold after merge
    assert(c3.size() == 3 && i3.size() == 2);
    assert( graph1.getNodeInputs(10).size() == 1 );
    assert( graph1.getNodeInputs(5).size() == 1 );
    assert( graph1.getNodeInputs(6).size() == 1 );
    assert( graph1.getNodeConsumers(1).size() == 1 );
    assert( graph1.getNodeConsumers(2).size() == 1 );
    assert( findIn(c3, 10) && findIn(c3, 5) && findIn(c3, 6) );
    assert( findIn(i3, 1) && findIn(i3, 2) );
    assert( findIn(graph1.getNodeInputs(10), 3) );
    assert( findIn(graph1.getNodeInputs(5), 3) );
    assert( findIn(graph1.getNodeInputs(6), 3) );
    assert( findIn(graph1.getNodeConsumers(1), 3) );
    assert( findIn(graph1.getNodeConsumers(2), 3) );
    
#if PRINT
    std::cout << "Graph 1\n-----------\n";
    graph1.printGraph();
    std::cout << "Graph 2\n-----------\n";
    graph2.printGraph();
#endif
  }
  {
    DirectedGraph<int> graph1{};
    graph1.addConnection(0, 1);
    graph1.addConnection(1, 2);
    graph1.addConnection(2, 3);
    graph1.addConnection(3, 4);
  
    DirectedGraph<int> graph2{};
    graph2.addConnection(4, 5);
    graph2.addConnection(5, 6);
    graph2.addConnection(6, 7);
    graph2.addConnection(7, 0);

    graph1.absorb(graph2);
    // After merge we should have a looped linked list. Let's do a cool test for this.
    for (int i{0}, currentElement{0}; i < 100; ++i)
      {
	const auto& consumers{ graph1.getNodeConsumers(currentElement) };
	assert(consumers.size() == 1 && "");
	assert(currentElement == i % 8);
	currentElement = consumers[0];
      }
    
#if PRINT
    std::cout << "Graph 1\n-----------\n";
    graph1.printGraph();
    std::cout << "Graph 2\n-----------\n";
    graph2.printGraph();
#endif
  }
}

void testAssociationAbsorb()
{
#if PRINT
  std::cout << "Testing association absorb, i.e. absorbDisjoint.\n";
#endif
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

    const auto& c3{ graph1.getNodeConsumers(3) };
    const auto& i3{ graph1.getNodeInputs(3) };
    // All this should hold after merge
    assert(c3.size() == 3 && i3.size() == 2);
    assert( graph1.getNodeInputs(10).size() == 1 );
    assert( graph1.getNodeInputs(5).size() == 1 );
    assert( graph1.getNodeInputs(6).size() == 1 );
    assert( graph1.getNodeConsumers(1).size() == 1 );
    assert( graph1.getNodeConsumers(2).size() == 1 );
    assert( findIn(c3, 10) && findIn(c3, 5) && findIn(c3, 6) );
    assert( findIn(i3, 1) && findIn(i3, 2) );
    assert( findIn(graph1.getNodeInputs(10), 3) );
    assert( findIn(graph1.getNodeInputs(5), 3) );
    assert( findIn(graph1.getNodeInputs(6), 3) );
    assert( findIn(graph1.getNodeConsumers(1), 3) );
    assert( findIn(graph1.getNodeConsumers(2), 3) );

#if PRINT
    std::cout << "Graph 1\n-----------\n";
    graph1.printGraph();
    std::cout << "Graph 2\n-----------\n";
    graph2.printGraph();
#endif
  }
  
  {
    DirectedGraph<int> graph1{};
    graph1.addConnection(0, 1);
    graph1.addConnection(1, 2);
    graph1.addConnection(2, 3);
    graph1.addConnection(3, 4);
  
    DirectedGraph<int> graph2{};
    graph2.addConnection(42, 5);
    graph2.addConnection(5, 6);
    graph2.addConnection(6, 7);
    graph2.addConnection(7, 69);
    
    std::vector<std::pair<int, int>> av{ std::make_pair(4, 42), std::make_pair(0, 69) };
    graph1.absorbDisjoint(graph2, av);

    for (int i{0}, currentElement{0}; i < 100; ++i)
      {
	const auto& consumers{ graph1.getNodeConsumers(currentElement) };
	assert(consumers.size() == 1 && "");
	assert(currentElement == i % 8);
	currentElement = consumers[0];
      }

#if PRINT
    std::cout << "Graph 1\n-----------\n";
    graph1.printGraph();
    std::cout << "Graph 2\n-----------\n";
    graph2.printGraph();
#endif
  }
}


int main()
{
  // Here also test the flush function and constructor.
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



