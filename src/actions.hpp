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

    /*
        VarDecl* Decl = it->getCapturedVar();
        QualType Type = Decl->getType();

        if (Type->isPointerType()) {
          FullSourceLoc FullLocation = Context->getFullLoc(Expr->getBeginLoc());
          if (FullLocation.isValid())
            llvm::outs() << "Found lambda capturing pointer-like member variable "
              << "via `this` pointer at line="
              << FullLocation.getSpellingLineNumber() << " column="
              << FullLocation.getSpellingColumnNumber() << "\n";
        }
        */
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

      /*
      ValueDecl* VD = Expr->getMemberDecl();
      std::string MemberTypeString;
      llvm::raw_string_ostream OS(MemberTypeString);
      Expr->getType()->dump(OS, *Context);
      */

      /* Gather source location information */
      /*
      FullSourceLoc UseLocation = Context->getFullLoc(Expr->getBeginLoc());
      FullSourceLoc DefineLocation = Context->getFullLoc(VD->getBeginLoc());
      FullSourceLoc LambdaLocation = Context->getFullLoc(Parent->getBeginLoc());
      */

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

class FindBadLambdaCapturesConsumer : public clang::ASTConsumer {
public:
  explicit FindBadLambdaCapturesConsumer(ASTContext *Context)
    : Visitor(Context) {}
  explicit FindBadLambdaCapturesConsumer(CompilerInstance& CI)
    : Visitor(&CI.getASTContext()) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  FindLambdaCaptureThis Visitor;
};
