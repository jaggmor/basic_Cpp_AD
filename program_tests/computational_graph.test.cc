
#include "operations.h"
#include "DirectedGraph.h"
#include "Scalar.h"
#include "OperationUnary.h"
#include "OperationBinary.h"
#include "Operation.h"

#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <unordered_map>

using v_ptr = std::unique_ptr<Variable>;
using Gradient = std::vector<double>;

template <typename V, typename K>
using map = std::unordered_map<V, K>;


const Input input{};
const ScalarAdd scalarAdd{};


void buildAddGraph()
{
  // This vector is an illustration of a container that could be used to have ownership of all
  // created variables.
  std::vector<v_ptr> vars {};
  
  DirectedGraph<Variable*> graph{};

  // Simple lambda function that can be used to put away the variables.
  auto put_away{ [&vars] (auto uptr) { vars.push_back(std::move(uptr)); } };
  
  auto x{ std::make_unique<Scalar>(input, 2.0) };
  auto y{ std::make_unique<Scalar>(input, 3.0) };

  auto a{ scalarAdd(*x, *y, graph ) };
  put_away(std::move(x)); put_away(std::move(y));
  
  auto b{ std::make_unique<Scalar>(input, 4.0) };
  auto c{ scalarAdd(*a, *b, graph) };
  put_away(std::move(a)); put_away(std::move(b)); put_away(std::move(c));
 
  auto customPrint{ [] (Variable* varptr) -> void
  {
    std::cout << *varptr;
  }};
  
  graph.printGraph(customPrint);
}


void buildAddCyclicalGraph()
{  
  DirectedGraph<Variable*> graph{};
  
  auto x{ std::make_unique<Scalar>(input, 2.0) };
  auto y{ std::make_unique<Scalar>(input, 3.0) };

  auto a{ scalarAdd(*x, *y, graph ) };
  
  auto b{ scalarAdd(*a, *x, graph) };
  auto c{ scalarAdd(*b, *x, graph) };

  auto customPrint{ [] (Variable* varptr) -> void
  {
    std::cout << *varptr;
  }};

  assert(x->getConsumers(graph).size() == 3 && "X should be parent to three nodes.");
  
  graph.printGraph(customPrint);
}

static void updateVarUnaryOp(const std::vector<Variable*>& inputs, Variable& var)
{
  const Operation& operation{ var.getOperation() };
  operation.uop(*inputs.at(0), var);
  var.setTrue();  
}

static void updateVarBinaryOp(const std::vector<Variable*>& inputs, Variable& var)
{
  const Operation& operation{ var.getOperation() };
  operation.bop(*inputs.at(0), *inputs.at(1), var);
  var.setTrue();  
}

/**
 * @brief Updates all variables in the graph with inputs witl m_flag == true.
 * @param graph The graphs the updates are with respect to.
 * @param variables Vector of the variables in the graph which should be updated.
 * @return The number of variables that were updated.
 */
static int update_vars_in_graph(DirectedGraph<Variable*>& graph, std::vector<v_ptr>& variables)
{
  int variables_updated{ 0 };

  for (auto& variable : variables)
    {
      if (*variable)   // Variable is already true
	continue;

      // Load the vector of the inputs from the graph.
      const std::vector<Variable*>& inputs{ variable->getInputs(graph) };      
      if (variable->getOperation().isUnary())
	{	  
	  assert(inputs.size() == 1 && "For unary operator size should be 1");
	  updateVarUnaryOp(inputs, *variable);
	  ++variables_updated;
	}
      else if (variable->getOperation().isBinary())
	{
	  assert(inputs.size() == 2 && "For binary operator size should be 2");
	  updateVarBinaryOp(inputs, *variable);
	  ++variables_updated;	  
	}
      else
	{
	  throw std::runtime_error("Unexpected error occurred!");
	}      
    }
  return variables_updated;
}

void buildAddGraphAndEvaluateIt()
{
  // Here I am just messing around a little with the capabilities so far.
  // Should probably be rewritten into a more proper test.
  
  DirectedGraph<Variable*> graph{};

  auto x{ std::make_unique<Scalar>(input, 2.0) };
  auto a{ std::make_unique<Scalar>(input, 3.0) };

  auto res1{ scalarAdd(*x, *a, graph ) };
  
  auto b{ std::make_unique<Scalar>(input, 4.0) };
  auto res2{ scalarAdd(*b, *res1, graph) };

  auto res3{ scalarAdd(*res2, *x, graph)};

  auto y{ scalarAdd(*res3, *x, graph) };

  // The lambda function which really should be made into a normal function.
  auto customPrint{ [] (Variable* varptr) -> void
  {
    std::cout << *varptr;
  }};

  graph.printGraph(customPrint);
  
  a->setValue(1.0);
  b->setValue(2.0);

  std::vector<v_ptr> params{};  
  params.push_back(std::move(a));
  params.push_back(std::move(b));

  std::vector<v_ptr> inter{};
  inter.push_back(std::move(res1));
  inter.push_back(std::move(res2));
  inter.push_back(std::move(res3));
  inter.push_back(std::move(y));

  // Let's now try to evaluate the new graph.

  x->setTrue();
  for (auto& param : params)
    param->setTrue(); // params a,b have been updated.

  for (auto& ivar : inter)
    ivar->setFalse(); // the res_ should all be false.

  
  // Loop until all inner vars are updated
  int number_updated_var{ 0 };
  int number_vars_to_update{ static_cast<int>(inter.size()) };

  while (number_updated_var < number_vars_to_update)
    {
      number_updated_var += update_vars_in_graph(graph, inter);
    }
  std::cout << "\nUpdated Graph after a=1.0 and b=2.0. \n";
  graph.printGraph(customPrint);
}

