#include "actions.hpp"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include <config.hpp>

struct LambdaCheckerCapturesAction : public clang::PluginASTAction {
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& CI,
      llvm::StringRef file) override {
    return std::make_unique<LambdaCaptureCheckerConsumer>(CI);
  }
  virtual bool ParseArgs(const CompilerInstance &CI,
      const std::vector<std::string>& args) override {
    return true;
  }
};

static FrontendPluginRegistry::Add<LambdaCheckerCapturesAction>
X(/*Name=*/PROJECT_NAME,
  /*Description=*/"Identify potentially problematic lambda captures of member"
    " variables.");
