#include "clang_common.hpp"
#include "field_visitor.hpp"

bool CheckFieldNames::VisitFieldDecl(FieldDecl* FD) {

  /* For reporting diagnostic information */
  clang::DiagnosticsEngine &DE = Context->getDiagnostics();
  std::string name = FD->getNameAsString();
  auto it = begin(name);

  /* Are there any uppercase characters in the name? */
  bool FoundUpper = false;
  while (it != end(name))
    if (std::isupper(*it++)) {
      FoundUpper=true; 
      break;
    }

  /* Does the name end with an underscore? */
  bool FoundTrailingUnderscore = false;
  if (name[name.size()-1] == '_')
    FoundTrailingUnderscore = true;

  /* This name is permissible, we can return now. */
  if (!FoundUpper and FoundTrailingUnderscore)
    return true;

  /* Stringstream for the suggested name of the member */
  std::stringstream ss;
  it = begin(name);

  /* Always lowercase the first letter */
  ss << (char)std::tolower(*it++);

  /* Replace `_name` with `Name` */
  while (it != end(name)) {
    if (std::isupper(*it)) {
      ss << '_' << (char)std::tolower(*it++);
    } else {
      ss << (char)*it++;
    }
  }

  /* Always end member variables with an underscore */
  std::string Suggestion = ss.str();
  if (Suggestion[Suggestion.size()-1] != '_')
    Suggestion += "_";

  auto const Hint = FixItHint::CreateReplacement(FD->getLocation(),
      Suggestion);

  auto ID = DE.getCustomDiagID(clang::DiagnosticsEngine::Warning,
      "Found non-snake case class/struct/union member variable name. Please"
      " consider renaming.");
  DE.Report(FD->getBeginLoc(), ID).AddFixItHint(Hint);
  return true;
}
