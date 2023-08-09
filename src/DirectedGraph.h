#ifndef DIRECTED_GRAPH_H
#define DIRECTED_GRAPH_H

#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <functional>
#include <algorithm>
#include <utility>

/**
 * A directed edge struct used to connect a tail ---> head in a graph.
 *
 * @brief Directed edge.
 * @param head What the directed edge points to. Head of an arrow.
 * @param tail Where the directed edge points from. Tail of the arrow
 */
template <typename T>
struct Edge
{
  T tail{};
  T head{};
};
/**
 * @brief A directed graph consisting of nodes and directed edges.
 * @note The graph is essentially a wrapper for a map where some key is mapped to two lists containing 
         its direct predecessors and successors.
 */
template <typename T>
class DirectedGraph
{
private:
  /**
   * @brief Node struct in the graph.
   * @param inputs std::vector of all inputs/direct predecessor of the node. 
   * @param consumers std::vector of all consumers/direct successors of the node.
   */
  struct Node
  {
    /// @note Each node contains references to its inputs and consumers trading space for speed.
    std::vector<T> inputs{};   
    std::vector<T> consumers{};
  };
  std::map<T, Node> m_graphMap{};
  /** @brief prints all underlying key-value pairs. */
  void debug_printHashMap(const std::map<T, Node> map)
  {
    std::cout << "Printing map\n";
    for (const auto& [key, value] : map)
      {
	std::cout << key << " - " << "inputs=" << value.inputs.size()
		  << "consumers=" << value.consumers.size() << '\n';
      }
  }
  /**
   * @brief Removes element for node's inputs.
   * @param removeElement Element to be removed
   * @param nodeElement Element key for the node whose inputs removeElement is to be removed from.
   */
  void removeFromInputs(const T& removeElement, const T& nodeElement);
  /**
   * @brief Removes element for node's consumers.
   * @param removeElement Element to be removed
   * @param nodeElement Element key for the node whose consumers removeElement is to be removed from.
   */
  void removeFromConsumers(const T& removeElement, const T& nodeElement);
  void removeFromVector(const T& element, std::vector<T>& v);
public:
  /**
   * @brief Default constructor.
   */
  DirectedGraph<T>() = default;
  /**
   * @brief Add a new node to the graph.
   * @param element The element representing the node.
   * @return true if the element already exists, else false.
   */
  bool addNode(const T& element);
  /**
   * @brief Adds new nodes to the graph.
   * @param elements The elements representing the nodes to be added.
   */
  void addNodes(const std::vector<T>& elements);
  /**
   * @brief Gets the consumers.
   * @param element Element representing node whose conumers we want.
   * @return std::vector reference to the consumer vector.
   */
  const std::vector<T>& getNodeConsumers(const T& element) const
  { return m_graphMap.at(element).consumers; }
  /**
   * @brief Gets the consumers.
   * @param element Element representing node whose conumers we want.
   * @return std::vector reference to the consumer vector.
   */
  const std::vector<T>& getNodeInputs(const T& element) const
  { return m_graphMap.at(element).inputs; }
  /**
   * @brief Adds an edge to the graph.
   * @param edge Edge struct.
   */
  void addEdge(const Edge<T>& edge);
  /**
   * @brief Adds edges to the graph.
   * @param edges std::vector of Edge objects to be added.
   */
  void addEdges(const std::vector<Edge<T>>& edges);
  /**
   * @brief Adds an edge to the graph.
   * @param tail The tail of the edge.
   * @param source The source of the edge.
   */
  void addEdgeElements(const T& tail, const T& head);
  /**
   * @brief Adds two nodes and an edge between them.
   * @param from element representing the tail.
   * @param to   element representing the head.
   */
  void addConnection(const T& from, const T& to);
  /**
   * Prints the element in each node as well as each node's inputs and consumers.
   * @brief Prints the graph.
   */
  void printGraph() const;
  /**
   * Prints the element in each node as well as each node's inputs and consumers.
   * @brief Prints the graph according to some element rule.
   */
  void printGraph(const std::function<void(T)> customPrintFcn) const;
  /**
   * Removes a node and all connecting edges from the graph.
   * @brief Removes node.
   * @param element Element representing the node to be removed.
   */
  void prune(const T& element);
  /**
   * Removes multiple nodes from the graph along with all connecting edges.
   * @brief Remove multiple nodes.
   * @param removedElements std::vector of the elements representing the nodes to be removed.
   */
  void pruneMultiple(const std::vector<T>& removedElements);
  /**
   * Clears the graph of all nodes and edges. Is essentially like making a new graph object.
   * @brief Flushes the object.
   */
  DirectedGraph<T>& flush() {m_graphMap.clear(); return *this;}
  /**
   * @brief Merges the nodes corresponding to the two elements keeping the first element and
   *        rerouting the connections from the second element to the first element.
   * @param Kept element.
   * @param Merged element.
   */
  void mergeElements(const T& keptElement, const T& mergedElement);
  /**
   * @brief Merges two graphs with some common nodes specified as pairs.
   * @param o_graph Other graph.
   * @param pairs Vector of pairs with associated nodes.
   */
  void absorbDisjoint(DirectedGraph<T>& o_graph, const std::vector<std::pair<T, T>>& associations);
  void absorb(DirectedGraph<T>& o_graph);
  bool isEmpty() { return (m_graphMap.size() == 0); }
};

