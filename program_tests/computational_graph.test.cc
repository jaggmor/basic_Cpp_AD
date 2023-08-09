#include "DirectedGraph.h"
#include "Scalar.h"
#include "OperationUnary.h"
#include "OperationBinary.h"
#include "Operation.h"
#include "operation_constants.h"

#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <unordered_map>

using v_ptr = std::unique_ptr<Variable>;
using Gradient = std::vector<double>;

template <typename V, typename K>
using map = std::unordered_map<V, K>;

/**
 * @note This method only works for the unit as long as we have ONE output and as long
 *       as the Directed graph is acyclical, which must be the case in a computational 
 *       graph since no computation can purely be a function of its own result.
 */
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

void forwardProp(DirectedGraph<Variable*>& graph, Variable& output) {
  assert(output.getConsumers(graph).size() == 0 && "Output for a unit cannot have consumers.");
  visit(graph, output);
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

  auto res1{ scalarAdd(graph, *x, *a) };
  
  auto b{ std::make_unique<Scalar>(input, 4.0) };
  auto res2{ scalarAdd(graph, *b, *res1) };

  auto res3{ scalarAdd(graph, *res2, *x)};

  auto y{ scalarAdd(graph, *res3, *x) };

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

  auto x{ std::make_unique<Scalar>("x", input, 2.0) };
  auto a{ std::make_unique<Scalar>("a", input, 3.0) };

  auto res1{ scalarAdd(graph, *x, *a) };
  
  auto b{ std::make_unique<Scalar>("b", input, 4.0) };
  auto res2{ scalarAdd(graph, *b, *res1) };

  auto res3{ scalarAdd(graph, *res2, *x)};

  auto y{ scalarAdd(graph, *res3, *x) };

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


void buildSubGraphEvaluateAndBackpropIt()
{
  // Here I am just messing around a little with the capabilities so far.
  // Should probably be rewritten into a more proper test.
  
  DirectedGraph<Variable*> graph{};

  auto x{ std::make_unique<Scalar>("x", input, 2.0) };
  auto a{ std::make_unique<Scalar>("a", input, 3.0) };

  auto res1{ scalarSub(graph, *x, *a) };
  
  auto b{ std::make_unique<Scalar>("b", input, 4.0) };
  auto res2{ scalarSub(graph, *b, *res1) };

  auto res3{ scalarSub(graph, *res2, *x)};

  auto y{ scalarSub(graph, *res3, *x) };

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

  std::cout << "Backpropagation starting: " << std::endl;
  auto grad_table{ backProp_walk(graph, *inter.at(3)) };
  printGradTable(grad_table); 
}


void buildPolynomialGraph()
{
  DirectedGraph<Variable*> graph{};

  auto x{ std::make_unique<Scalar>("x", input, 2.0) };

  auto res1{ scalarMul(graph, *x, *x) };
  
  auto res2{ scalarMul(graph, *x, *res1) };

  auto res3{ scalarMul(graph, *res2, *x)};

  auto y{ scalarMul(graph, *res3, *x) };

  // The lambda function which really should be made into a normal function.
  auto customPrint{ [] (Variable* varptr) -> void
  {
    std::cout << *varptr;
  }};

  graph.printGraph(customPrint);

  x->setValue(3.0);

  std::vector<v_ptr> inter{};
  inter.push_back(std::move(res1));
  inter.push_back(std::move(res2));
  inter.push_back(std::move(res3));
  inter.push_back(std::move(y));

  // Let's now try to evaluate the new graph.

  x->setTrue();

  for (auto& ivar : inter)
    ivar->setFalse(); // the res_ should all be false.

  
  // Loop until all inner vars are updated
  int number_updated_var{ 0 };
  int number_vars_to_update{ static_cast<int>(inter.size()) };

  while (number_updated_var < number_vars_to_update)
    {
      number_updated_var += update_vars_in_graph(graph, inter);
    }
  std::cout << "\nUpdated Graph after x=3.0. \n";
  graph.printGraph(customPrint);
  

  std::cout << "Backpropagation starting: " << std::endl;
  auto grad_table{ backProp_walk(graph, *inter.at(3)) };
  printGradTable(grad_table);
}


void testBasicOperations()
{
  // Testing the basic operations by creating simple functions.
  // Function is     (x^3 -  5*x^2 + 10*x - 20)/(x - 5)^2
  // Derivative is   (-10 + 40*x - 15*x^2 + x^3)/(-5 + x)^3
  //
  // Written pythonically
  // (x**3 -  5*x**2 + 10*x - 20)/(x - 5)**2
  // (-10 + 40*x - 15*x**2 + x**3)/(-5 + x)**3
  
  DirectedGraph<Variable*> graph{};

  auto x{ std::make_unique<Scalar>("x", input, 2.0) };
  auto _x2{ scalarMul(graph, *x, *x) };
  auto _x3{ scalarMul(graph, *x, *_x2) };

  auto five{ std::make_unique<Scalar>("five", input, 5.0) };
  auto _5x2{ scalarMul(graph, *five, *_x2) };

  auto ten{ std::make_unique<Scalar>("ten", input, 10.0) };
  auto _10x{ scalarMul(graph, *ten, *x) };

  auto twenty{ std::make_unique<Scalar>("twenty", input, 20.0) };
  
  auto ps1{ scalarSub(graph, *_x3, *_5x2) };
  auto ps2{ scalarSub(graph, *_10x, *twenty) };
  auto num{ scalarAdd(graph, *ps1, *ps2) };

  auto mon{ scalarSub(graph, *x, *five) };
  auto den{ scalarMul(graph, *mon, *mon) };

  auto y{ scalarDiv(graph, *num, *den) };

  auto customPrint{ [] (Variable* varptr) -> void
  {
    std::cout << *varptr;
  }};
  graph.printGraph(customPrint);

  assert(Scalar::value(*y) - (-1.33333) < 1e-4
	       && "The function should evaluate to this value");

  std::cout << "Backpropagation starting: " << std::endl;
  auto grad_table{ backProp_walk(graph, *y) };
  printGradTable(grad_table);

  // Note that the Scalar gradient is a vector with only one entry which is the derivative.
	 assert(grad_table.at(x.get())[0] - (-0.66666) < 1e-4
		&& "The gradient of the input should be this value");  
  
}


void testNotSoBasicOperations()
{
  // The function to test is
  // (exp(2*x) + abs(5*x) - x^(1/2))/(ln(x+1))
  // This evaluates to 16.4309 at x=1.0
  // Derivative at 1.0 should evaluate to 15.96

  DirectedGraph<Variable*> graph{};

  auto x{ std::make_unique<Scalar>("x", input, 1.0) };
  
  auto two{ std::make_unique<Scalar>("two", input, 2.0)};
  auto _2x{ scalarMul(graph, *two, *x)};
  auto exp_2x{ scalarExp(graph, *_2x)};

  auto five{ std::make_unique<Scalar>("five", input, 5.0)};
  auto _5x{ scalarMul(graph, *five, *x) };
  auto abs_5x{ scalarAbs(graph, *_5x) };

  auto half{ std::make_unique<Scalar>("half", input, 0.5)};
  auto x_1d5{ scalarXpn(graph, *x, *half)};
  
  auto one{ std::make_unique<Scalar>("one", input, 1.0) };
  auto _xp1{ scalarAdd(graph, *x, *one)};
  auto log_xp1{ scalarLog(graph, *_xp1)};

  auto ps1{ scalarAdd(graph, *exp_2x, *abs_5x)};
  auto ps2{ scalarSub(graph, *ps1, *x_1d5)};

  auto y{ scalarDiv(graph, *ps2, *log_xp1)};
  
  auto customPrint{ [] (Variable* varptr) -> void
  {
    std::cout << *varptr;
  }};
  graph.printGraph(customPrint);

  //  assert(Scalar::value(*y) - (-1.33333) < 1e-4 && "The function should evaluate to this value");

  std::cout << "Backpropagation starting: " << std::endl;
  auto grad_table{ backProp_walk(graph, *y) };
  printGradTable(grad_table);

  // Note that the Scalar gradient is a vector with only one entry which is the derivative.
  //	 assert(grad_table.at(x.get())[0] - (-0.66666) < 1e-4 && "The gradient of the input should be this value");  
}

void testForwardProp()
{
  auto f{  [] (auto x) { return (x*x*x -  5*x*x + 10*x - 20)/( (x - 5)*(x - 5) );         }};
  auto f_p{[] (auto x) { return (x*x*x - 15*x*x + 40*x - 10)/( (x - 5)*(x - 5)*(x - 5) ); }};
  // (x**3 -  5*x**2 + 10*x - 20)/(x - 5)**2
  // (-10 + 40*x - 15*x**2 + x**3)/(-5 + x)**3
  
  DirectedGraph<Variable*> graph{};
  
  auto x{ std::make_unique<Scalar>("x", input, 2.0) };
  auto _x2{ scalarMul(graph, *x, *x) };
  auto _x3{ scalarMul(graph, *x, *_x2) };

  auto five{ std::make_unique<Scalar>("five", input, 5.0) };
  auto _5x2{ scalarMul(graph, *five, *_x2) };

  auto ten{ std::make_unique<Scalar>("ten", input, 10.0) };
  auto _10x{ scalarMul(graph, *ten, *x) };

  auto twenty{ std::make_unique<Scalar>("twenty", input, 20.0) };
  
  auto ps1{ scalarSub(graph, *_x3, *_5x2) };
  auto ps2{ scalarSub(graph, *_10x, *twenty) };
  auto num{ scalarAdd(graph, *ps1, *ps2) };

  auto mon{ scalarSub(graph, *x, *five) };
  auto den{ scalarMul(graph, *mon, *mon) };

  auto y{ scalarDiv(graph, *num, *den) };

  auto customPrint{ [] (Variable* varptr) -> void
  {
    std::cout << *varptr;
  }};
  graph.printGraph(customPrint);

  assert(y->getValue() - f(x->getValue()) < 1e-4
	       && "The function should evaluate to this value");
  x->setValue(1.0);
  forwardProp(graph, *y);
  assert(y->getValue() - f(x->getValue()) < 1e-4
	       && "The function should now evaluate to this value after FP.");
  std::cout << "New value is: " << y->getValue() << '\n';
}


int main()
{
  auto nl{ [] () {std::cout << '\n'; } };

  buildAddGraphAndEvaluateIt();
  nl();
  buildAddGraphAndBackPropIt();
  nl();
  buildAddGraphAndBackPropIt();
  nl();
  buildSubGraphEvaluateAndBackpropIt();
  nl();
  buildPolynomialGraph();
  nl();
  testBasicOperations();
  nl();
  testNotSoBasicOperations();
  nl();
  testForwardProp();
  nl();
    
  return 0;
}
