#pragma once

#include <cpputils/meta.hh>

#include <type_traits>
#include <tuple>
#include <iterator>
#include <vector>

namespace utils {

template<typename TRange>
struct TRangeTraits {
  using iterator = decltype(std::begin(std::declval<TRange>()));
  using value_type = std::decay_t<decltype(*std::declval<iterator>())>;

  using TEnd = decltype(std::end(std::declval<TRange>()));
};

struct TSentinel {};
const TSentinel Sentinel;

template<class TBegin, class TEnd>
struct TRangeView {
  using iterator = TBegin;
  using value_type = decltype(*std::declval<iterator>());

  TRangeView(iterator b, TEnd e) : b{b}, e{e} {}

  template<typename TContainer>
  TRangeView(const TContainer& c) : b{std::begin(c)}, e{std::end(c)} {}

  iterator begin() const {
    return b;
  }

  TEnd end() const {
    return e;
  }

private:
  iterator b;
  TEnd e;
};

template<class TContainer>
TRangeView(TContainer) -> TRangeView<
  typename TRangeTraits<TContainer>::iterator,
  typename TRangeTraits<TContainer>::TEnd
>;

#define DEFINE_GENERIC_ITERATOR(range) \
  public: \
    using value_type = value_type; \
    using iterator_category = std::input_iterator_tag; \
    TIterator(range* r, TNestedIterator begin) : UnderlyingRange{r}, NestedIterator{begin}, Value{*NestedIterator} {} \
    TValue operator*() const { return Value; } \
    friend bool operator==(const TIterator &lhs, const TIterator &rhs) { \
      return lhs.UnderlyingRange == rhs.UnderlyingRange && lhs.NestedIterator == rhs.NestedIterator; \
    } \
    friend bool operator==(const TIterator &lhs, const TSentinel&) { \
      return lhs.NestedIterator == lhs.UnderlyingRange->end(); \
    } \
    friend bool operator==(const TSentinel&, const TIterator &rhs) { \
      return rhs.UnderlyingRange == rhs.UnderlyingRange->end(); \
    } \
    friend bool operator!=(const TIterator &lhs, const TIterator &rhs) { \
      return !(lhs == rhs); \
    } \
    friend bool operator!=(const TIterator &lhs, const TSentinel &rhs) { \
      return !(lhs == rhs); \
    } \
    friend bool operator!=(const TSentinel &lhs, const TIterator &rhs) { \
      return !(lhs == rhs); \
    } \
  private: \
    range* UnderlyingRange; \
    TNestedIterator NestedIterator; \
    TValue Value; \

    // END OF GENERIC CODE

/*******************************************************************************
*                                Mapped range                                 *
*******************************************************************************/

template<class TNestedRange, class Fn>
struct TMappedRange {
  struct TIterator;
  friend TIterator;

  using TNestedIterator = typename TRangeTraits<TNestedRange>::iterator;
  using TValue =
    std::decay_t<
      std::invoke_result_t<
        Fn,
        decltype(*std::declval<TNestedIterator>())
      >
    >;

  using iterator = TIterator;
  using value_type = TValue;

  TMappedRange(TNestedRange r, Fn mapper) : NestedRange{std::move(r)}, Mapper{std::move(mapper)} {}

  struct TIterator {
    TIterator operator++() {
      ++NestedIterator;
      if (!IsEnd()) {
        Value = UnderlyingRange->Mapper(*NestedIterator);
      }
      return *this;
    }

    value_type operator*() const { return Value; }

  public:
    using iterator_category = std::input_iterator_tag;
    TIterator(const TMappedRange* r, TNestedIterator begin) : UnderlyingRange{r}, NestedIterator{begin}, Value{*NestedIterator} {}
    friend bool operator==(const TIterator &lhs, const TIterator &rhs) {
      return lhs.UnderlyingRange == rhs.UnderlyingRange && lhs.NestedIterator == rhs.NestedIterator;
    }
    friend bool operator==(const TIterator &lhs, const TSentinel&) {
      return lhs.NestedIterator == lhs.UnderlyingRange->NestedRange.end();
    }
    friend bool operator==(const TSentinel&, const TIterator &rhs) {
      return rhs.UnderlyingRange == rhs.UnderlyingRange->NestedRange.end();
    }
    friend bool operator!=(const TIterator &lhs, const TIterator &rhs) {
      return !(lhs == rhs);
    }
    friend bool operator!=(const TIterator &lhs, const TSentinel &rhs) {
      return !(lhs == rhs);
    }
    friend bool operator!=(const TSentinel &lhs, const TIterator &rhs) {
      return !(lhs == rhs);
    }
  private:

    bool IsEnd() const { return NestedIterator == std::end(UnderlyingRange->NestedRange); }
    const TMappedRange* UnderlyingRange;
    TNestedIterator NestedIterator;
    value_type Value;
  };

  TIterator begin() const {
    return TIterator(this, NestedRange.begin());
  }

  TSentinel end() const {
    return Sentinel;
  }


// private:
  TNestedRange NestedRange;
  Fn Mapper;
};

template<class TRange, class Fn>
auto Map(const TRange& r, Fn mapper) {
  return TMappedRange<TRange, std::decay_t<Fn>>(r, mapper);
}

/*******************************************************************************
*                                Zipped range                                 *
*******************************************************************************/

template<class... Ts>
struct TZipIterator {

};

template<class TContainer>
auto ToVector(const TContainer& c) {
  std::vector<typename TRangeTraits<TContainer>::value_type> result;
  for (auto v : c) {
    result.emplace_back(std::move(v));
  }
  return result;
}

}  // namespace utils
