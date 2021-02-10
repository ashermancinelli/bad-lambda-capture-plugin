#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include "actions.hpp"
#include <config.hpp>
#include "clang/Tooling/CommonOptionsParser.h"

using namespace llvm;
using namespace clang;

class LambdaCaptureCheckerAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new LambdaCaptureCheckerConsumer(&Compiler.getASTContext()));
  }
};

static cl::OptionCategory LambdaCaptureCheckerCategory(
    PROJECT_NAME " options");

int main(int argc, const char **argv) {
  clang::tooling::CommonOptionsParser OptionsParser(argc, argv,
      LambdaCaptureCheckerCategory);
  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
      OptionsParser.getSourcePathList());
  return Tool.run(
      clang::tooling::newFrontendActionFactory<LambdaCaptureCheckerAction>().get());
}
