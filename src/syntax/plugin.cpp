#include "clang/Frontend/FrontendPluginRegistry.h"
#include <config.hpp>
#include "consumer.hpp"

/*******************************************************************************
 * @file plugin.cpp
 * @brief outward-facing component of the library plugin.
 *
 * This source contains the component of the plugin specific to the dynamic
 * library version of the plugin.
 ******************************************************************************/

struct SyntaxCheckerAction : public clang::PluginASTAction {
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& CI,
      llvm::StringRef file) override {
    return std::make_unique<SyntaxCheckerConsumer>(CI);
  }
  virtual bool ParseArgs(const CompilerInstance &CI,
      const std::vector<std::string>& args) override {
    return true;
  }
};

static FrontendPluginRegistry::Add<SyntaxCheckerAction>
X(/*Name=*/"SyntaxChecker",
  /*Description=*/"Identify differences between code and select style guide "
  "policies.");
