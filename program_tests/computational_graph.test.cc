
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
#include <stdexcept>

using v_ptr = std::unique_ptr<Variable>;


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


int main()
{
  auto nl{ [] () {std::cout << '\n'; } };

  buildAddGraph();
  nl();
  buildAddCyclicalGraph();
  nl();
  buildAddGraphAndEvaluateIt();
  nl();
  return 0;
}
