#ifndef DIGIT_ADAPTOR_HH_
#define DIGIT_ADAPTOR_HH_

#include <algorithm>
#include <cmath>
#include <type_traits>

namespace jz {

// Adapts an integer type (or type that behaves as one) to look like a standard
// container holding digits in a particular radix.  The container can be const
// without the contained entity being const.
template <typename T, int RADIX = 10,
          std::enable_if_t<(RADIX > 1), bool> = true>
class digit_adaptor {
  public:
    constexpr explicit digit_adaptor(T& number) noexcept
    : number_{number}, digits_{total_digits(number)} {}

    constexpr digit_adaptor(T& number, std::size_t digits) noexcept
    : number_{number}, digits_{digits} {}

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

    constexpr explicit operator T () const noexcept {
      return number_;
    }

    // Returns the digit value itself if the underlying type is const.
    // Otherwise, returns a digit_proxy which acts like an lvalue reference.
    constexpr auto operator[] (int index) const noexcept {
      const auto divisor = compute_divisor(index, digits_);
      if constexpr (std::is_const_v<T>) {
        return make_positive((number_ / divisor) % RADIX);
      } else {
        return digit_adaptor::reference_<T>(&number_, divisor);
      }
    }

    constexpr std::size_t size() const noexcept {
      return digits_;
    }

  private:
    using NCT = std::remove_const_t<T>;
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

      if constexpr (Direction == Forward) {
        for (auto i = index + 1; i < digits; ++i) {
          divisor *= RADIX;
        }
      } else {
        for (auto i = 0; i != index; ++i) {
          divisor *= RADIX;
        }
      }

      return divisor;
    }

    // Forward declaration.
    template <typename QT = T> class pointer_;

    // Provides indirect access to a digit held in an underlying digit
    // adaptor. This fills the same purpose as std::bitset::reference, only
    // generalized for digit_adaptor.
    // 
    // The const-ness of the digit is carried in T, not the const-ness of
    // digit_proxy itself.
    template <typename QT = T>
    class reference_ {
      public:
        constexpr reference_(QT* number, const T divisor) noexcept
        : number_{number}, divisor_{divisor} {}

        constexpr operator T () const noexcept {
          return T(make_positive((*number_ / divisor_) % RADIX));
        }

        // Allow assignment only if QT itself is not const.
        constexpr std::enable_if_t<!std::is_const_v<QT>, const reference_&>
            operator=(const T& digit) const noexcept {
          const auto is_negative = *number_ < 0;
          auto temp = make_positive(*number_);
          temp -= ((temp / divisor_) % RADIX) * divisor_;
          temp += (digit % RADIX) * divisor_;
          *number_ = static_cast<NCT>(is_negative ? -temp : temp);
          return *this;
        }

        constexpr reference_(const reference_&) noexcept = default;

        // Behaves like an lvalue reference, copying the referent's value to
        // our value, rather than copying the proxy.
        //
        // Allow assignment only if T itself is not const.
        constexpr std::enable_if_t<!std::is_const_v<QT>, const reference_&>
            operator=(const reference_& rhs) const noexcept {
          this->operator=(T{rhs});
          return *this;
        }

        ~reference_() noexcept = default;

        // Convert a "reference" back into a "pointer."
        constexpr auto operator&() const noexcept {
          return pointer_<QT>{number_, divisor_};
        }

        constexpr bool operator<(const reference_& rhs) const noexcept {
          return T{*this} < T{rhs};
        }

        constexpr bool operator==(const reference_& rhs) const noexcept {
          return T{*this} == T{rhs};
        }
        
        // Forwards increments to underlying digit, as references should.
        constexpr const reference_& operator++() const noexcept {
          this->operator=(T{*this} + 1);
          return *this;
        }
        
        // Forwards decrements to underlying digit, as references should.
        // Allowed only if T is not const.
        constexpr std::enable_if_t<!std::is_const_v<QT>, const reference_&>
            operator--() const noexcept {
          this->operator=(T{*this} - 1);
          return *this;
        }
        
        // Returns the previous value of the digit, as the result of postfix
        // increment is an rvalue, not an lvalue.  We cannot construct a
        // reference to the previous value of the digit.
        // Allowed only if T is not const.
        template <std::enable_if_t<!std::is_const_v<QT>, bool> = true>
        constexpr T operator++(int) const noexcept {
          auto temp = T{*this}; 
          this->operator++();
          return temp;
        }
        
        // Returns the previous value the digit.  See operator++(int).
        // Allowed only if T is not const.
        template <std::enable_if_t<!std::is_const_v<QT>, bool> = true>
        constexpr T operator--(int) const noexcept {
          auto temp = T{*this}; 
          this->operator--();
          return temp;
        }

        // Swaps the underlying digit values, not the reference proxies.
        // Allowed only if T is not const.
        template <std::enable_if_t<!std::is_const_v<QT>, bool> = true>
        constexpr void swap(reference_& rhs) const noexcept {
          auto d1 = T{*this};
          auto d2 = T{rhs};
          this->operator=(d2);
          rhs.operator=(d1);
        }

      private:
        T *const number_;
        const T  divisor_;
    };

    template <typename QT>
    class pointer_ {
      public:
        constexpr pointer_(T* number, const T divisor) noexcept
        : ref_{number, divisor} {}

        constexpr reference_<QT> operator->() const { return ref_; }
        constexpr reference_<QT> operator*()  const { return ref_; }

      private:
        reference_<QT> ref_;
    };

    // Relies on ADL to specialize swap() for digit_adaptor::reference.
    constexpr friend std::enable_if_t<!std::is_const_v<T>, void> swap(
        reference_<T> dp1, reference_<T> dp2) {
      dp1.swap(dp2);
    }

    template <iterator_dir Direction, typename QT = T>
    class iterator_ {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = QT;
        using pointer           = pointer_<QT>;
        using reference         = reference_<QT>;

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

        constexpr auto operator*() const {
          const auto divisor =
            compute_divisor<Direction>(index_, digit_adaptor_->digits_);
              
          if constexpr (std::is_const_v<T>) {
            const auto temp = (digit_adaptor_->number_ / divisor) % RADIX;
            return make_positive(temp);
          } else {
            return 
              reference_<QT>(&digit_adaptor_->number_, divisor);
          }
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
    using pointer         = digit_adaptor::pointer_<T>;
    using const_pointer   = digit_adaptor::pointer_<const T>;
    using reference       = digit_adaptor::reference_<T>;
    using const_reference = digit_adaptor::reference_<const T>;
    using iterator        = digit_adaptor::iterator_<Forward, T>;
    using const_iterator  = digit_adaptor::iterator_<Forward, const T>;
};

}  // namespace jz

/*
template <typename T, int RADIX>
constexpr std::enable_if_t<!std::is_const_v<T>, void> swap(
    jz::digit_adaptor<T, RADIX>::reference dp1,
    jz::digit_adaptor<T, RADIX>::reference dp2);
*/

#endif // DIGIT_ADAPTOR_HH_
