#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include "actions.hpp"
#include <config.hpp>

class FindBadLambdaCapturesAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new FindBadLambdaCapturesConsumer(&Compiler.getASTContext()));
  }
};

int main(int argc, char **argv) {

  std::vector<std::string> args(argv, argv+argc);

  auto usage = [&] {
    std::stringstream ss;
    ss << "Searches C++ AST for lambda captures that may be problematic when"
      << " using portability libraries such as RAJA and Kokkos."
      << "\nUsage:\n\t" << args[0] << " [-f file] [-s string]\n";
    return ss.str();
  };

  if (argc != 3) {
    std::cout << usage();
    return 1;
  }

  std::string storage;

  /* Read from file */
  if (args[1] == "-f") {
    std::ifstream f(args[2], std::ios_base::in);
    f.unsetf(std::ios::skipws);
    std::copy(
        std::istream_iterator<char>(f),
        std::istream_iterator<char>(),
        std::back_inserter(storage));
  }
  /* Read from string */
  else if (args[1] == "-s") {
    storage.assign(args[1]);
  }
  else {
    std::cout << usage();
    return 1;
  }

  clang::tooling::runToolOnCode(std::make_unique<FindBadLambdaCapturesAction>(), storage.c_str());
  return 0;
}