template <typename T>
void DirectedGraph<T>::removeFromInputs(const T& removeElement, const T& nodeElement)
{
  try
    {
      removeFromVector(removeElement, m_graphMap.at(nodeElement).inputs);
    }
  catch(std::out_of_range)
    {
      // Do nothing since it is already not here.
    }
}

template <typename T>
void DirectedGraph<T>::removeFromConsumers(const T& removeElement, const T& nodeElement)
{
  try
    {
      removeFromVector(removeElement, m_graphMap.at(nodeElement).consumers);
    }
  catch(std::out_of_range)
    {
      // Do nothing since it is already not here.
    }
}

template <typename T>
void DirectedGraph<T>::removeFromVector(const T& element, std::vector<T>& v)
{
  const auto it {std::find(v.begin(), v.end(), element)};
  if ( it != v.end() )
    v.erase( it );
}

template <typename T>
void DirectedGraph<T>::printGraph() const
{
  for (const auto& pair : m_graphMap)
    {
      // Iterate over the Node values.
      for (const auto& input : pair.second.inputs)
	{
	  std::cout << input << ' ';
	}
      std::cout << "-- (" << pair.first << ") --> ";
      for (const auto& consumer : pair.second.consumers)
	{
	  std::cout << consumer << ' ';
	}
      std::cout << '\n';
    }
}

template <typename T>
void DirectedGraph<T>::printGraph(const std::function<void(T)> customPrintFcn) const
{
  for (const auto& pair : m_graphMap)
    {
      // Iterate over the Node values.
      for (const auto& input : pair.second.inputs)
	{
	  customPrintFcn(input);
	  std::cout << ' ';
	}
      std::cout << "-- (";
      customPrintFcn(pair.first);
      std::cout << ") --> ";
      for (const auto& consumer : pair.second.consumers)
	{
	  customPrintFcn(consumer);
	  std::cout << ' ';
	}
      std::cout << '\n';
    }
}

template <typename T>
bool DirectedGraph<T>::addNode(const T& element)
{
  if (m_graphMap.count(element) == 1)
    return true;
  m_graphMap[element] = Node{};
  return false;
}

template <typename T>
void DirectedGraph<T>::addNodes(const std::vector<T>& elements)
{
  for (const auto& element : elements)
    {
      addNode(element);
    }  
}

template <typename T>
void DirectedGraph<T>::addEdge(const Edge<T>& edge)
{
  assert(m_graphMap.count(edge.tail) == 1
	 && m_graphMap.count(edge.head) == 1
	 && "Tried to add an edge with non-existant nodes");
  // Add the head as a consumer of the tail...
  m_graphMap[edge.tail].consumers.push_back(edge.head);
  // and add the tail as an input of the head.
  m_graphMap[edge.head].inputs.push_back(edge.tail);
}

