
#include "forwardProp.h"

const Variable& forwardProp(DirectedGraph<Variable*>& graph, Variable& output) {
  assert(output.getConsumers(graph).size() == 0 && "Output for a unit cannot have consumers.");
  visit(graph, output);
  return output;
}

static void visit(DirectedGraph<Variable*>& graph, Variable& currentVar) {      
  if (currentVar.getOperation() == input) return; // Early return at leaf.
  assert(currentVar.getInputs(graph).size() == 1 || currentVar.getInputs(graph).size() == 2);
  for (Variable* input : currentVar.getInputs(graph)) {
    visit(graph, *input);
  }
  // When all inputs are visited the current node is updated.
  // In the future the updating can be made into a recursive function call for multithreading.
  if (currentVar.getOperation().isUnary()) {
    const Variable& input{ *currentVar.getInputs(graph).at(0) };
    currentVar.getOperation().uop(input, currentVar);
  } else if (currentVar.getOperation().isBinary()) {
    const Variable& linput{ *currentVar.getInputs(graph).at(0) };
    const Variable& rinput{ *currentVar.getInputs(graph).at(1) };
    currentVar.getOperation().bop(linput, rinput, currentVar);
  }
  return;
}

