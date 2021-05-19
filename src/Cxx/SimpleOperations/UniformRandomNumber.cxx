#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNew.h>

#include <time.h>

int main(int, char*[])
{
  // Set the number of random numbers we wish to produce to 3.
  unsigned int numRand = 3;

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(8775070);
  // Without this line, the random numbers will be the same every iteration.
  // randomSequence->SetSeed(time(NULL));

  // Generate numRand random numbers from a uniform distribution between 0.0
  // and 2.0
  for (unsigned int i = 0; i < numRand; i++)
  {
    auto a = randomSequence->GetRangeValue(0.0, 2.0);
    randomSequence->Next();

    std::cout << a << std::endl;
  }

  return EXIT_SUCCESS;
}
