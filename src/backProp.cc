
#include "backProp.h"

map<Variable*, Gradient> backProp_walk(DirectedGraph<Variable*>& graph, Variable& output)
{
  // Put the gradient of the output with itself.
  map<Variable*, Gradient> grad_table{};
  // Set the output gradient to 1.0 Scalar.
  Gradient initial_gradient{1.0};
  grad_table[&output] = initial_gradient;
  map<Variable*, int> visits{};
  walk_gradient(output, graph, grad_table, visits);
  return grad_table;  
}

static void walk_gradient(Variable& var, DirectedGraph<Variable*>& graph,
		   map<Variable*, Gradient>& grad_table, map<Variable*, int>& visits) {
  // We suppose that var has its final gradient computed. This is a kind of invariant for this
  // method. This immediatly assures us that we have access to the following.
  auto& gradient{ grad_table.at(&var) };
  const auto& inputs{ var.getInputs(graph) };
  auto updateGradient{ [] (Gradient& old_gradient, Gradient& new_gradient) {
    assert(old_gradient.size() == new_gradient.size() && "Gradient size must match.");      
    for (size_t i{0}; i < old_gradient.size(); ++i) {
      old_gradient[i] += new_gradient[i];
    }
  }};
  for (auto inputVar_ptr : inputs) {
    // For the variable pointed to by inputVar_ptr we woult like to compute its (or part of
    // its) gradient.
    auto& operation{ var.getOperation() };   // Lol don't forget to get a reference!!!!!
    Gradient var_gradient{ operation.bprop(inputs, *inputVar_ptr, gradient) };
    if (inputVar_ptr->getConsumers(graph).size() == 1) {
      // This means that this input only has one consumer which must be var.
      // Thus we can store the gradient as a new variable and call the function again.
      grad_table[inputVar_ptr] = std::move(var_gradient);
      walk_gradient(*inputVar_ptr, graph, grad_table, visits);
    } else if (inputVar_ptr->getConsumers(graph).size() > 1
	       && visits.find(inputVar_ptr) == visits.end()) {
      // This means that the input has more than one consumer and must therefore have its
      // gradient added to it multiple times.	  
      // This is also the first time this element is accessed
      // We therefore add its entry to the grad table:
      grad_table[inputVar_ptr] = std::move(var_gradient);
      // We have one visit.
      visits[inputVar_ptr] = 1;	  
      // We don't want to keep going since this variable needs more gradient calculations.
    } else if (inputVar_ptr->getConsumers(graph).size() > 1
	       && visits.at(inputVar_ptr)+1
	       < static_cast<int>(inputVar_ptr->getConsumers(graph).size()) ) {
      // Here we have already visited the variable once but this is not the last visit.
      updateGradient(grad_table[inputVar_ptr], var_gradient);
      visits[inputVar_ptr] += 1;
    } else if (inputVar_ptr->getConsumers(graph).size() > 1
	       && visits.at(inputVar_ptr)+1 ==
	       static_cast<int>(inputVar_ptr->getConsumers(graph).size()) ) {
      // We are now at the last visit of the walker...
      updateGradient(grad_table[inputVar_ptr], var_gradient);
      // ...so no need to keep the element around since gradient is computed..
      visits.erase(inputVar_ptr);
      // Now we can keep going.
      walk_gradient(*inputVar_ptr, graph, grad_table, visits);
    } else {
      throw BadWalkException("An unexpected condition occurred in the graph");
    }     
  }
  return;
}

void printGradTable(const map<Variable*, Gradient>& grad_table) {
  for (const auto& kv_pair : grad_table) {
    std::cout << *kv_pair.first << '\n';
    for (auto val : kv_pair.second) {
      std::cout << val << ' ';
    }
    std::cout << '\n';
  }  
}


