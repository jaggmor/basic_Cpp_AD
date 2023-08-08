#include "Scalar.h"
#include "Variable.h"
#include "DirectedGraph.h"
#include "operation_constants.h"
#include "OperationUnary.h"
#include "OperationBinary.h"
#include "util.h"

#include <vector>
#include <memory>
#include <unordered_set>
#include <sstream>

using uvptr = std::unique_ptr<Variable>;
  
class Unit
{
private:
  std::vector<uvptr> m_varsContainer{};
  std::vector<Variable*> m_leafs{};
  DirectedGraph<Variable*> m_graph{};
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
  static void forwardProp(DirectedGraph<Variable*>& graph, Variable& output) {
    assert(output.getConsumers(graph).size() == 0 && "Output for a unit cannot have consumers.");
    visit(graph, output);
  }
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

#if 0
  // Need to be able to add two units together
  void binaryOp(Unit& o_unit, const OperationBinary& operation) {
    // We only support the same input for now.
    assert(getInput().getName() == o_unit.getInput().getName());

    // We save references to the outputs;
    Variable& this_output{getOutput()};
    Variable& othr_output{o_unit.getOutput()};
      
    // AS OF NOW THIS CODE DOES NOT SUPPORT MULTIPLE INPUTS.
    // 1. Find the common leafs.
    // First we define a comparator that compares two variable objects by their name.
    auto comparator{[](const Variable* v1, const Variable* v2) {
		      return v1->getName() == v2->getName();
		    }};
    auto matches{ util::intersect(m_leafs, o_unit.m_leafs, comparator) };

    // Since we do not yet support multiple inputs the following assertion holds.
    // absorbDisjoint will otherwise crash. This could be fixed quite easily later by simpluy calling
    // absorb if matches is empty. We may want to replace the getInput by some other method later.
    // This will also ensure that the lookup in matches does not crash below.
    assert( matches.size() > 0 && "Must have at least some matching variables.");
    
    // 2. Merge the graph from unit into *this. The graphs should not have any common elements
    // since each unit has exclusive ownership.
    m_graph.absorbDisjoint(o_unit.m_graph, matches);
    
    // 3. transfer ownership from the other unit.
    // First we want to move all variables from the other unit's containter
    // except the associated ones
    for (int i{0}; uvptr& varuptr : o_unit.m_varsContainer) {
      if (i < matches.size() && varuptr.get() == matches[i].second) {
	// other varuptr equivalent element in this unit.
	++i;
      } else { 
	m_varsContainer.emplace_back(std::move(varuptr));
      }
    }
    for (int i{0}; Variable* varptr : o_unit.m_leafs) {
      if (i < matches.size() && varptr == matches[i].second) {
	++i;
      } else {
	m_leafs.push_back(varptr);
      }
    }
    // Now we clean the other object's list into its null state.      
    o_unit.m_varsContainer.clear();
    o_unit.m_leafs.clear();

    // Now we need to add the outputs...
    // TODO This needs some control flow!

    // also, how do we know which add to use? More control flow will be requiered here.
    auto res{ scalarAdd(m_graph, this_output, othr_output) };
    // No need to push back anything else than the result which is of course not a leaf
    m_varsContainer.push_back(std::move(res));
    return *this;
  }
#endif

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
  // Need to be able to add two units together
  Unit& add(Unit& o_unit) {
    // We only support the same input for now.
    assert(getInput().getName() == o_unit.getInput().getName());

    // We save references to the outputs;
    Variable& this_output{getOutput()};
    Variable& othr_output{o_unit.getOutput()};
      
    // AS OF NOW THIS CODE DOES NOT SUPPORT MULTIPLE INPUTS.
    // 1. Find the common leafs.
    // First we define a comparator that compares two variable objects by their name.
    auto comparator{[](const Variable* v1, const Variable* v2) {
		      return v1->getName() == v2->getName();
		    }};
    auto matches{ util::intersect(m_leafs, o_unit.m_leafs, comparator) };

    // Since we do not yet support multiple inputs the following assertion holds.
    // absorbDisjoint will otherwise crash. This could be fixed quite easily later by simpluy calling
    // absorb if matches is empty. We may want to replace the getInput by some other method later.
    // This will also ensure that the lookup in matches does not crash below.
    assert( matches.size() > 0 && "Must have at least some matching variables.");
    
    // 2. Merge the graph from unit into *this. The graphs should not have any common elements
    // since each unit has exclusive ownership.
    m_graph.absorbDisjoint(o_unit.m_graph, matches);
    
    // 3. transfer ownership from the other unit.
    // First we want to move all variables from the other unit's containter
    // except the associated ones
    for (int i{0}; uvptr& varuptr : o_unit.m_varsContainer) {
      if (i < matches.size() && varuptr.get() == matches[i].second) {
	// other varuptr equivalent element in this unit.
	++i;
      } else { 
	m_varsContainer.emplace_back(std::move(varuptr));
      }
    }
    for (int i{0}; Variable* varptr : o_unit.m_leafs) {
      if (i < matches.size() && varptr == matches[i].second) {
	++i;
      } else {
	m_leafs.push_back(varptr);
      }
    }
    // Now we clean the other object's list into its null state.      
    o_unit.m_varsContainer.clear();
    o_unit.m_leafs.clear();

    // Now we need to add the outputs...
    // TODO This needs some control flow!

    // also, how do we know which add to use? More control flow will be requiered here.
    auto res{ scalarAdd(m_graph, this_output, othr_output) };
    // No need to push back anything else than the result which is of course not a leaf
    m_varsContainer.push_back(std::move(res));
    return *this;
  }
  
  Unit& add(double alpha) {
    binaryOp(alpha, scalarAdd);
    return *this;
  }  
  Unit& mul(double mu) {
    binaryOp(mu, scalarMul);
    return *this;
  }
  Unit& sub(double sigma) {
    binaryOp(sigma, scalarSub);
    return *this;
  }  
  Unit& div(double delta) {
    binaryOp(delta, scalarDiv);
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
  double forward(double inputValue) {
    // Setting the value for the input will result in a different output.
    Scalar::setValue( getInput(), inputValue );
    forwardProp(m_graph, getOutput());
    return Scalar::value( getOutput() );
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

