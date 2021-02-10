struct CaptureTest {

  /* Should not capture */
  int *i;

  /* Should not capture */
  int j[1];

  /* OK to capture */
  int k=0;

  /* Method which implicitly captures `this` pointer and modifies member
   * variable `i`. This is problematic when using portability libraries, as
   * member variables may not reside on the host. */
  void methodUsingBadCapturePointer() {
    auto throwaway = [=] () {
      *i = 1;
    };
  }

  /* Raw arrays should not be used either. */
  void methodUsingBadCaptureArray() {
    auto throwaway = [=] () {
      j[0] = 1;
    };
  }

  /* The preferred method to mitigate the issue outlined above is to create a
   * local copy of the pointer and modify the underlying data through the copy.
   */
  void methodUsingGoodCapture() {
    int* localCopy = i;
    auto throwaway = [=] () {
      *localCopy += 1;
    };
  }

  /* Methods which capture `this` variables which are not pointers should not
   * cause an issue. */
  void methodNotCapturingPointer() {
    auto throwaway = [=] () {
      k++;
    };
  }
};

int main() {
  int a = 3;
  CaptureTest ct{&a};
  return 0;
}
