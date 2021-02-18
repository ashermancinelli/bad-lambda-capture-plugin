#pragma once
#include "clang_common.hpp"
#include "recorddecl_visitor.hpp"
#include "field_visitor.hpp"

/*******************************************************************************
 *
 * @file consumer.hpp
 *
 * @brief includes the visitors described by this plugin and creates a consumer
 * to apply the visitors on each TU.
 *
 ******************************************************************************/

class SyntaxCheckerConsumer : public clang::ASTConsumer {
public:
  explicit SyntaxCheckerConsumer(ASTContext *Context)
    : ClassVisitor(Context), FieldVisitor(Context) {}
  explicit SyntaxCheckerConsumer(CompilerInstance& CI)
    : ClassVisitor(&CI.getASTContext()), FieldVisitor(&CI.getASTContext()) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    ClassVisitor.TraverseDecl(Context.getTranslationUnitDecl());
    FieldVisitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  CheckPascalCaseClassNames ClassVisitor;
  CheckFieldNames FieldVisitor;
};
