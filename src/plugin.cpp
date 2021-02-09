#include "actions.hpp"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include <config.hpp>

struct FindBadLambdaCapturesAction : public clang::PluginASTAction {
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& CI,
      llvm::StringRef file) override {
    return std::make_unique<FindBadLambdaCapturesConsumer>(CI);
  }
  virtual bool ParseArgs(const CompilerInstance &CI,
      const std::vector<std::string>& args) override {
    return true;
  }
};

static FrontendPluginRegistry::Add<FindBadLambdaCapturesAction>
X(/*Name=*/PROJECT_NAME,
  /*Description=*/"Identify potentially problematic lambda captures of member"
    " variables.");
