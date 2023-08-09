
#ifndef BACK_PROP_H
#define BACK_PROP_H

#include "Variable.h"
#include "DirectedGraph.h"
#include <unordered_map>
#include <vector>
#include <memory>


using uvptr = std::unique_ptr<Variable>;
using Gradient = std::vector<double>;

template <typename V, typename K>
using map = std::unordered_map<V, K>;

/**
 * @brief        Functions that computes the gradient for all Variables in graph using my method
 *               which is a simple graph walk. 
 *
 *               This will work under the assumption that we want to compute all gradients 
 *               and that there is only one output in the computational graph.
 * @param graph  The computational graph.
 * @param output The scalar output of the computational graph.
 * @return       A map of Variable* -> Gradient. 
 */
map<Variable*, Gradient> backProp_walk(DirectedGraph<Variable*>& graph, Variable& output);

static void walk_gradient(Variable& var, DirectedGraph<Variable*>& graph,
		   map<Variable*, Gradient>& grad_table, map<Variable*, int>& visits);
void printGradTable(const map<Variable*, Gradient>& grad_table);

#endif
