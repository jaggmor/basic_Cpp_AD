
#ifndef FORWARD_PROP_H
#define FORWARD_PROP_H

#include "Variable.h"
#include "DirectedGraph.h"
#include "input_constant.h"
#include <vector>

/**
 * @brief Sets the values of the computational graph. This side effect is the main purpose.
 * @param graph A computational graph representing a function with one output.
 * @param The output variable.
 */
const Variable& forwardProp(DirectedGraph<Variable*>& graph, Variable& output);

/**
 * @note This method only works for the unit as long as we have ONE output and as long
 *       as the Directed graph is acyclical, which must be the case in a computational 
 *       graph since no computation can purely be a function of its own result.
 */
static void visit(DirectedGraph<Variable*>& graph, Variable& currentVar);

#endif
