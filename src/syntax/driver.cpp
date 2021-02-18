#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <optional>
#include "clang/Tooling/CommonOptionsParser.h"
#include <config.hpp>
#include "consumer.hpp"

/*******************************************************************************
 *
 * @file driver.cpp
 * @brief outward-facing component of the tool. This source contains the
 * component of the plugin specific to the standalone version.
 *
 ******************************************************************************/

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

using std::optional;
using std::nullopt;

class SyntaxCheckerAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new SyntaxCheckerConsumer(&Compiler.getASTContext()));
  }
};

static cl::OptionCategory SyntaxCheckerCategory("SyntaxChecker options");
std::string StripPrefix="";
static cl::opt<std::string, true> StripPrefixOption("strip",
    cl::desc("Strip prefix from class/struct decls."
      "stripping."), cl::location(StripPrefix),
    cl::value_desc("prefix"), cl::cat(SyntaxCheckerCategory));

int main(int argc, const char **argv) {

  cl::SetVersionPrinter([] (llvm::raw_ostream& os) {
        os << R"(
===--------------------------------------------------------------------------===

   SyntaxChecker

   --------------------------------------------------------------------------   

   [Version )" CHECKER_VERSION << "\n  compiled on " << __DATE__ << R"(]

===--------------------------------------------------------------------------===

)";
      });

  CommonOptionsParser Op(argc, argv, SyntaxCheckerCategory);

  /* Create a new Clang Tool instance (a LibTooling environment). */
  ClangTool Tool(Op.getCompilations(), Op.getSourcePathList());

  return Tool.run(newFrontendActionFactory<SyntaxCheckerAction>().get());
}
