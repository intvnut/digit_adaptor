#ifndef DIGIT_ADAPTOR_HH_
#define DIGIT_ADAPTOR_HH_

#include <algorithm>
#include <cmath>
#include <type_traits>

namespace jz {

// Adapts an integer type (or type that behaves as one) to look like a standard
// container holding digits in a particular radix.  The container can be const
// without the contained entity being const.
//
// Digits in the container are numbered left-to-right.  That is, the most
// significant digit is at the start of the container and the least signficant
// is at the end.
template <typename T, int RADIX = 10>
class digit_adaptor {
  static_assert(RADIX > 1, "RADIX must be larger than 1");

  public:
    // Sets number of digits based on the current magnitude of the number.
    // The number zero gets 1 digit.
    constexpr explicit digit_adaptor(T& number) noexcept
    : number_{number}, digits_{total_digits(number)} {}

    // Sets an explicit number of digits, irrespective of the number's
    // current magnitude.  Setting the number of digits smaller than the
    // number's current magnitude may result in unusual operation.
    constexpr explicit digit_adaptor(T& number, std::size_t digits) noexcept
    : number_{number}, digits_{digits} {}

    // Forward iterators.
    constexpr auto begin() const noexcept { 
      return iterator_<Forward, T>{*this, 0}; 
    }
    constexpr auto end() const noexcept {
      return iterator_<Forward, T>{*this, digits_};
    }
    constexpr auto cbegin() const noexcept {
      return iterator_<Forward, const T>{*this, 0};
    }
    constexpr auto cend() const noexcept {
      return iterator_<Forward, const T>{*this, digits_};
    }

    // Reverse iterators.
    constexpr auto rbegin() const noexcept { 
      return iterator_<Reverse, T>{*this, 0}; 
    }
    constexpr auto rend() const noexcept {
      return iterator_<Reverse, T>{*this, digits_};
    }
    constexpr auto crbegin() const noexcept {
      return iterator_<Reverse, const T>{*this, 0};
    }
    constexpr auto crend() const noexcept {
      return iterator_<Reverse, const T>{*this, digits_};
    }

    // Explicit casts to T return the number.
    constexpr explicit operator T () const noexcept {
      return number_;
    }

    // Provides indirect access to each digit.  Behaves as a reference or a
    // const reference depending on whether T is const.
    constexpr auto operator[] (int index) const noexcept {
      return reference{&number_, compute_divisor(index, digits_)};
    }

    // Returns the number of digits in the container.
    constexpr std::size_t size() const noexcept {
      return digits_;
    }

  private:
    using NCT = std::remove_cv_t<T>;
    using NCU = std::make_unsigned_t<NCT>;

    T& number_;
    const std::size_t digits_;

    // Replaces 'abs' in a more type-generic way, as std::abs is only defined
    // for a handful of signed integer types.  We don't worry about integer
    // overflow here, as we rely on the code below to only call this on "small"
    // values.
    constexpr static inline auto make_positive(const T& value) {
      const auto u = static_cast<NCU>(value);
      return value < 0 ? -u : u;
    }

    // Returns the total number of RADIX digits a number.  Allow 0 to have
    // exactly 1 digit.
    constexpr static std::size_t total_digits(NCT number) noexcept {
      auto u = make_positive(number);
      auto d = std::size_t{0};

      do {
        d++;
        u /= RADIX;
      } while (u > 0);

      return d;
    }

    enum iterator_dir { Forward, Reverse };

    // Returns the divisor for a specific digit position in RADIX.
    // N.B. This could be converted to a lookup table later for speed.
    template <iterator_dir Direction = Forward>
    constexpr static NCU compute_divisor(
        std::size_t index, std::size_t digits) {
      auto divisor = NCU{1};

      // Clamps index to digits >= index >= 0.
      index = std::max(std::min(digits, index), std::size_t{0});

      if (Direction == Forward) {
        for (auto i = index + 1; i < digits; ++i) {
          divisor *= RADIX;
        }
      } else {
        for (auto i = std::size_t{0}; i != index; ++i) {
          divisor *= RADIX;
        }
      }

      return divisor;
    }

    // Forward declarations.
    class mutable_pointer_;
    class const_pointer_;
    class const_reference_;

    // Provides indirect access to a digit held in an underlying digit
    // adaptor. This fills the same purpose as std::bitset::reference, only
    // generalized for digit_adaptor.
    class mutable_reference_ {
      public:
        using is_digit_adaptor_mutable_reference = std::true_type;

        constexpr mutable_reference_(T* number, NCU divisor) noexcept
        : number_{number}, divisor_{divisor} {}

        // Allow constructing copies of references from other references.
        //     reference_ a = b;      is like   auto&  a = b;
        //     reference_ a = foo();  is like   auto&& a = foo();
        constexpr mutable_reference_(const mutable_reference_&) = default;

        constexpr operator T () const noexcept {
          return T((make_positive(*number_) / divisor_) % RADIX);
        }

