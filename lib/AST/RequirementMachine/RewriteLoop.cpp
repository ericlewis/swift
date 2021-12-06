//===--- RewriteLoop.cpp - Identities between rewrite rules ---------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2021 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#include "swift/AST/Type.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include "RewriteSystem.h"

using namespace swift;
using namespace rewriting;

/// Dumps the rewrite step that was applied to \p term. Mutates \p term to
/// reflect the application of the rule.
void RewriteStep::dump(llvm::raw_ostream &out,
                       RewritePathEvaluator &evaluator,
                       const RewriteSystem &system) const {
  switch (Kind) {
  case ApplyRewriteRule: {
    auto result = evaluator.applyRewriteRule(*this, system);

    if (!result.prefix.empty()) {
      out << result.prefix;
      out << ".";
    }
    out << "(" << result.lhs << " => " << result.rhs << ")";
    if (!result.suffix.empty()) {
      out << ".";
      out << result.suffix;
    }

    break;
  }
  case AdjustConcreteType: {
    auto result = evaluator.applyAdjustment(*this, system);

    out << "(σ";
    out << (Inverse ? " - " : " + ");
    out << result << ")";

    break;
  }
  case Shift: {
    evaluator.applyShift(*this, system);

    out << (Inverse ? "B>A" : "A>B");
    break;
  }
  case Decompose: {
    evaluator.applyDecompose(*this, system);

    out << (Inverse ? "Compose(" : "Decompose(");
    out << RuleID << ")";
    break;
  }
  }
}

/// Invert a rewrite path, producing a path that rewrites the original path's
/// destination back to the original path's source.
void RewritePath::invert() {
  std::reverse(Steps.begin(), Steps.end());

  for (auto &step : Steps)
    step.invert();
}

/// Dumps a series of rewrite steps applied to \p term.
void RewritePath::dump(llvm::raw_ostream &out,
                       MutableTerm term,
                       const RewriteSystem &system) const {
  RewritePathEvaluator evaluator(term);
  bool first = true;

  for (const auto &step : Steps) {
    if (!first) {
      out << " ⊗ ";
    } else {
      first = false;
    }

    step.dump(out, evaluator, system);
  }
}

void RewriteLoop::dump(llvm::raw_ostream &out,
                       const RewriteSystem &system) const {
  out << Basepoint << ": ";
  Path.dump(out, Basepoint, system);
  if (isDeleted())
    out << " [deleted]";
}

void RewritePathEvaluator::dump(llvm::raw_ostream &out) const {
  out << "A stack:\n";
  for (const auto &term : A) {
    out << term << "\n";
  }
  out << "\nB stack:\n";
  for (const auto &term : B) {
    out << term << "\n";
  }
}

void RewritePathEvaluator::checkA() const {
  if (A.empty()) {
    llvm::errs() << "Empty A stack\n";
    dump(llvm::errs());
    abort();
  }
}

void RewritePathEvaluator::checkB() const {
  if (B.empty()) {
    llvm::errs() << "Empty B stack\n";
    dump(llvm::errs());
    abort();
  }
}

MutableTerm &RewritePathEvaluator::getCurrentTerm() {
  checkA();
  return A.back();
}

AppliedRewriteStep
RewritePathEvaluator::applyRewriteRule(const RewriteStep &step,
                                       const RewriteSystem &system) {
  auto &term = getCurrentTerm();

  assert(step.Kind == RewriteStep::ApplyRewriteRule);

  const auto &rule = system.getRule(step.RuleID);

  auto lhs = (step.Inverse ? rule.getRHS() : rule.getLHS());
  auto rhs = (step.Inverse ? rule.getLHS() : rule.getRHS());

  auto bug = [&](StringRef msg) {
    llvm::errs() << msg << "\n";
    llvm::errs() << "- Term: " << term << "\n";
    llvm::errs() << "- StartOffset: " << step.StartOffset << "\n";
    llvm::errs() << "- EndOffset: " << step.EndOffset << "\n";
    llvm::errs() << "- Expected subterm: " << lhs << "\n";
    abort();
  };

  if (term.size() != step.StartOffset + lhs.size() + step.EndOffset) {
    bug("Invalid whiskering");
  }

  if (!std::equal(term.begin() + step.StartOffset,
                  term.begin() + step.StartOffset + lhs.size(),
                  lhs.begin())) {
    bug("Invalid subterm");
  }

  MutableTerm prefix(term.begin(), term.begin() + step.StartOffset);
  MutableTerm suffix(term.end() - step.EndOffset, term.end());

  term = prefix;
  term.append(rhs);
  term.append(suffix);

  return {lhs, rhs, prefix, suffix};
}

