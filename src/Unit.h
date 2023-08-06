#include "Scalar.h"
#include "Variable.h"
#include "DirectedGraph.h"
#include "operation_constants.h"
#include "OperationUnary.h"
#include "OperationBinary.h"

#include <vector>
#include <memory>
#include <unordered_set>
#include <sstream>

using uvptr = std::unique_ptr<Variable>;
  
class Unit
{
private:
  std::vector<uvptr> m_varsContainer{};
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
    m_varsContainer.push_back(std::move(rightArg));
    m_varsContainer.push_back(std::move(res));
  }
  void binaryOp(double value, const OperationBinary& operation) {    
    static int add_counter{0};
    std::stringstream ss{};
    ss << "add_param" << add_counter++; 
    binaryOp(std::make_unique<Scalar>(input, value, ss.str()), operation);
  }
  void unaryOp(const OperationUnary& operation) {
    auto res{ operation(m_graph, getOutput()) };
    m_varsContainer.push_back(std::move(res));
  }

public:
  Unit(Scalar&& scalar) {
    uvptr x{ std::make_unique<Scalar>(std::move(scalar)) }; // Move to heap for longer lifetime.
    m_graph.addNode(x.get());
    m_varsContainer.push_back(std::move(x));
  }
  Variable& getInput() {
    return *(m_varsContainer.front());
  }
  Variable& getOutput() {
    return *(m_varsContainer.back());
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
    m_graph.printGraph();
  }
};
