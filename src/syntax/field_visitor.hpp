#pragma once

#include "clang_common.hpp"

/**
 * @brief Check member variable names for trailing underscore and snake case.
 */
class CheckFieldNames
  : public RecursiveASTVisitor<CheckFieldNames> {
public:
  explicit CheckFieldNames(ASTContext *Context)
    : Context(Context) {}

  /* Performs two checks to ensure the name matches style guidelines:
   *
   * - Does the name end with a '_'?
   * - Does the name contain any uppercase letters?
   *
   * If either of these conditions is found to be true, create a suggested
   * modification to the name and print a diagnostic.
   */
  bool VisitFieldDecl(FieldDecl* FD);
private:
  ASTContext *Context;
};
