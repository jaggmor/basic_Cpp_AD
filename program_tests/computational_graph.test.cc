
#include "operations.h"
#include "DirectedGraph.h"
#include "Scalar.h"

#include <iostream>
#include <vector>
#include <memory>


using v_ptr = std::unique_ptr<Variable>;


constexpr Input input{};
constexpr ScalarAdd scalarAdd{};


void buildAddGraph()
{
  std::vector<v_ptr> vars {};
  
  DirectedGraph<Variable*> graph{};

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


int main()
{
  buildAddGraph();
  return 0;
}
