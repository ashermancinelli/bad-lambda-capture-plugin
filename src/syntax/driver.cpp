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
    cl::desc("Strip prefix from class/struct decls."),
    cl::location(StripPrefix), cl::value_desc("prefix"),
    cl::cat(SyntaxCheckerCategory));

bool DoCheckPascalCaseClassNames=true;
static cl::opt<bool, true> CheckClassNamesOption("class-names",
    cl::desc("Check for class names to be in pascal case."),
    cl::location(DoCheckPascalCaseClassNames), cl::cat(SyntaxCheckerCategory));

bool DoCheckFieldNames=true;
static cl::opt<bool, true> CheckFieldNamesOption("field-names",
    cl::desc("Check for field names to be in snake case with a trailing "
      "underscore."), cl::location(DoCheckFieldNames),
    cl::cat(SyntaxCheckerCategory));

auto VersionPrinter = [] (llvm::raw_ostream& OS) {
  OS << R"(
===--------------------------------------------------------------------------===

   SyntaxChecker

   --------------------------------------------------------------------------   

   [Version )" CHECKER_VERSION << " compiled on " << __DATE__ << R"(]

===--------------------------------------------------------------------------===

)";
};

int main(int argc, const char **argv) {

  std::vector<std::string> args(argv, argv+argc);
  cl::SetVersionPrinter(VersionPrinter);
  CommonOptionsParser Op(argc, argv, SyntaxCheckerCategory);

  /* Create a new Clang Tool instance (a LibTooling environment). */
  ClangTool Tool(Op.getCompilations(), Op.getSourcePathList());

  if (StripPrefix != "" && !DoCheckPascalCaseClassNames) {
    llvm::errs() << "Option --strip=<prefix> requires option --class-names to "
      << "be enabled!\n";
    return 1;
  }

  return Tool.run(newFrontendActionFactory<SyntaxCheckerAction>().get());
}
