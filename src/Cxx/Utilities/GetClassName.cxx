#include <typeinfo>
#include <vtkNew.h>
#include <vtkPoints.h>

namespace {
static const char* unmangleName(const char* name);
}

int main(int, char*[])
{
  vtkNew<vtkPoints> points;

  std::cout << "points->GetClassName():             " << points->GetClassName()
            << std::endl;
  std::cout << "Mangled" << std::endl;
  std::cout << "\ttypeid(points).name():              " << typeid(points).name()
            << std::endl;
  std::cout << "\ttypeid(points.GetPointer()).name(): "
            << typeid(points.GetPointer()).name() << std::endl;
  std::cout << "Unmangled" << std::endl;
  std::cout << "\ttypeid(points.GetPointer()).name(): "
            << unmangleName(typeid(points).name()) << std::endl;
  std::cout << "\ttypeid(points.GetPointer()).name(): "
            << unmangleName(typeid(points.GetPointer()).name()) << std::endl;
  return EXIT_SUCCESS;
}

namespace {
// Better name demangling for gcc
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ > 0)
#define GCC_USEDEMANGLE
#endif

#ifdef GCC_USEDEMANGLE
#include <cstdlib>
#include <cxxabi.h>
#endif

const char* unmangleName(const char* name)
{
#ifdef GCC_USEDEMANGLE
  char const* mangledName = name;
  int status;
  char* unmangled = abi::__cxa_demangle(mangledName, 0, 0, &status);
  return unmangled;
#else
  return name;
#endif
}
} // namespace
