//===--- UnusedParametersCheck.cpp - clang-tidy----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "UnusedParametersCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {

void UnusedParametersCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      parmVarDecl(hasParent(functionDecl().bind("function"))).bind("x"),
      this);
}

static FixItHint removeParameter(const FunctionDecl *Function, unsigned Index) {
  const ParmVarDecl *Param = Function->getParamDecl(Index);
  unsigned ParamCount = Function->getNumParams();
  SourceRange RemovalRange = Param->getSourceRange();
  if (ParamCount == 1)
    return FixItHint::CreateRemoval(RemovalRange);

  if (Index == 0)
    RemovalRange.setEnd(
        Function->getParamDecl(Index + 1)->getLocStart().getLocWithOffset(-1));
  else
    RemovalRange.setBegin(
        Function->getParamDecl(Index - 1)->getLocEnd().getLocWithOffset(1));

  return FixItHint::CreateRemoval(RemovalRange);
}

static FixItHint removeArgument(const CallExpr *Call, unsigned Index) {
  unsigned ArgCount = Call->getNumArgs();
  const Expr *Arg = Call->getArg(Index);
  SourceRange RemovalRange = Arg->getSourceRange();
  if (ArgCount == 1)
    return FixItHint::CreateRemoval(RemovalRange);
  if (Index == 0)
    RemovalRange.setEnd(
        Call->getArg(Index + 1)->getLocStart().getLocWithOffset(-1));
  else
    RemovalRange.setBegin(
        Call->getArg(Index - 1)->getLocEnd().getLocWithOffset(1));
  return FixItHint::CreateRemoval(RemovalRange);
}

void UnusedParametersCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Function = Result.Nodes.getNodeAs<FunctionDecl>("function");
  if (!Function->doesThisDeclarationHaveABody())
    return;
  const auto *Param = Result.Nodes.getNodeAs<ParmVarDecl>("x");
  if (Param->isUsed() || Param->isReferenced() || !Param->getDeclName() ||
      Param->hasAttr<UnusedAttr>())
    return;

  auto MyDiag = diag(Param->getLocation(), "parameter '%0' is unused")
                << Param->getName();

  auto UsedByRef = [&] {
    return !ast_matchers::match(
                decl(hasDescendant(
                    declRefExpr(to(equalsNode(Function)),
                                unless(hasAncestor(
                                    callExpr(callee(equalsNode(Function)))))))),
                *Result.Context->getTranslationUnitDecl(), *Result.Context)
                .empty();
  };

  // Comment out parameter name for non-local functions.
  if ((Function->isExternallyVisible() &&
       Function->getStorageClass() != StorageClass::SC_Static) ||
      UsedByRef()) {
    SourceRange RemovalRange(Param->getLocation(), Param->getLocEnd());
    MyDiag << FixItHint::CreateReplacement(
        RemovalRange, (Twine(" /*") + Param->getName() + "*/").str());
    return;
  }

  // Handle local functions by deleting the parameters.
  unsigned ParamIndex = Param->getFunctionScopeIndex();
  assert(ParamIndex < Function->getNumParams());

  // Fix all redeclarations.
  for (const FunctionDecl *FD : Function->redecls())
    MyDiag << removeParameter(FD, ParamIndex);

  // Fix all call sites.
  auto CallMatches = ast_matchers::match(
      decl(forEachDescendant(
          callExpr(callee(functionDecl(equalsNode(Function)))).bind("x"))),
      *Result.Context->getTranslationUnitDecl(), *Result.Context);
  for (const auto &Match : CallMatches)
    MyDiag << removeArgument(Match.getNodeAs<CallExpr>("x"), ParamIndex);
}

} // namespace tidy
} // namespace clang