#if 0
Gradient bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
	       Gradient gradient) const override
{
  // The gradient of a + b = y w.r.t. a or b is always 1.0.
  return Gradient{ 1.0 * gradient[0] };
  }
#endif

void walk_gradient(Variable& var, DirectedGraph<Variable*>& graph,
		   map<Variable*, Gradient>& grad_table, map<Variable*, int>& visits)
{
  // We suppose that var has its final gradient computed. This is a kind of invariant for this
  // method. This immediatly assures us that we have access to the following.
  auto& gradient{ grad_table.at(&var) };
  const auto& inputs{ var.getInputs(graph) };

  auto updateGradient{ [] (Gradient& old_gradient, Gradient& new_gradient)
  {
    assert(old_gradient.size() == new_gradient.size() && "Gradient size must match.");      
    for (int i{0}; i < old_gradient.size(); ++i)
      old_gradient[i] += new_gradient[i];
  }};
  
  for (auto inputVar_ptr : inputs)
    {
      // For the variable pointed to by inputVar_ptr we woult like to compute its (or part of
      // its) gradient.
      auto& operation{ var.getOperation() };   // Lol don't forget to get a reference!!!!!
      Gradient var_gradient{ operation.bprop(inputs, *inputVar_ptr, gradient) };

      if (inputVar_ptr->getConsumers(graph).size() == 1)
	{
	  // This means that this input only has one consumer which must be var.
	  // Thus we can store the gradient as a new variable and call the function again.
	  grad_table[inputVar_ptr] = std::move(var_gradient);
	  walk_gradient(*inputVar_ptr, graph, grad_table, visits);
	}
      else if (inputVar_ptr->getConsumers(graph).size() > 1
	       && visits.find(inputVar_ptr) == visits.end())
	{
	  // This means that the input has more than one consumer and must therefore have its
	  // gradient added to it multiple times.	  
	  // This is also the first time this element is accessed
	  // We therefore add its entry to the grad table:
	  grad_table[inputVar_ptr] = std::move(var_gradient);

	  // We have one visit.
	  visits[inputVar_ptr] = 1;
	  
	  // We don't want to keep going since this variable needs more gradient calculations.
	}
      else if (inputVar_ptr->getConsumers(graph).size() > 1
	       && visits.at(inputVar_ptr)+1 < inputVar_ptr->getConsumers(graph).size())
	{
	  // Here we have already visited the variable once but this is not the last visit.

	  updateGradient(grad_table[inputVar_ptr], var_gradient);
	  visits[inputVar_ptr] += 1;
	}
      else if (inputVar_ptr->getConsumers(graph).size() > 1
	       && visits.at(inputVar_ptr)+1 == inputVar_ptr->getConsumers(graph).size())
	{
	  // We are now at the last visit of the walker...
	  updateGradient(grad_table[inputVar_ptr], var_gradient);
	  // ...so no need to keep the element around since gradient is computed..
	  visits.erase(inputVar_ptr);
	  
	  // Now we can keep going.
	  walk_gradient(*inputVar_ptr, graph, grad_table, visits);
	}
      else
	{
	  throw BadWalkException("An unexpected condition occurred in the graph");
	}
	      
    }
  return;
}

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
map<Variable*, Gradient> backProp_walk(DirectedGraph<Variable*>& graph, Scalar& output)
{
  // Put the gradient of the output with itself.
  map<Variable*, Gradient> grad_table{};

  // Set the output gradient to 1.0 Scalar.
  Gradient initial_gradient{1.0};
  grad_table[&output] = initial_gradient;
  
  map<Variable*, int>      visits{};

  walk_gradient(output, graph, grad_table, visits);

  return grad_table;  
}

void printGradTable(const map<Variable*, Gradient>& grad_table)
{
  for (const auto& kv_pair : grad_table)
    {
      std::cout << *kv_pair.first << '\n';
      for (auto val : kv_pair.second)
	std::cout << val << ' '; 
      std::cout << '\n';
    }
  
}

void buildAddGraphAndBackPropIt()
{
  // Here I am just messing around a little with the capabilities so far.
  // Should probably be rewritten into a more proper test.
  
  DirectedGraph<Variable*> graph{};

  auto x{ std::make_unique<Scalar>(input, 2.0) };
  auto a{ std::make_unique<Scalar>(input, 3.0) };

  auto res1{ scalarAdd(*x, *a, graph ) };
  
  auto b{ std::make_unique<Scalar>(input, 4.0) };
  auto res2{ scalarAdd(*b, *res1, graph) };

  auto res3{ scalarAdd(*res2, *x, graph)};

  auto y{ scalarAdd(*res3, *x, graph) };

  // The lambda function which really should be made into a normal function.
  auto customPrint{ [] (Variable* varptr) -> void
  {
    std::cout << *varptr;
  }};

  graph.printGraph(customPrint);
  std::cout << "Backpropagation starting: " << std::endl;
  auto grad_table{ backProp_walk(graph, *y) };
  printGradTable(grad_table); 
}


int main()
{
  auto nl{ [] () {std::cout << '\n'; } };

  buildAddGraph();
  nl();
  buildAddCyclicalGraph();
  nl();
  buildAddGraphAndEvaluateIt();
  nl();
  buildAddGraphAndBackPropIt();
  nl();
  buildAddGraphAndBackPropIt();
  nl();
  return 0;
}
