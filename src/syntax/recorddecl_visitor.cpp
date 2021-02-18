#include "recorddecl_visitor.hpp"
#include <iterator>
using std::optional;
using std::nullopt;

bool CheckPascalCaseClassNames::VisitCXXRecordDecl(CXXRecordDecl* RD) {
  /* For reporting diagnostic information */
  clang::DiagnosticsEngine &DE = Context->getDiagnostics();
  bool ApplyStripPrefix = false;

  /* Check for prefix to strip */
  if (StripPrefix != "") {
    std::string name = RD->getNameAsString();
    if (name.find_first_of(StripPrefix) == 0) {
      ApplyStripPrefix = true;
    }
  }

  /* Check pascal case for class name */
  if(std::islower(RD->getNameAsString()[0])) {
    std::string name = RD->getNameAsString();
    std::stringstream ss;
    auto it = begin(name);

    /* If we're stripping a prefix from the name, we want to increase the
     * iterator so not to include the prefix. */
    if (ApplyStripPrefix)
      std::advance(it, StripPrefix.size());

    /* Always uppercase the first letter */
    ss << (char)std::toupper(*it++);

    /* Replace *_name with *Name */
    while (it != end(name)) {
      if (*it == '_') {
        it++;
        if (it == end(name)) {
          name = "When checking name " + name + ", found trailing underscore!";
          char warning[256];
          strcpy(warning, name.c_str());
          auto ID = DE.getCustomDiagID(clang::DiagnosticsEngine::Error,
              warning);
          DE.Report(RD->getBeginLoc(), ID);
          return true;
        }
        ss << (char)std::toupper(*it++);
      } else {
        ss << (char)*it++;
      }
    }

    auto const Hint = FixItHint::CreateReplacement(RD->getLocation(), ss.str());

    auto ID = DE.getCustomDiagID(clang::DiagnosticsEngine::Warning,
        "Found non-pascal case class/struct/union name. Please consider "
        "renaming.");
    DE.Report(RD->getBeginLoc(), ID).AddFixItHint(Hint);
  }
  return true;
}
