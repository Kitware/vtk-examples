#include <vtkArrayDispatch.h>
#include <vtkDataArrayAccessor.h>
#include <vtkDataArrayRange.h>
#include <vtkNew.h>
#include <vtkTypeFloat64Array.h>

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace {

// Using naive way to go through the array.
void naivemag3(vtkDataArray* vectors, vtkDataArray* magnitudes);

// GetVoidPointer, mag3GetPointer call mag3Trampoline then call mag3Helper
void mag3GetPointer(vtkDataArray* vecs, vtkDataArray* mags);

template <typename VecType>
void mag3Trampoline(VecType* vecs, vtkDataArray* mags, vtkIdType numTuples);

template <typename VecType, typename MagType>
void mag3Helper(VecType* vecs, MagType* mags, vtkIdType numTuples);

// mag3 explicit types
template <typename ArrayT1, typename ArrayT2>
void mag3Explicit(ArrayT1* vectors, ArrayT2* magnitudes);

struct Mag3Worker1
{
  template <typename VecArray, typename MagArray>
  void operator()(VecArray* vecs, MagArray* mags)
  {
    // The Accessor types:
    using VecAccess = vtkDataArrayAccessor<VecArray>;
    using MagAccess = vtkDataArrayAccessor<MagArray>;

    // The "APITypes"
    // (explicit-type when possible, double for plain vtkDataArrays)
    using VecType = typename VecAccess::APIType;
    using MagType = typename MagAccess::APIType;

    // Tell the compiler the tuple sizes to enable optimizations:
    VTK_ASSUME(vecs->GetNumberOfComponents() == 3);
    VTK_ASSUME(mags->GetNumberOfComponents() == 1);

    const vtkIdType numTuples = vecs->GetNumberOfTuples();

    VecAccess vecAccess{vecs};
    MagAccess magAccess{mags};

    for (vtkIdType t = 0; t < numTuples; ++t)
    {
      MagType mag = 0;
      for (int c = 0; c < 3; ++c)
      {
        VecType comp = vecAccess.Get(t, c);
        auto cc = static_cast<MagType>(comp);
        mag += cc * cc;
      }
      mag = std::sqrt(mag);
      magAccess.Set(t, 0, mag);
    }
  }
};

struct Mag3Worker2a
{
  template <typename VecArray, typename MagArray>
  void operator()(VecArray* vecs, MagArray* mags)
  {
    // Create range objects:
    // Refer to this:
    // https://vtk.org/doc/nightly/html/classvtkArrayDispatch.html
    const auto vecRange = vtk::DataArrayTupleRange<3>(vecs);
    auto magRange = vtk::DataArrayValueRange<1>(mags);

    using VecType = typename decltype(vecRange)::ComponentType;
    using MagType = typename decltype(magRange)::ValueType;

    auto magIter = magRange.begin();
    for (const auto& vecTuple : vecRange)
    {
      MagType mag = 0;
      for (const VecType comp : vecTuple)
      {
        auto c = static_cast<MagType>(comp);
        mag += c * c;
      }
      *magIter = std::sqrt(mag);
      magIter++;
    }
  }
};

/**
 * This is similar to MagWorker2a but demonstrates the use of ReferenceType and
 * ConstReferenceType.
 *
 * Also elements in the range are accessed using operator[]
 * like an stl indexed container.
 *
 */
struct Mag3Worker2b
{
  template <typename VecArray, typename MagArray>
  void operator()(VecArray* vecs, MagArray* mags)
  {
    // Create range objects:
    const auto vecRange = vtk::DataArrayTupleRange<3>(vecs);
    auto magRange = vtk::DataArrayValueRange<1>(mags);

    using VecConstTupleRef =
        typename decltype(vecRange)::ConstTupleReferenceType;
    using VecConstCompRef =
        typename decltype(vecRange)::ConstComponentReferenceType;
    using MagRef = typename decltype(magRange)::ReferenceType;
    using MagType = typename decltype(magRange)::ValueType;

    for (vtkIdType id = 0; id < vecRange.size(); ++id)
    {
      MagRef magRef = magRange[id] = 0;
      VecConstTupleRef vecTuple = vecRange[id];
      for (VecConstCompRef&& comp : vecTuple)
      {
        auto c = static_cast<MagType>(comp);
        magRef += c * c;
      }
      magRef = std::sqrt(magRef);
    }
  }
};

/**
 * Here we create a functor for use by std::transform.
 *
 */
struct Mag3Worker3
{
  template <typename VecArray, typename MagArray>
  void operator()(VecArray* vecs, MagArray* mags)
  {
    // Create range objects:
    const auto vecRange = vtk::DataArrayTupleRange<3>(vecs);
    auto magRange = vtk::DataArrayValueRange<1>(mags);

    using VecConstTupleRef =
        typename decltype(vecRange)::ConstTupleReferenceType;
    using MagType = typename decltype(magRange)::ValueType;

    // Per-tuple magnitude functor for std::transform:
    auto computeMag = [](VecConstTupleRef tuple) -> MagType {
      MagType mag = 0;
      for (const auto& comp : tuple)
      {
        auto c = static_cast<MagType>(comp);
        mag += c * c;
      }
      return std::sqrt(mag);
    };

    std::transform(vecRange.cbegin(), vecRange.cend(), magRange.begin(),
                   computeMag);
  }
};

void mag3Dispatch1(vtkDataArray* vecs, vtkDataArray* mags);

void mag3Dispatch2a(vtkDataArray* vecs, vtkDataArray* mags);

void mag3Dispatch2b(vtkDataArray* vecs, vtkDataArray* mags);

void mag3Dispatch3(vtkDataArray* vecs, vtkDataArray* mags);

bool verifyResults(vtkDataArray* magnitudes,
                   std::vector<std::string> const& expectedMagnitudes);

} // namespace

