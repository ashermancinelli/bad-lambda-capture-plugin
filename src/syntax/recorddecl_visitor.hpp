#pragma once
#include "clang_common.hpp"
#include <optional>

/* Prefix to strip from class declarations, parsed in driver */
extern std::string StripPrefix;

/* Ensures classes/structs/enums use pascal case */
class CheckPascalCaseClassNames
  : public RecursiveASTVisitor<CheckPascalCaseClassNames> {
public:
  explicit CheckPascalCaseClassNames(ASTContext *Context) : Context(Context) {}

  bool VisitCXXRecordDecl(CXXRecordDecl* RD);
private:
  ASTContext *Context;
};
