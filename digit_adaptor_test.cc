#include "digit_adaptor.hh"
#include <algorithm>
#include <iostream>

namespace {

template <typename T, typename U>
void output_digits(const T& digits, const U& number) {
  for (const auto d : digits) {
    std::cout << d;
  }
  std::cout << " " << number << '\n';
}

} // namespace

int main() {
  using namespace jz;

  {
    auto u = 8675309u;
    auto da = digit_adaptor<unsigned>{u};

    output_digits(da, u);
    std::reverse(da.begin(), da.end());
    output_digits(da, u);
    std::sort(da.begin(), da.end());
    output_digits(da, u);
    da[4]++;
    output_digits(da, u);
    --da[4];
    output_digits(da, u);
    da[0]=1;
    output_digits(da, u);
    auto ri = da.rbegin();
    *ri++ = 1;
    *ri++ = 2;
    *ri++ = 3;
    *ri++ = 4;
    output_digits(da, u);
  }

  {
    auto i = -8675309;
    auto da = digit_adaptor<int>{i};

    output_digits(da, i);
    std::reverse(da.begin(), da.end());
    output_digits(da, i);
    std::sort(da.begin(), da.end());
    output_digits(da, i);
    da[4]++;
    output_digits(da, i);
    --da[4];
    output_digits(da, i);
    da[0]=1;
    output_digits(da, i);
    auto ri = da.rbegin();
    *ri++ = 1;
    *ri++ = 2;
    *ri++ = 3;
    *ri++ = 4;
    output_digits(da, i);
  }

  {
    auto       mu = 8675309;
    const auto cu = 8675319;
    auto mda = digit_adaptor{mu};
    auto cda = digit_adaptor{cu};

    for (int i = 0; i < 7; ++i) {
      std::cout 
        << "mda[" << i << "] == cda[" << i << "]? "
        << (mda[i] == cda[i]) << '\n';
    }

    for (int i = 0; i < 7; ++i) {
      std::cout
        << "mda[" << i << "] + cda[" << i << "]? "
        << (mda[i] + cda[i]) << '\n';
    }
  }
}