int main(int argc, char* argv[])
{
  // The data and the result arrays.
  // Of course you can use:
  // vtkNew<vtkDoubleArray> darray;
  // vtkNew<vtkDoubleArray> results;
  // However, lets use the VTK templated data types:
  vtkNew<vtkTypeFloat64Array> darray;
  vtkNew<vtkTypeFloat64Array> results;

  // The number of components must be set in advance.
  darray->SetNumberOfComponents(3);
  results->SetNumberOfComponents(1);

  int TupleNum = 10;

  darray->SetNumberOfTuples(TupleNum);
  results->SetNumberOfTuples(TupleNum);

  auto printTuple = [](const std::array<vtkTypeFloat64, 3> tuple) {
    std::ostringstream os;
    auto separator = "";
    auto const sep = ", ";
    for (const auto& t : tuple)
    {
      os.setf(ios::fixed, ios::floatfield);
      os << std::setprecision(1) << separator << t;
      separator = sep;
    }
    return os.str();
  };

  for (vtkIdType i = 0; i < TupleNum; i++)
  {
    std::array<vtkTypeFloat64, 3> tuple = {{i * 0.1, i * 0.2, i * 0.3}};
    // std::cout << printTuple(tuple) << std::endl;

    // If the number of tuples is not set in advance, we can use InsertTuple.
    // darray->InsertTuple(i, tuple.data());
    darray->SetTuple(i, tuple.data());
  }

  // Set up for testing.
  std::vector<std::string> expectedMagnitudes{
      "0.000000", "0.374166", "0.748331", "1.122497", "1.496663",
      "1.870829", "2.244994", "2.619160", "2.993326", "3.367492"};
  std::string failMessage =
      "---         Fail: Results don't match expected values.";
  auto exitValue = EXIT_SUCCESS;

  // We could explicitly specify the capture list:
  //  [&exitValue, &results, &expectedMagnitudes, &failMessage]
  //  instead of just [&].
  auto checkResult = [&]() {
    if (!verifyResults(results, expectedMagnitudes))
    {
      std::cout << failMessage << std::endl;
      exitValue = EXIT_FAILURE;
    }
  };

  auto resetResults = [&]() {
    for (vtkIdType i = 0; i < TupleNum; i++)
    {
      vtkTypeFloat64 v = 0;
      results->SetTuple(i, &v);
    }
  };

  // Using naive API.
  naivemag3(darray, results);
  checkResult();

  // Reset results to zero.
  resetResults();

  // Using get raw pointer.
  mag3GetPointer(darray, results);
  checkResult();

  // Reset results to zero.
  resetResults();

  // Instantiate with explicit type.
  mag3Explicit<vtkTypeFloat64Array, vtkTypeFloat64Array>(darray, results);
  checkResult();

  // Worker and dispatcher, there are four different types of worker.
  mag3Dispatch1(darray, results);
  checkResult();

  mag3Dispatch2a(darray, results);
  checkResult();

  mag3Dispatch2b(darray, results);
  checkResult();

  mag3Dispatch3(darray, results);
  checkResult();

  if (exitValue == EXIT_SUCCESS)
  {
    std::cout << "All tests passed." << std::endl;
  }
  else
  {
    std::cout << "Some tests failed." << std::endl;
  }
  return exitValue;
}

