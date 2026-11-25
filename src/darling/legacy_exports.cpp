//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// DARLING: symbols that libc++.1.dylib exported up to LLVM 15 and that later
// releases no longer define, because their headers stopped referring to them.
// Binaries built against the older headers still bind to them, so the dylib
// keeps exporting them. Their names are given with asm labels, because the
// current headers declare the same entities differently (as inline variables,
// or as templates with other parameters).

#include <__config>
#include <cstddef>
#include <utility>

#define DARLING_LEGACY_EXPORT __attribute__((__visibility__("default"), __used__))

// The tag types are empty; each of their objects occupies one byte. (The type
// has external linkage, so that the objects can have it too.)
namespace darling_legacy {
struct tag {};
} // namespace darling_legacy

// The tag objects that the headers now define as inline constexpr variables.
#define DARLING_LEGACY_TAG(name, symbol)                                                                               \
  extern const darling_legacy::tag name __asm__(symbol);                                                                        \
  DARLING_LEGACY_EXPORT const darling_legacy::tag name = {};

DARLING_LEGACY_TAG(legacy_adopt_lock, "__ZNSt3__110adopt_lockE")
DARLING_LEGACY_TAG(legacy_defer_lock, "__ZNSt3__110defer_lockE")
DARLING_LEGACY_TAG(legacy_try_to_lock, "__ZNSt3__111try_to_lockE")
DARLING_LEGACY_TAG(legacy_allocator_arg, "__ZNSt3__113allocator_argE")
DARLING_LEGACY_TAG(legacy_piecewise_construct, "__ZNSt3__119piecewise_constructE")

// std::__insertion_sort_incomplete<__less<T, T>&, T*>(T*, T*, __less<T, T>&)
// and std::__sort5<__less<long double, long double>&, long double*>, which the
// LLVM 15 std::sort declared as extern templates. The comparator is the
// stateless std::__less, passed by reference, so only operator< is used.
//
// __insertion_sort_incomplete sorts [first, last) by insertion, but gives up
// after moving eight elements; it returns whether the range is sorted.
// __sort5 sorts five elements and returns the number of swaps it made.
namespace {

template <class T>
unsigned sort3(T* a, T* b, T* c) {
  unsigned swaps = 0;
  if (*b < *a) {
    std::swap(*a, *b);
    ++swaps;
  }
  if (*c < *b) {
    std::swap(*b, *c);
    ++swaps;
    if (*b < *a) {
      std::swap(*a, *b);
      ++swaps;
    }
  }
  return swaps;
}

template <class T>
unsigned sort4(T* a, T* b, T* c, T* d) {
  unsigned swaps = sort3(a, b, c);
  if (*d < *c) {
    std::swap(*c, *d);
    ++swaps;
    if (*c < *b) {
      std::swap(*b, *c);
      ++swaps;
      if (*b < *a) {
        std::swap(*a, *b);
        ++swaps;
      }
    }
  }
  return swaps;
}

template <class T>
unsigned sort5(T* a, T* b, T* c, T* d, T* e) {
  unsigned swaps = sort4(a, b, c, d);
  if (*e < *d) {
    std::swap(*d, *e);
    ++swaps;
    if (*d < *c) {
      std::swap(*c, *d);
      ++swaps;
      if (*c < *b) {
        std::swap(*b, *c);
        ++swaps;
        if (*b < *a) {
          std::swap(*a, *b);
          ++swaps;
        }
      }
    }
  }
  return swaps;
}

template <class T>
bool insertion_sort_incomplete(T* first, T* last) {
  switch (last - first) {
  case 0:
  case 1:
    return true;
  case 2:
    if (first[1] < first[0])
      std::swap(first[0], first[1]);
    return true;
  case 3:
    sort3(first, first + 1, first + 2);
    return true;
  case 4:
    sort4(first, first + 1, first + 2, first + 3);
    return true;
  case 5:
    sort5(first, first + 1, first + 2, first + 3, first + 4);
    return true;
  }

  sort3(first, first + 1, first + 2);
  const unsigned limit = 8;
  unsigned moved = 0;
  for (T* i = first + 3; i != last; ++i) {
    if (*i < i[-1]) {
      T value(std::move(*i));
      T* hole = i;
      do {
        *hole = std::move(hole[-1]);
        --hole;
      } while (hole != first && value < hole[-1]);
      *hole = std::move(value);
      if (++moved == limit)
        return i + 1 == last;
    }
  }
  return true;
}

} // namespace

#define DARLING_LEGACY_INSERTION_SORT(T, symbol)                                                                       \
  DARLING_LEGACY_EXPORT bool legacy_insertion_sort_incomplete_##symbol(T* first, T* last, const void*) __asm__(        \
      "__ZNSt3__127__insertion_sort_incompleteIRNS_6__lessI" #symbol #symbol "EEP" #symbol "EEbT0_S5_T_");             \
  bool legacy_insertion_sort_incomplete_##symbol(T* first, T* last, const void*) {                                     \
    return insertion_sort_incomplete(first, last);                                                                     \
  }

DARLING_LEGACY_INSERTION_SORT(char, c)
DARLING_LEGACY_INSERTION_SORT(wchar_t, w)
DARLING_LEGACY_INSERTION_SORT(signed char, a)
DARLING_LEGACY_INSERTION_SORT(unsigned char, h)
DARLING_LEGACY_INSERTION_SORT(short, s)
DARLING_LEGACY_INSERTION_SORT(unsigned short, t)
DARLING_LEGACY_INSERTION_SORT(int, i)
DARLING_LEGACY_INSERTION_SORT(unsigned int, j)
DARLING_LEGACY_INSERTION_SORT(long, l)
DARLING_LEGACY_INSERTION_SORT(unsigned long, m)
DARLING_LEGACY_INSERTION_SORT(long long, x)
DARLING_LEGACY_INSERTION_SORT(unsigned long long, y)
DARLING_LEGACY_INSERTION_SORT(float, f)
DARLING_LEGACY_INSERTION_SORT(double, d)
DARLING_LEGACY_INSERTION_SORT(long double, e)

DARLING_LEGACY_EXPORT unsigned
legacy_sort5_long_double(long double* a, long double* b, long double* c, long double* d, long double* e, const void*)
    __asm__("__ZNSt3__17__sort5IRNS_6__lessIeeEEPeEEjT0_S5_S5_S5_S5_T_");
unsigned
legacy_sort5_long_double(long double* a, long double* b, long double* c, long double* d, long double* e, const void*) {
  return sort5(a, b, c, d, e);
}