        constexpr const auto& operator=(const T& digit) const noexcept {
          const auto is_negative = *number_ < 0;
          auto temp = make_positive(*number_);
          temp -= ((temp / divisor_) % RADIX) * divisor_;
          temp += (digit % RADIX) * divisor_;
          *number_ = static_cast<NCT>(is_negative ? -temp : temp);
          return *this;
        }

        // Behaves like an lvalue reference, copying the referent's value to
        // our value, rather than copying the proxy.
        constexpr const auto& operator=(const_reference_ rhs) const noexcept {
          this->operator=(T{rhs});
          return *this;
        }
        constexpr const auto& operator=(mutable_reference_ rhs) const noexcept {
          this->operator=(T{rhs});
          return *this;
        }

        ~mutable_reference_() noexcept = default;

        // Convert a "reference" back into a "pointer."
        constexpr auto operator&() const noexcept {
          return mutable_pointer_{number_, divisor_};
        }

        constexpr bool operator<(const_reference_ rhs) const noexcept {
          return T{*this} < T{rhs};
        }

        constexpr bool operator==(const_reference_ rhs) const noexcept {
          return T{*this} == T{rhs};
        }
        
        // Forwards increments to underlying digit, as references should.
        constexpr const auto& operator++() const noexcept {
          this->operator=(T{*this} + 1);
          return *this;
        }
        
        // Forwards decrements to underlying digit, as references should.
        // Allowed only if T is not const.
        constexpr const auto& operator--() const noexcept {
          this->operator=(T{*this} - 1);
          return *this;
        }
        
        // Returns the previous value of the digit, as the result of postfix
        // increment is an rvalue, not an lvalue.  We cannot construct a
        // reference to the previous value of the digit.
        // Allowed only if T is not const.
        constexpr T operator++(int) const noexcept {
          auto temp = T{*this}; 
          this->operator++();
          return temp;
        }
        
        // Returns the previous value the digit.  See operator++(int).
        // Allowed only if T is not const.
        constexpr T operator--(int) const noexcept {
          auto temp = T{*this}; 
          this->operator--();
          return temp;
        }

        // Swaps the underlying digit values, not the reference proxies.
        // Allowed only if T is not const.
        constexpr void swap(const mutable_reference_& rhs) const noexcept {
          auto d1 = T{*this};
          auto d2 = T{rhs};
          this->operator=(d2);
          rhs.operator=(d1);
        }

        // We can convert to a const_reference_ at the drop of a hat.
        constexpr operator const_reference_() const noexcept {
          return const_reference_{*this};
        }

      private:
        T *const number_;
        const NCU divisor_;
        friend class const_reference_;
    };

    // Minimally behaves like a pointer to mutable digit.  It's really just
    // sititng on a mutable_reference_, and the only thing you can do is
    // "dereference" it to yield the underlying mutable_reference_.
    class mutable_pointer_ {
      public:
        constexpr mutable_pointer_(T* number, NCU divisor) noexcept
        : ref_{number, divisor} {}

        constexpr auto operator->() const { return ref_; }
        constexpr auto operator*()  const { return ref_; }

      private:
        mutable_reference_ ref_;
    };

    // Provides read-only indirect access to a digit held in an underlying
    // digit adaptor.
    //
    // We provide a const reference proxy rather than returning a value like
    // std::bitset does, to preserve the fiction that we're a container. 
    class const_reference_ {
      public:
        constexpr const_reference_(T* number, NCU divisor) noexcept
        : number_{number}, divisor_{divisor} {}

        // Allow constructing copies of const_references from other
        // const_references.
        constexpr const_reference_(const const_reference_&) noexcept = default;

        // Allow constructing const_reference from a reference.
        constexpr const_reference_(const mutable_reference_& ref)
        : number_{ref.number_}, divisor_{ref.divisor_} {}

        constexpr operator T () const noexcept {
          return T((make_positive(*number_) / divisor_) % RADIX);
        }

        ~const_reference_() noexcept = default;

        // Convert a "reference" back into a "pointer."
        constexpr auto operator&() const noexcept {
          return const_pointer_{number_, divisor_};
        }

        constexpr bool operator<(const_reference_ rhs) const noexcept {
          return T{*this} < T{rhs};
        }

        constexpr bool operator==(const_reference_ rhs) const noexcept {
          return T{*this} == T{rhs};
        }

      private:
        const T *const number_;
        const NCU divisor_;
    };

    // Minimally behaves like a pointer to const digit.  It's really just
    // sititng on a const_reference_, and the only thing you can do is
    // "dereference" it to yield the underlying const_reference_.
    class const_pointer_ {
      public:
        constexpr const_pointer_(T* number, NCU divisor) noexcept
        : ref_{number, divisor} {}

        constexpr auto operator->() const { return ref_; }
        constexpr auto operator*()  const { return ref_; }

      private:
        const_reference_ ref_;
    };

