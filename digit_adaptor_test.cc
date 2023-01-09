// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#include "digit_adaptor.hh"

#include <cstdint>
#include <iostream>
#include <utility>

namespace {

using jz::digit_adaptor;

// This code would benefit from a proper unit-test framework.  I have not
// included one here to keep this repository free of external dependencies. 
// Instead, I've just produced a very minimal framework.
using TestCaseFxn = bool(void);

struct TestCase {
  const char  *name;
  TestCaseFxn *test;
};

// Tests that we can read decimal digits via a digit-adaptor on a signed int
// that's positive.
bool TestReadingDecimalDigitsFromPositiveSignedInt() {
  const int x = 12345;
  const digit_adaptor<const int> d{x};

  if (d.size() != 5) { return false; }

  if (d[0] != 1) { return false; }
  if (d[1] != 2) { return false; }
  if (d[2] != 3) { return false; }
  if (d[3] != 4) { return false; }
  if (d[4] != 5) { return false; }

  return true;
}

// Tests that we can read decimal digits via a digit-adaptor on a signed int
// that's negative.
bool TestReadingDecimalDigitsFromNegativeSignedInt() {
  const int x = -12345;
  const digit_adaptor<const int> d{x};

  if (d.size() != 5) { return false; }

  if (d[0] != 1) { return false; }
  if (d[1] != 2) { return false; }
  if (d[2] != 3) { return false; }
  if (d[3] != 4) { return false; }
  if (d[4] != 5) { return false; }

  return true;
}

// Tests that we can read decimal digits via a digit-adaptor on an unsigned int.
bool TestReadingDecimalDigitsFromUnsignedInt() {
  const unsigned int x = 12345;
  const digit_adaptor<const unsigned int> d{x};

  if (d.size() != 5) { return false; }

  if (d[0] != 1) { return false; }
  if (d[1] != 2) { return false; }
  if (d[2] != 3) { return false; }
  if (d[3] != 4) { return false; }
  if (d[4] != 5) { return false; }

  return true;
}

// Tests that we can read hex digits via a digit-adaptor on a signed int that's
// positive.
bool TestReadingHexDigitsFromPositiveSignedInt() {
  const int x = 0x12345;
  const digit_adaptor<const int, 16> d{x};

  if (d.size() != 5) { return false; }

  if (d[0] != 1) { return false; }
  if (d[1] != 2) { return false; }
  if (d[2] != 3) { return false; }
  if (d[3] != 4) { return false; }
  if (d[4] != 5) { return false; }

  return true;
}

// Tests that we can read hex digits via a digit-adaptor on a signed int that's
// negative.
bool TestReadingHexDigitsFromNegativeSignedInt() {
  const int x = -0x12345;
  const digit_adaptor<const int, 16> d{x};

  if (d.size() != 5) { return false; }

  if (d[0] != 1) { return false; }
  if (d[1] != 2) { return false; }
  if (d[2] != 3) { return false; }
  if (d[3] != 4) { return false; }
  if (d[4] != 5) { return false; }

  return true;
}

// Tests that we can read hex digits via a digit-adaptor on an unsigned int.
bool TestReadingHexDigitsFromUnsignedInt() {
  const unsigned int x = 0x12345;
  const digit_adaptor<const unsigned int, 16> d{x};

  if (d.size() != 5) { return false; }

  if (d[0] != 1) { return false; }
  if (d[1] != 2) { return false; }
  if (d[2] != 3) { return false; }
  if (d[3] != 4) { return false; }
  if (d[4] != 5) { return false; }

  return true;
}

// Tests that we can read octal digits via a digit-adaptor on a signed int
// that's positive.
bool TestReadingOctalDigitsFromPositiveSignedInt() {
  const int x = 012345;
  const digit_adaptor<const int, 8> d{x};

  if (d.size() != 5) { return false; }

  if (d[0] != 1) { return false; }
  if (d[1] != 2) { return false; }
  if (d[2] != 3) { return false; }
  if (d[3] != 4) { return false; }
  if (d[4] != 5) { return false; }

  return true;
}

// Tests that we can read octal digits via a digit-adaptor on a signed int
// that's negative.
bool TestReadingOctalDigitsFromNegativeSignedInt() {
  const int x = -012345;
  const digit_adaptor<const int, 8> d{x};

  if (d.size() != 5) { return false; }

  if (d[0] != 1) { return false; }
  if (d[1] != 2) { return false; }
  if (d[2] != 3) { return false; }
  if (d[3] != 4) { return false; }
  if (d[4] != 5) { return false; }

  return true;
}

// Tests that we can read octal digits via a digit-adaptor on an unsigned int.
bool TestReadingOctalDigitsFromUnsignedInt() {
  const unsigned int x = 0x12345;
  const digit_adaptor<const unsigned int, 16> d{x};

  if (d.size() != 5) { return false; }

  if (d[0] != 1) { return false; }
  if (d[1] != 2) { return false; }
  if (d[2] != 3) { return false; }
  if (d[3] != 4) { return false; }
  if (d[4] != 5) { return false; }

  return true;
}

// Tests that we can write decimal digits via a digit-adaptor on a signed long
// that's positive.
bool TestWritingDecimalDigitsToPositiveSignedLong() {
  long x = 12345L;
  digit_adaptor<long> d{x};

  if (d.size() != 5) { return false; }
  d[0] = 6;
  if (x != 62345L) { return false; }
  d[1] = 7;
  if (x != 67345L) { return false; }
  d[2] = 8;
  if (x != 67845L) { return false; }
  d[3] = 9;
  if (x != 67895L) { return false; }
  d[4] = 0;
  if (x != 67890L) { return false; }

  return true;
}

// Tests that we can write decimal digits via a digit-adaptor on a signed long
// that's negative.
bool TestWritingDecimalDigitsToNegativeSignedLong() {
  long x = -12345L;
  digit_adaptor<long> d{x};

  if (d.size() != 5) { return false; }

  d[0] = 6;
  if (x != -62345L) { return false; }
  d[1] = 7;
  if (x != -67345L) { return false; }
  d[2] = 8;
  if (x != -67845L) { return false; }
  d[3] = 9;
  if (x != -67895L) { return false; }
  d[4] = 0;
  if (x != -67890L) { return false; }

  return true;
}

// Tests that we can write decimal digits via a digit-adaptor on an unsigned
// long.
bool TestWritingDecimalDigitsToUnsignedLong() {
  unsigned long x = 12345L;
  digit_adaptor<unsigned long> d{x};

  if (d.size() != 5) { return false; }

  d[0] = 6;
  if (x != 62345UL) { return false; }
  d[1] = 7;
  if (x != 67345UL) { return false; }
  d[2] = 8;
  if (x != 67845UL) { return false; }
  d[3] = 9;
  if (x != 67895UL) { return false; }
  d[4] = 0;
  if (x != 67890UL) { return false; }

  return true;
}

// Tests that we can write hex digits via a digit-adaptor on a signed long
// that's positive.
bool TestWritingHexDigitsToPositiveSignedLong() {
  long x = 0x12345L;
  digit_adaptor<long, 16> d{x};

  if (d.size() != 5) { return false; }

  d[0] = 6;
  if (x != 0x62345L) { return false; }
  d[1] = 7;
  if (x != 0x67345L) { return false; }
  d[2] = 8;
  if (x != 0x67845L) { return false; }
  d[3] = 9;
  if (x != 0x67895L) { return false; }
  d[4] = 0;
  if (x != 0x67890L) { return false; }

  return true;
}

// Tests that we can write hex digits via a digit-adaptor on a signed long
// that's negative.
bool TestWritingHexDigitsToNegativeSignedLong() {
  long x = -0x12345L;
  digit_adaptor<long, 16> d{x};

  if (d.size() != 5) { return false; }
  d[0] = 6;
  if (x != -0x62345L) { return false; }
  d[1] = 7;
  if (x != -0x67345L) { return false; }
  d[2] = 8;
  if (x != -0x67845L) { return false; }
  d[3] = 9;
  if (x != -0x67895L) { return false; }
  d[4] = 0;
  if (x != -0x67890L) { return false; }

  return true;
}

// Tests that we can write hex digits via a digit-adaptor on an unsigned
// long.
bool TestWritingHexDigitsToUnsignedLong() {
  unsigned long x = 0x12345UL;
  digit_adaptor<unsigned long, 16> d{x};

  if (d.size() != 5) { return false; }

  d[0] = 6;
  if (x != 0x62345UL) { return false; }
  d[1] = 7;
  if (x != 0x67345UL) { return false; }
  d[2] = 8;
  if (x != 0x67845UL) { return false; }
  d[3] = 9;
  if (x != 0x67895UL) { return false; }
  d[4] = 0;
  if (x != 0x67890UL) { return false; }

  return true;
}

// Tests that we can write octal digits via a digit-adaptor on a signed long
// that's positive.
bool TestWritingOctalDigitsToPositiveSignedLong() {
  long x = 012345;
  digit_adaptor<long, 8> d{x};

  if (d.size() != 5) { return false; }

  d[0] = 6;
  if (x != 062345L) { return false; }
  d[1] = 7;
  if (x != 067345L) { return false; }
  d[2] = 0;
  if (x != 067045L) { return false; }
  d[3] = 1;
  if (x != 067015L) { return false; }
  d[4] = 2;
  if (x != 067012L) { return false; }

  return true;
}

// Tests that we can write octal digits via a digit-adaptor on a signed long
// that's negative.
bool TestWritingOctalDigitsToNegativeSignedLong() {
  long x = -012345L;
  digit_adaptor<long, 8> d{x};

  if (d.size() != 5) { return false; }

  d[0] = 6;
  if (x != -062345L) { return false; }
  d[1] = 7;
  if (x != -067345L) { return false; }
  d[2] = 0;
  if (x != -067045L) { return false; }
  d[3] = 1;
  if (x != -067015L) { return false; }
  d[4] = 2;
  if (x != -067012L) { return false; }

  return true;
}

// Tests that we can write octal digits via a digit-adaptor on an unsigned
// long.
bool TestWritingOctalDigitsToUnsignedLong() {
  unsigned long x = 012345UL;
  digit_adaptor<unsigned long, 8> d{x};

  if (d.size() != 5) { return false; }

  d[0] = 6;
  if (x != 062345UL) { return false; }
  d[1] = 7;
  if (x != 067345UL) { return false; }
  d[2] = 0;
  if (x != 067045UL) { return false; }
  d[3] = 1;
  if (x != 067015UL) { return false; }
  d[4] = 2;
  if (x != 067012UL) { return false; }

  return true;
}

// Tests creating an adaptor on zero.
bool TestAdaptorOnZero() {
  int x = 0;
  digit_adaptor<int> d{x};

  if (d.size() != 1) { return false; }
  if (d[0] != 0)     { return false; }
  d[0] = 3;
  if (x != 3)        { return false; }

  return true;
}

// Tests creating an adaptor with extra digits.
bool TestAdaptorWithExtraDigits() {
  int x = 0;
  digit_adaptor<int> d{x, 5};

  if (d.size() != 5) { return false; }
  d[0] = 1;
  d[1] = 2;
  d[2] = 3;
  d[3] = 4;
  d[4] = 5;
  if (x != 12345) { return false; }

  return true;
}

// Tests whether we can set leading zeros, and recover after, with a positive
// initial value.
bool TestLeadingZerosWithPositiveInt() {
  int x = 12345;
  digit_adaptor<int> d{x};

  if (d.size() != 5) { return false; }
  d[0] = 0;
  if (x != 2345)  { return false; }
  d[1] = 0;      
  if (x != 345)   { return false; }
  d[2] = 0;      
  if (x != 45)    { return false; }
  d[3] = 0;      
  if (x != 5)     { return false; }
  d[4] = 0;      
  if (x != 0)     { return false; }
                 
  d[0] = 1;      
  if (x != 10000) { return false; }
  d[1] = 2;      
  if (x != 12000) { return false; }
  d[2] = 3;      
  if (x != 12300) { return false; }
  d[3] = 4;      
  if (x != 12340) { return false; }
  d[4] = 5;      
  if (x != 12345) { return false; }

  x = 0;
                 
  d[4] = 5;      
  if (x != 5)     { return false; }
  d[3] = 4;      
  if (x != 45)    { return false; }
  d[2] = 3;      
  if (x != 345)   { return false; }
  d[1] = 2;      
  if (x != 2345)  { return false; }
  d[0] = 1;      
  if (x != 12345) { return false; }

  return true;
}

// Tests whether we can set leading zeros, and recover after, with a negative
// initial value.  We can't go all the way to 0 without losing the sign.
bool TestLeadingZerosWithNegativeInt() {
  int x = -12345;
  digit_adaptor<int> d{x};

  if (d.size() != 5) { return false; }
  d[0] = 0;
  if (x != -2345)  { return false; }
  d[1] = 0;      
  if (x != -345)   { return false; }
  d[2] = 0;      
  if (x != -45)    { return false; }
  d[3] = 0;      
  if (x != -5)     { return false; }
                 
  d[0] = 1;      
  if (x != -10005) { return false; }
  d[1] = 2;      
  if (x != -12005) { return false; }
  d[2] = 3;      
  if (x != -12305) { return false; }
  d[3] = 4;      
  if (x != -12345) { return false; }

  x = -5;
                 
  d[3] = 4;      
  if (x != -45)    { return false; }
  d[2] = 3;      
  if (x != -345)   { return false; }
  d[1] = 2;      
  if (x != -2345)  { return false; }
  d[0] = 1;      
  if (x != -12345) { return false; }

  return true;
}

// Tests whether we can swap digits using swap().
bool TestSwappingDigits() {
  using std::swap;

  int x = 1234;
  digit_adaptor<int> d{x};

  if (d.size() != 4) { return false; }

  swap(d[0], d[3]);
  if (x != 4231 ) { return false; }
  swap(d[2], d[1]);
  if (x != 4321 ) { return false; }
  swap(d[0], d[1]);
  if (x != 3421 ) { return false; }
  swap(d[2], d[3]);
  if (x != 3412 ) { return false; }
  swap(d[0], d[2]);
  if (x != 1432 ) { return false; }
  swap(d[3], d[1]);
  if (x != 1234 ) { return false; }

  return true;
}

// Tests that we can read via forward iterators.
bool TestReadingViaForwardIterators() {
  int x = 12345;
  digit_adaptor<int> d{x};

  // Mutable iterator.
  auto mit = d.begin();
  if (std::distance(mit, d.end()) != 5) { return false; }
  if (*mit++ != 1)      { return false; }
  if (*mit++ != 2)      { return false; }
  if (*mit++ != 3)      { return false; }
  if (*mit++ != 4)      { return false; }
  if (*mit++ != 5)      { return false; }
  if (mit != d.end())   { return false; }
  if (*--mit != 5)      { return false; }
  if (*--mit != 4)      { return false; }
  if (*--mit != 3)      { return false; }
  if (*--mit != 2)      { return false; }
  if (*--mit != 1)      { return false; }
  if (mit != d.begin()) { return false; }

  // Const iterator.
  auto cit = d.cbegin();
  if (std::distance(cit, d.cend()) != 5) { return false; }
  if (*cit++ != 1)       { return false; }
  if (*cit++ != 2)       { return false; }
  if (*cit++ != 3)       { return false; }
  if (*cit++ != 4)       { return false; }
  if (*cit++ != 5)       { return false; }
  if (cit != d.cend())   { return false; }
  if (*--cit != 5)       { return false; }
  if (*--cit != 4)       { return false; }
  if (*--cit != 3)       { return false; }
  if (*--cit != 2)       { return false; }
  if (*--cit != 1)       { return false; }
  if (cit != d.cbegin()) { return false; }

  return true;
}

// Tests that we can read via reverse iterators.
bool TestReadingViaReverseIterators() {
  int x = 12345;
  digit_adaptor<int> d{x};

  // Mutable iterator.
  auto mit = d.rbegin();
  if (std::distance(mit, d.rend()) != 5) { return false; }
  if (*mit++ != 5)       { return false; }
  if (*mit++ != 4)       { return false; }
  if (*mit++ != 3)       { return false; }
  if (*mit++ != 2)       { return false; }
  if (*mit++ != 1)       { return false; }
  if (mit != d.rend())   { return false; }
  if (*--mit != 1)       { return false; }
  if (*--mit != 2)       { return false; }
  if (*--mit != 3)       { return false; }
  if (*--mit != 4)       { return false; }
  if (*--mit != 5)       { return false; }
  if (mit != d.rbegin()) { return false; }

  // Const iterator.
  auto cit = d.crbegin();
  if (std::distance(cit, d.crend()) != 5) { return false; }
  if (*cit++ != 5)        { return false; }
  if (*cit++ != 4)        { return false; }
  if (*cit++ != 3)        { return false; }
  if (*cit++ != 2)        { return false; }
  if (*cit++ != 1)        { return false; }
  if (cit != d.crend())   { return false; }
  if (*--cit != 1)        { return false; }
  if (*--cit != 2)        { return false; }
  if (*--cit != 3)        { return false; }
  if (*--cit != 4)        { return false; }
  if (*--cit != 5)        { return false; }
  if (cit != d.crbegin()) { return false; }

  return true;
}

// Tests sorting digits with std::sort.
bool TestSortingDigits() {
  long x = 8675309L;
  digit_adaptor<long> d{x};

  std::sort(d.begin(), d.end());
  if (x != 356789L) { return false; }

  std::sort(d.rbegin(), d.rend());
  if (x != 9876530L) { return false; }

  x = -8675309L;

  std::sort(d.begin(), d.end());
  if (x != -356789L) { return false; }

  std::sort(d.rbegin(), d.rend());
  if (x != -9876530L) { return false; }

  return true;
}

// Tests reversing digits with std::reverse.
bool TestReversingDigits() {
  long x = 8675309L;
  digit_adaptor<long> d{x};

  std::reverse(d.begin(), d.end());
  if (x != 9035768L) { return false; }

  x = 8675309L;
  std::reverse(d.rbegin(), d.rend());
  if (x != 9035768L) { return false; }

  x = -8675309L;
  std::reverse(d.begin(), d.end());
  if (x != -9035768L) { return false; }

  x = -8675309L;
  std::reverse(d.rbegin(), d.rend());
  if (x != -9035768L) { return false; }

  return true;
}

// Declares our set of test cases.
#define TEST_CASE(x) TestCase{ #x, x }
const TestCase tests[] = {
  TEST_CASE(TestReadingDecimalDigitsFromPositiveSignedInt),
  TEST_CASE(TestReadingDecimalDigitsFromNegativeSignedInt),
  TEST_CASE(TestReadingDecimalDigitsFromUnsignedInt),
  TEST_CASE(TestReadingHexDigitsFromPositiveSignedInt),
  TEST_CASE(TestReadingHexDigitsFromNegativeSignedInt),
  TEST_CASE(TestReadingHexDigitsFromUnsignedInt),
  TEST_CASE(TestReadingOctalDigitsFromPositiveSignedInt),
  TEST_CASE(TestReadingOctalDigitsFromNegativeSignedInt),
  TEST_CASE(TestReadingOctalDigitsFromUnsignedInt),
  TEST_CASE(TestWritingDecimalDigitsToPositiveSignedLong),
  TEST_CASE(TestWritingDecimalDigitsToNegativeSignedLong),
  TEST_CASE(TestWritingDecimalDigitsToUnsignedLong),
  TEST_CASE(TestWritingHexDigitsToPositiveSignedLong),
  TEST_CASE(TestWritingHexDigitsToNegativeSignedLong),
  TEST_CASE(TestWritingHexDigitsToUnsignedLong),
  TEST_CASE(TestWritingOctalDigitsToPositiveSignedLong),
  TEST_CASE(TestWritingOctalDigitsToNegativeSignedLong),
  TEST_CASE(TestWritingOctalDigitsToUnsignedLong),
  TEST_CASE(TestAdaptorOnZero),
  TEST_CASE(TestAdaptorWithExtraDigits),
  TEST_CASE(TestLeadingZerosWithPositiveInt),
  TEST_CASE(TestLeadingZerosWithNegativeInt),
  TEST_CASE(TestSwappingDigits),
  TEST_CASE(TestReadingViaForwardIterators),
  TEST_CASE(TestReadingViaReverseIterators),
  TEST_CASE(TestSortingDigits),
  TEST_CASE(TestReversingDigits),
};

}  // namespace


int main() {
  int pass_cnt = 0, fail_cnt = 0;

  for (const auto& test : tests) {
    const bool passed = test.test();
    std::cout << (passed ? "PASS     :  " : "     FAIL:  ") 
              << test.name << '\n';
    pass_cnt += passed == true;
    fail_cnt += passed == false;
  }

  std::cout << "Passed: " << pass_cnt << " Failed: " << fail_cnt << '\n';
}