MutableTerm
RewritePathEvaluator::applyAdjustment(const RewriteStep &step,
                                      const RewriteSystem &system) {
  auto &term = getCurrentTerm();

  assert(step.Kind == RewriteStep::AdjustConcreteType);

  auto &ctx = system.getRewriteContext();
  MutableTerm prefix(term.begin() + step.StartOffset,
                     term.begin() + step.StartOffset + step.RuleID);

  // We're either adding or removing the prefix to each concrete substitution.
  term.back() = term.back().transformConcreteSubstitutions(
    [&](Term t) -> Term {
      if (step.Inverse) {
        if (!std::equal(t.begin(),
                        t.begin() + step.RuleID,
                        prefix.begin())) {
          llvm::errs() << "Invalid rewrite path\n";
          llvm::errs() << "- Term: " << term << "\n";
          llvm::errs() << "- Substitution: " << t << "\n";
          llvm::errs() << "- Start offset: " << step.StartOffset << "\n";
          llvm::errs() << "- Expected subterm: " << prefix << "\n";
          abort();
        }

        MutableTerm mutTerm(t.begin() + step.RuleID, t.end());
        return Term::get(mutTerm, ctx);
      } else {
        MutableTerm mutTerm(prefix);
        mutTerm.append(t);
        return Term::get(mutTerm, ctx);
      }
    }, ctx);

  return prefix;
}

void RewritePathEvaluator::applyShift(const RewriteStep &step,
                                      const RewriteSystem &system) {
  assert(step.Kind == RewriteStep::Shift);
  assert(step.StartOffset == 0);
  assert(step.EndOffset == 0);
  assert(step.RuleID == 0);

  if (!step.Inverse) {
    // Move top of A stack to B stack.
    checkA();
    B.push_back(A.back());
    A.pop_back();
  } else {
    // Move top of B stack to A stack.
    checkB();
    A.push_back(B.back());
    B.pop_back();
  }
}

void RewritePathEvaluator::applyDecompose(const RewriteStep &step,
                                          const RewriteSystem &system) {
  assert(step.Kind == RewriteStep::Decompose);
  assert(step.EndOffset == 0);

  auto &ctx = system.getRewriteContext();
  unsigned numSubstitutions = step.RuleID;

  if (!step.Inverse) {
    // The top of the A stack must be a term ending with a superclass or
    // concrete type symbol.
    const auto &term = getCurrentTerm();
    auto symbol = term.back();
    if (!symbol.hasSubstitutions()) {
      llvm::errs() << "Expected term with superclass or concrete type symbol"
                   << " on A stack\n";
      dump(llvm::errs());
      abort();
    }

    // The symbol must have the expected number of substitutions.
    if (symbol.getSubstitutions().size() != numSubstitutions) {
      llvm::errs() << "Expected " << numSubstitutions << " substitutions\n";
      dump(llvm::errs());
      abort();
    }

    // Push each substitution on the A stack.
    for (auto substitution : symbol.getSubstitutions()) {
      A.push_back(MutableTerm(substitution));
    }
  } else {
    // The A stack must store the number of substitutions, together with a
    // term ending with a superclass or concrete type symbol.
    if (A.size() < numSubstitutions + 1) {
      llvm::errs() << "Not enough terms on A stack\n";
      dump(llvm::errs());
      abort();
    }

    // The term immediately underneath the substitutions is the one we're
    // updating with new substitutions.
    auto &term = *(A.end() - numSubstitutions - 1);
    auto symbol = term.back();
    if (!symbol.hasSubstitutions()) {
      llvm::errs() << "Expected term with superclass or concrete type symbol"
                   << " on A stack\n";
      dump(llvm::errs());
      abort();
    }

    // The symbol at the end of this term must have the expected number of
    // substitutions.
    if (symbol.getSubstitutions().size() != numSubstitutions) {
      llvm::errs() << "Expected " << numSubstitutions << " substitutions\n";
      dump(llvm::errs());
      abort();
    }

    // Collect the substitutions from the A stack.
    SmallVector<Term, 2> substitutions;
    substitutions.reserve(numSubstitutions);
    for (unsigned i = 0; i < numSubstitutions; ++i) {
      const auto &substitution = *(A.end() - numSubstitutions + i);
      substitutions.push_back(Term::get(substitution, ctx));
    }

    // Build the new symbol with the new substitutions.
    auto newSymbol = (symbol.getKind() == Symbol::Kind::Superclass
                      ? Symbol::forSuperclass(symbol.getSuperclass(),
                                              substitutions, ctx)
                      : Symbol::forConcreteType(symbol.getConcreteType(),
                                                substitutions, ctx));

    // Update the term with the new symbol.
    term.back() = newSymbol;

    // Pop the substitutions from the A stack.
    A.resize(A.size() - numSubstitutions);
  }
}

void RewritePathEvaluator::apply(const RewriteStep &step,
                                 const RewriteSystem &system) {
  switch (step.Kind) {
  case RewriteStep::ApplyRewriteRule:
    (void) applyRewriteRule(step, system);
    break;

  case RewriteStep::AdjustConcreteType:
    (void) applyAdjustment(step, system);
    break;

  case RewriteStep::Shift:
    applyShift(step, system);
    break;

  case RewriteStep::Decompose:
    applyDecompose(step, system);
    break;
  }
}