template <typename T>
void DirectedGraph<T>::addEdges(const std::vector<Edge<T>>& edges)
{
  for (const auto& edge : edges)
    {
      addEdge(edge);
    }
}

template <typename T>
void DirectedGraph<T>::addEdgeElements(const T& tail, const T& head)
{
  addEdge( Edge<T>{tail, head} );
}

template <typename T>
void DirectedGraph<T>::addConnection(const T& from, const T& to)
{
  addNode(from);
  addNode(to);
  addEdgeElements(from, to);
}

template <typename T>
void DirectedGraph<T>::prune(const T& element)
{
  // Fun fact, when initiating a range based for loop and then removing elements the
  // range is pointing at we will still access the removed element in the loop.
  // Segfault :(
  m_graphMap.erase(element);
  for (const auto& pair : m_graphMap)
    {
      if (pair.first == element)
	{
	  continue;
	}	
      removeFromInputs(element, pair.first);
      removeFromConsumers(element, pair.first);	
    }
}

template <typename T>
void DirectedGraph<T>::pruneMultiple(const std::vector<T>& removedElements)
{
  for (auto element : removedElements)
    prune(element);
}

template <typename T>
void DirectedGraph<T>::mergeElements(const T& keptElement, const T& mergedElement)
{
  /** @invariant For two elements a <=> b their order of apperance in inputs and consumers
   *             must remain the same!
   */
  for (const T& inputOfMerge : m_graphMap.at(mergedElement).inputs)
    {
      // Inside the consumers of each input to the merged elements...
      auto& consumersOfInput{m_graphMap.at(inputOfMerge).consumers};
      // ... replace the merged element with the kept element...
      std::replace(consumersOfInput.begin(), consumersOfInput.end(), mergedElement, keptElement);
      // ... and finally att the input to the kept element.
      m_graphMap.at(keptElement).inputs.push_back(inputOfMerge);
      // Hopefully this will place it in order and operations will work for computational graphs.
    }
  for (const T& consumerOfMerge : m_graphMap.at(mergedElement).consumers)
    {
      auto& inputsOfConsumer{m_graphMap.at(consumerOfMerge).inputs};
      std::replace(inputsOfConsumer.begin(), inputsOfConsumer.end(), mergedElement, keptElement);
      m_graphMap.at(keptElement).consumers.push_back(consumerOfMerge);
    }
  // Finally remove the merged element from the graph.
  m_graphMap.erase(mergedElement);
}

template <typename T>
void DirectedGraph<T>::absorbDisjoint(DirectedGraph<T>& o_graph,
			      const std::vector<std::pair<T, T>>& associations)
{
  m_graphMap.merge(o_graph.m_graphMap);
  assert(o_graph.m_graphMap.size() == 0 &&
	 "Do not support merging of graphs with equal elements using associations.");
  for (const auto& [firstElement, secondElement] : associations)
    {
      mergeElements(firstElement, secondElement);
    }
}

template <typename T>
void DirectedGraph<T>::absorb(DirectedGraph<T>& o_graph)
{
  // A normal merge implies that some elements are equal in the graphs.
  m_graphMap.merge(o_graph.m_graphMap);
  std::cout << "FML\n";
  assert(o_graph.m_graphMap.size() > 0 && "Merging two disjoint graphs.");
  for (auto& pair : o_graph.m_graphMap)
    {
      for (const T& copyInput : pair.second.inputs)
	{
	  m_graphMap.at(pair.first).inputs.push_back(copyInput);
	}
      for (const T& copyConsumer : pair.second.consumers)
	{
	  m_graphMap.at(pair.first).consumers.push_back(copyConsumer);
	}
      // NEVER EVER TOUCH ANYTHING YOU ITERATE OVER WITH AN ITERATOR OR ANYTHIN ELSE OK??!!!!!!
    }
  o_graph.flush();
}

#endif
