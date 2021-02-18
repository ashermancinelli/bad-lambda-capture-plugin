/*******************************************************************************
 *
 * @file test/syntax.cpp
 * @brief Contains examples of style guideline changes and suggestions the
 * SyntaxChecker plugin will perform. You will find a comment next to each
 * section of the code that indicates the intended outcome of the plugin.
 *
 ******************************************************************************/

// Should not warn
struct GoodName {};

// Should warn
struct badName {};

// Should suggest BetterName
struct better_name {};

struct GoodNameTwo_three {
  // Should warn
  int myvar;

  // Should warn
  double myMemberVar;

  // Should not warn
  int myvar_;
};

/* Passing flag `-s=foo` should strip `foo` from this name, resulting in
 * `NameWithPrefix`. */
struct fooNameWithPrefix {};

int main() {return 0;}
