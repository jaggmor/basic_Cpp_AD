#include "Scalar.h"
#include "Variable.h"
#include "DirectedGraph.h"
#include "operation_constants.h"
#include "OperationUnary.h"
#include "OperationBinary.h"
#include "util.h"
#include "forwardProp.h"
#include "backProp.h"

#include <vector>
#include <memory>
#include <sstream>
#include <utility>

using uvptr = std::unique_ptr<Variable>;
  
class Unit
{
private:
  std::vector<uvptr> m_varsContainer{};
  std::vector<Variable*> m_leafs{};
  DirectedGraph<Variable*> m_graph{};
  
  void binaryOp(uvptr rightArg, const OperationBinary& operation) {
    auto res{ operation(m_graph, getOutput(), *rightArg) };
    m_leafs.push_back(rightArg.get());
    m_varsContainer.push_back(std::move(rightArg));
    m_varsContainer.push_back(std::move(res));
  }
  void binaryOp(double value, const OperationBinary& operation) {    
    static int counter{0};
    std::stringstream ss{};
    ss << "prm-" << counter++; 
    binaryOp(std::make_unique<Scalar>(ss.str(), input, value), operation);
  }
  void unaryOp(const OperationUnary& operation) {
    auto res{ operation(m_graph, getOutput()) };
    m_varsContainer.push_back(std::move(res));
  }

  /* Performs a matched merge with o_unit. o_ubut will be left in an indeterminate state.*/
  void matchedMerge(Unit& o_unit, const std::vector<std::pair<Variable*, Variable*>>& matches)
  {
    assert( matches.size() > 0 && "Can't merge two disjoint graphs without any matches.");
    // Merge the graph from o_unit into *this. The graphs should not have any common elements
    // since each unit is designed to have exclusive ownership over its variables.
    m_graph.absorbDisjoint(o_unit.m_graph, matches);
    
    // transfer ownership from the other unit.
    // First all variables are moved from the other unit's containter except the matched ones.
    for (int i{0}; uvptr& varuptr : o_unit.m_varsContainer) {
      if (i < matches.size() && varuptr.get() == matches[i].second) { 
	++i;   // Matched vars appear in order.
      } else { 
	m_varsContainer.emplace_back(std::move(varuptr));
      }
    }
    // Also transfer leafs.
    for (int i{0}; Variable* varptr : o_unit.m_leafs) {
      if (i < matches.size() && varptr == matches[i].second) {
	++i;
      } else {
	m_leafs.push_back(varptr);
      }
    }
    o_unit.m_varsContainer.clear(); // Now we clean the other object's list into its null state.
    o_unit.m_leafs.clear();
  }
  
  void binaryOp(Unit& o_unit, const OperationBinary& operation) {    
    assert(getInput().getName() == o_unit.getInput().getName()); // Only equal inputs for now.
    
    Variable& this_output{getOutput()};        // Save references to the outputs;
    Variable& othr_output{o_unit.getOutput()}; //
      
    // Find the common leafs.
    auto comparator{[](const Variable* v1, const Variable* v2) { // Variables are merged if they
		      return v1->getName() == v2->getName();     // have the same name.
		    }};
    auto matches{ util::intersect(m_leafs, o_unit.m_leafs, comparator) };
    // Until multiple inputs are allowed the inputs must provide at least one match.
    assert( matches.size() > 0 && "Must have at least some matching variables.");
    matchedMerge(o_unit, matches);    
    // also, how do we know which add to use? More control flow will be requiered here.
    auto res{ operation(m_graph, this_output, othr_output) };
    // No need to push back anything else than the result which is of course not a leaf
    m_varsContainer.push_back(std::move(res));
  }
  
public:
  Unit(Scalar&& scalar) {
    uvptr x{ std::make_unique<Scalar>(std::move(scalar)) }; // Move to heap for longer lifetime.
    m_graph.addNode(x.get());
    m_leafs.push_back(x.get());
    m_varsContainer.push_back(std::move(x));
  }
  Variable& getInput() {
    return *(m_varsContainer.front());
  }
  Variable& getOutput() {
    return *(m_varsContainer.back());
  }
  Unit& add(Unit& o_unit) {
    binaryOp(o_unit, scalarAdd);
    return *this;
  }
  Unit& add(double alpha) {
    binaryOp(alpha, scalarAdd);
    return *this;
  }
  Unit& mul(Unit& o_unit) {
    binaryOp(o_unit, scalarMul);
    return *this;
  }
  Unit& mul(double mu) {
    binaryOp(mu, scalarMul);
    return *this;
  }
  Unit& sub(Unit& o_unit) {
    binaryOp(o_unit, scalarSub);
    return *this;
  }
  Unit& sub(double sigma) {
    binaryOp(sigma, scalarSub);
    return *this;
  }  
  Unit& div(Unit& o_unit) {
    binaryOp(o_unit, scalarDiv);
    return *this;
  }
  Unit& div(double delta) {
    binaryOp(delta, scalarDiv);
    return *this;
  }
  Unit& xpn(Unit& o_unit) {
    binaryOp(o_unit, scalarXpn);
    return *this;
  }
  Unit& xpn(double xi) {
    binaryOp(xi, scalarXpn);
    return *this;
  }
  Unit& log() {
    unaryOp(scalarLog);
    return *this;
  }
  Unit& abs() {
    unaryOp(scalarAbs);
    return *this;
  }
  Unit& exp() {
    unaryOp(scalarExp);
    return *this;
  }
  /**
   * @brief Joins output of this unit with input of other unit.
   */
  Unit& join(Unit& n_unit) {
    const auto& thisOutLen{ this->getOutput().getLengths() };
    const auto& othrInLen{ n_unit.getInput().getLengths() };    
    if (thisOutLen.size() != othrInLen.size() || thisOutLen != othrInLen) {
      throw InvalidOperationException("Joining elements have different sizes"); 
    }
    // Input and output have equal dimensions.
    std::vector<std::pair<Variable*, Variable*>> inOutMatch{    // Should delete input leaf after 
      std::make_pair(&this->getOutput(), &n_unit.getInput()) }; // disjointAbsorb.
    matchedMerge(n_unit, inOutMatch);
    return *this;
  }
  // @note The output must be scalar.
  double forward(double inputValue) {
    // Setting the value for the input will result in a different output.
    Scalar::setValue( getInput(), inputValue );
    return Scalar::value( forwardProp(m_graph, getOutput()) );
  }
  // @note The output must be scalar.
  double backward(double inputValue) {
    Scalar::setValue( getInput(), inputValue );
    forwardProp(m_graph, getOutput());
    map<Variable*, Gradient> grad_table{ backProp_walk(m_graph, getOutput()) };
    const Gradient& grad_input{ grad_table.at( &getInput()) };
    assert(grad_input.size() == 1);
    return grad_input[0];
  }
  void printGraph() {
    auto customPrint{ [] (Variable* varptr) -> void {
      std::cout << *varptr;
    }};
  m_graph.printGraph(customPrint);
  }
  void printLeafs() {
    std::cout << "Unit leafs:\n";
    for (auto varptr : m_leafs) {
      std::cout << *varptr << '\n';
    }
  }
};