namespace {

void naivemag3(vtkDataArray* vectors, vtkDataArray* magnitudes)
{
  std::cout << "--- Testing naivemag3" << std::endl;
  const vtkIdType numTuples = vectors->GetNumberOfTuples();
  std::array<vtkTypeFloat64, 3> tuple;
  for (vtkIdType t = 0; t < numTuples; ++t)
  {
    vectors->GetTuple(t, tuple.data());
    vtkTypeFloat64 mag = 0.;
    for (vtkTypeFloat64 comp : tuple)
    {
      // std::cout << "comp " << comp << std::endl;
      mag += comp * comp;
    }
    mag = std::sqrt(mag);
    // Assume that space is allocated.
    magnitudes->SetTuple(t, &mag);
  }
}

// GetVoidPointer, mag3GetPointer call mag3Trampoline then call mag3Helper
void mag3GetPointer(vtkDataArray* vecs, vtkDataArray* mags)
{
  std::cout << "--- Testing mag3GetPointer" << std::endl;
  const vtkIdType numTuples = vecs->GetNumberOfTuples();
  // Resolve vecs data type:
  switch (vecs->GetDataType())
  {
    vtkTemplateMacro(mag3Trampoline(
        static_cast<VTK_TT*>(vecs->GetVoidPointer(0)), mags, numTuples));
  default:
    std::cout << "error at mag3GetPointer" << std::endl;
  }
}

template <typename VecType>
void mag3Trampoline(VecType* vecs, vtkDataArray* mags, vtkIdType numTuples)
{
  // Resolve mags data type:
  switch (mags->GetDataType())
  {
    vtkTemplateMacro(mag3Helper(
        vecs, static_cast<VTK_TT*>(mags->GetVoidPointer(0)), numTuples));
  default:
    std::cout << "error at mag3Trampoline" << std::endl;
  }
}

template <typename VecType, typename MagType>
void mag3Helper(VecType* vecs, MagType* mags, vtkIdType numTuples)
{
  for (vtkIdType t = 0; t < numTuples; ++t)
  {
    MagType mag = 0;
    for (size_t i = 0; i < 3; ++i)
    {
      auto v = static_cast<MagType>(*vecs);
      mag += v * v;
      ++vecs;
    }
    *mags = std::sqrt(mag);
    ++mags;
  }
}

template <typename ArrayT1, typename ArrayT2>
void mag3Explicit(ArrayT1* vectors, ArrayT2* magnitudes)

{
  std::cout << "--- Testing mag3Explicit" << std::endl;
  using VecType = typename ArrayT1::ValueType;
  using MagType = typename ArrayT2::ValueType;

  const vtkIdType numTuples = vectors->GetNumberOfTuples();

  for (vtkIdType t = 0; t < numTuples; ++t)
  {
    MagType mag = 0;
    for (int c = 0; c < 3; ++c)
    {
      VecType comp = vectors->GetTypedComponent(t, c);
      mag += static_cast<MagType>(comp * comp);
    }
    mag = std::sqrt(mag);
    magnitudes->SetTypedComponent(t, 0, mag);
  }
}

void mag3Dispatch1(vtkDataArray* vecs, vtkDataArray* mags)
{
  std::cout << "--- Testing mag3Dispatch1" << std::endl;
  Mag3Worker1 worker1;

  // Create a dispatcher. We want to generate fast-paths for when
  // vecs and mags both use doubles or floats, but fallback to a
  // slow path for any other situation.
  using Dispatcher =
      vtkArrayDispatch::Dispatch2ByValueType<vtkArrayDispatch::Reals,
                                             vtkArrayDispatch::Reals>;

  if (!Dispatcher::Execute(vecs, mags, worker1))
  {
    // Otherwise fallback to using the vtkDataArray API.
    worker1(vecs, mags);
  }
}

void mag3Dispatch2a(vtkDataArray* vecs, vtkDataArray* mags)
{
  std::cout << "--- Testing mag3Dispatch2a" << std::endl;
  Mag3Worker2a worker2a;

  // Create a dispatcher. We want to generate fast-paths for when
  // vecs and mags both use doubles or floats, but fallback to a
  // slow path for any other situation.
  using Dispatcher =
      vtkArrayDispatch::Dispatch2ByValueType<vtkArrayDispatch::Reals,
                                             vtkArrayDispatch::Reals>;

  // Generate optimized workers when mags/vecs are both float|double
  if (!Dispatcher::Execute(vecs, mags, worker2a))
  {
    // Otherwise fallback to using the vtkDataArray API.
    worker2a(vecs, mags);
  }
}

void mag3Dispatch2b(vtkDataArray* vecs, vtkDataArray* mags)
{
  std::cout << "--- Testing mag3Dispatch2b" << std::endl;
  Mag3Worker2b worker2b;

  // Create a dispatcher. We want to generate fast-paths for when
  // vecs and mags both use doubles or floats, but fallback to a
  // slow path for any other situation.
  using Dispatcher =
      vtkArrayDispatch::Dispatch2ByValueType<vtkArrayDispatch::Reals,
                                             vtkArrayDispatch::Reals>;

  // Generate optimized workers when mags/vecs are both float|double
  if (!Dispatcher::Execute(vecs, mags, worker2b))
  {
    // Otherwise fallback to using the vtkDataArray API.
    worker2b(vecs, mags);
  }
}

void mag3Dispatch3(vtkDataArray* vecs, vtkDataArray* mags)
{
  std::cout << "--- Testing mag3Dispatch3" << std::endl;
  Mag3Worker3 worker3;

  // Create a dispatcher. We want to generate fast-paths for when
  // vecs and mags both use doubles or floats, but fallback to a
  // slow path for any other situation.
  using Dispatcher =
      vtkArrayDispatch::Dispatch2ByValueType<vtkArrayDispatch::Reals,
                                             vtkArrayDispatch::Reals>;

  // Generate optimized workers when mags/vecs are both float|double
  if (!Dispatcher::Execute(vecs, mags, worker3))
  {
    // Otherwise fallback to using the vtkDataArray API.
    worker3(vecs, mags);
  }
}

bool verifyResults(vtkDataArray* magnitudes,
                   std::vector<std::string> const& expectedMagnitudes)
{
  std::vector<std::string> actualMagnitudes;
  auto magRange = vtk::DataArrayValueRange<1>(magnitudes);
  for (const auto& magTuple : magRange)
  {
    std::ostringstream os;
    os.setf(ios::fixed, ios::floatfield);
    os << std::setprecision(6) << magTuple;
    actualMagnitudes.push_back(os.str());
  }

  return (std::equal(actualMagnitudes.begin(), actualMagnitudes.end(),
                     expectedMagnitudes.begin()));
}

} // namespace
