#pragma once

#include "config.hpp"
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

/*******************************************************************************
 *
 * @file clang_common.hpp
 *
 * @brief Convenience header to include all relevant headers for the clang
 * tools. Should this project increase in size, this header ought to be removed
 * such that only the needed headers are included in each TU. This should
 * decrease compile time, but for now is negligible.
 *
 ******************************************************************************/
