#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;

struct FindLambdaCapturedFields
  : public RecursiveASTVisitor<FindLambdaCapturedFields> {
public:
  explicit FindLambdaCapturedFields(ASTContext *Context)
    : Context(Context) {}

  bool VisitMemberExpr(MemberExpr *Expr) {
    auto MemberType = Expr->getType();

    /* Problematic use of member variable! Time to generate diagnostic
     * information */
    if (MemberType->isArrayType() || MemberType->isPointerType()) {

      /* Report diagnostic information */
      clang::DiagnosticsEngine &DE = Context->getDiagnostics();

      /* Error message describing the issue */
      auto ID = DE.getCustomDiagID(
          clang::DiagnosticsEngine::Error,
          "Found lambda capturing pointer-like member variable here.\n");
      DE.Report(Expr->getBeginLoc(), ID);

      /* Remark indicating which member variable triggered the error */
      ID = DE.getCustomDiagID(clang::DiagnosticsEngine::Note,
          "Member variable declared here:");
      DE.Report(Expr->getMemberDecl()->getBeginLoc(), ID);

      /* Remark with suggested change to mitigate the issue */
      ID = DE.getCustomDiagID(clang::DiagnosticsEngine::Remark,
          "Consider creating a local copy of the member variable in local scope"
          "\njust outside the lambda capture.");
      DE.Report(Parent->getBeginLoc(), ID);
    }
    return true;
  }

  ASTContext *Context;
  LambdaExpr *Parent=nullptr;
};

class FindLambdaCaptureThis
  : public RecursiveASTVisitor<FindLambdaCaptureThis> {
public:
  explicit FindLambdaCaptureThis(ASTContext *Context)
    : Context(Context), MemberVisitor(Context) {}

  bool VisitLambdaExpr(LambdaExpr *Expr) {
    bool FoundThis = false;
    for (auto it = Expr->capture_begin(); it != Expr->capture_end(); it++) {
      if (it->capturesThis()) {
        FoundThis = true;
        break;
      }
    }

    /* If `this` is not captured, we don't care about it. */
    if (!FoundThis)
      return true;

    const CompoundStmt* LambdaBody = Expr->getCompoundStmtBody();
    if (LambdaBody->body_empty())
      return true;

    for(auto Stmt : LambdaBody->body()) {
      MemberVisitor.Parent = Expr;
      MemberVisitor.TraverseStmt(Stmt);
    }

    return true;
  }

private:
  ASTContext *Context;
  FindLambdaCapturedFields MemberVisitor;
};

class LambdaCaptureCheckerConsumer : public clang::ASTConsumer {
public:
  explicit LambdaCaptureCheckerConsumer(ASTContext *Context)
    : Visitor(Context) {}
  explicit LambdaCaptureCheckerConsumer(CompilerInstance& CI)
    : Visitor(&CI.getASTContext()) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  FindLambdaCaptureThis Visitor;
};
