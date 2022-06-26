//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <string>

#include <cassert>
#include <string>

struct Incomplete;
template<class T> struct Holder { T t; };

template<class T>
struct Charlike {
    char ch_;
    Charlike(char ch) : ch_(ch) {}
    operator char() const { return ch_; }
};

int main(int, char**)
{
    std::string s;
    Charlike<Holder<Incomplete> > a[] = {'m', 'a', 'h', 'i'};
    s.append(a, a+4);
    s.assign(a, a+4);
    s.insert(s.begin(), a, a+4);
    s.replace(s.begin(), s.begin()+4, a, a+4);
    assert(s == "mahimahi");

    return 0;
}