    // Only pick up the mutable pointer and reference types if T is mutable.
    using pointer_   = std::conditional_t<std::is_const<T>::value,
                           const_pointer_, mutable_pointer_>;
    using reference_ = std::conditional_t<std::is_const<T>::value,
                           const_reference_, mutable_reference_>;

    // The iterator_ supports both forward and reverse traversal, as well as
    // const and non-const iterators.  QT is the "qualified T."  That is, for
    // a const iterator, QT = const T, while a non-const iterator has QT = T.
    template <iterator_dir Direction, typename QT = T>
    class iterator_ {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::remove_cv_t<T>;

        // The type aliases just above this class already fold const and
        // non-const reference and pointer types into a common type for const
        // elements.  The iterator just has to discriminate between const and
        // non-const iterators.
        //
        // A non-const iterator to const elements ends up being the same as
        // a const iterator to those same elements; however, this logic doesn't
        // are.  The aliases above handle is_const<T>, while these handle
        // is_const<QT>.
        using pointer   = std::conditional_t<std::is_const<QT>::value,
                              const_pointer_, pointer_>;
        using reference = std::conditional_t<std::is_const<QT>::value,
                              const_reference_, reference_>;

        constexpr iterator_(const digit_adaptor& da, std::size_t index) noexcept
        : digit_adaptor_{&da}, index_{index} {}

        constexpr iterator_(const iterator_&)            = default;
        constexpr iterator_& operator=(const iterator_&) = default;
        
        constexpr auto& operator++() noexcept {
          if (index_ < digit_adaptor_->digits_) ++index_;
          return *this;
        }

        constexpr auto operator++(int) noexcept {
          auto temp = iterator_{*this};
          this->operator++();
          return temp;
        }

        constexpr auto& operator--() noexcept {
          if (index_ > 0) --index_;
          return *this;
        }

        constexpr auto operator--(int) noexcept {
          auto temp = iterator_{*this};
          this->operator--();
          return temp;
        }

        constexpr auto operator+(int rhs) const noexcept {
          auto temp = iterator_{*this};
          temp.index_ = std::min(digit_adaptor_->digits_,
                                 std::max(std::size_t{0}, index_ + rhs));
          return temp;
        }

        constexpr auto operator-(int rhs) const noexcept {
          auto temp = iterator_{*this};
          temp.index_ = std::min(digit_adaptor_->digits_,
                                 std::max(std::size_t{0}, index_ - rhs));
          return temp;
        }

        constexpr auto operator-(const iterator_& rhs) const noexcept {
          return std::ptrdiff_t(index_ - rhs.index_);
        }

        constexpr auto& operator+=(int rhs) noexcept {
          index_ = std::min(digit_adaptor_->digits_,
                            std::max(std::size_t{0}, index_ + rhs));
          return *this;
        }

        constexpr auto& operator-=(int rhs) noexcept {
          index_ = std::min(digit_adaptor_->digits_,
                            std::max(std::size_t{0}, index_ - rhs));
          return *this;
        }

        constexpr bool operator==(const iterator_& rhs) const noexcept {
          return index_ == rhs.index_;
        }

        constexpr bool operator!=(const iterator_& rhs) const noexcept {
          return !this->operator==(rhs);
        }

        constexpr bool operator<(const iterator_& rhs) const noexcept {
          return index_ < rhs.index_;
        }

        constexpr bool operator>=(const iterator_& rhs) const noexcept {
          return !this->operator<(rhs);
        }

        constexpr bool operator>(const iterator_& rhs) const noexcept {
          return index_ > rhs.index_;
        }

        constexpr bool operator<=(const iterator_& rhs) const noexcept {
          return !this->operator>(rhs);
        }

        constexpr reference operator*() const noexcept {
          return {&digit_adaptor_->number_,
                  compute_divisor<Direction>(index_, digit_adaptor_->digits_)};
        }

      private:
        const digit_adaptor* digit_adaptor_;
        std::size_t index_;
    };

  public:
    using element_type    = T;
    using value_type      = std::remove_cv_t<T>;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer         = digit_adaptor::pointer_;
    using const_pointer   = digit_adaptor::const_pointer_;
    using reference       = digit_adaptor::reference_;
    using const_reference = digit_adaptor::const_reference_;
    using iterator        = digit_adaptor::iterator_<Forward, T>;
    using const_iterator  = digit_adaptor::iterator_<Forward, const T>;
};

// Relies on ADL to specialize swap() for digit_adaptor::reference.
// Uses T::is_digit_adaptor_mutable_reference for SFINAE.
//
// Digit references are not swappable with std::swap, and I have no intention
// of trying to fix that.  Standard practice is to provide swap() in your own
// namespace and let ADL find it.
template <typename T>
constexpr void swap(const T& dp1, const T& dp2) noexcept {
  static_assert(
    std::is_same<typename T::is_digit_adaptor_mutable_reference,
                 std::true_type>::value, ""
  );

  dp1.swap(dp2);
}

}  // namespace jz
#endif // DIGIT_ADAPTOR_HH_
