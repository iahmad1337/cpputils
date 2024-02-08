#pragma once

#include <cpputils/meta.hh>

#include <type_traits>
#include <tuple>
#include <iterator>
#include <vector>
#include <utility>
#include <optional>

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
  using iterator = std::decay_t<TBegin>;
  using value_type = std::decay_t<decltype(*std::declval<iterator>())>;

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
  std::decay_t<typename TRangeTraits<TContainer>::iterator>,
  std::decay_t<typename TRangeTraits<TContainer>::TEnd>
>;

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

  TMappedRange(TNestedRange r, Fn mapper) : NestedRange{r}, Mapper{mapper} {}

  struct TIterator {
    using iterator_category = std::input_iterator_tag;

    TIterator(const TMappedRange* r, TNestedIterator begin)
      : UnderlyingRange{r}, NestedIterator{begin} {}

    TIterator operator++() {
      ++NestedIterator;
      return *this;
    }

    value_type operator*() {
        return UnderlyingRange->Mapper(*NestedIterator);
    }

    friend bool operator==(const TIterator &lhs, const TIterator &rhs) {
      return lhs.UnderlyingRange == rhs.UnderlyingRange && lhs.NestedIterator == rhs.NestedIterator;
    }
    friend bool operator==(const TIterator &lhs, const TSentinel&) {
      return lhs.IsEnd();
    }
    friend bool operator==(const TSentinel&, const TIterator &rhs) {
      return rhs.IsEnd();
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
  };

  TIterator begin() const {
    return TIterator(this, NestedRange.begin());
  }

  TSentinel end() const {
    return Sentinel;
  }


// private:  // TODO: the friend mechanics is broken here for some reason (gcc 11.4)
  TNestedRange NestedRange;
  Fn Mapper;
};

template<class TRange, class Fn>
TMappedRange(TRange, Fn) -> TMappedRange<std::decay_t<TRange>, std::decay_t<Fn>>;

template<class TRange, class Fn>
auto Map(const TRange& r, Fn mapper) {
  return TMappedRange(TRangeView{r}, mapper);
}

/*******************************************************************************
*                                Zipped range                                 *
*******************************************************************************/

template<class... TRanges>
struct TZippedRange {
  struct TIterator {
    friend TZippedRange;
    using value_type = std::tuple<typename TRangeTraits<TRanges>::value_type...>;

    using TIteratorTuple = std::tuple<typename TRangeTraits<TRanges>::iterator...>;
    using TEndTuple = std::tuple<typename TRangeTraits<TRanges>::TEnd...>;

    TIterator operator++() {
      IteratorTuple = utils::meta::TransformTuple(IteratorTuple, [](auto x) { return ++x; });
      return *this;
    }

    value_type operator*() {
      return utils::meta::TransformTuple(IteratorTuple, [](auto x) { return *x; });
    }

    friend bool operator==(const TIterator &lhs, const TIterator &rhs) {
      return lhs.IsEnd() or rhs.IsEnd();
    }
    friend bool operator==(const TIterator &lhs, const TSentinel&) {
      return lhs.IsEnd();
    }
    friend bool operator==(const TSentinel&, const TIterator &rhs) {
      return rhs.IsEnd();
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
    template<class... TBegins>
    TIterator(const TZippedRange* r, TBegins... begins) : IteratorTuple{begins...}, UnderlyingRange{r} {}

    bool IsEnd() const {
      return utils::meta::ReduceTuple(
        utils::meta::ApplyBinOp(IteratorTuple, UnderlyingRange->storage, [] (auto x, auto y) { return x == std::end(y); }),
        false,
        std::bit_or{}
      );
    }

    TIteratorTuple IteratorTuple;
    const TZippedRange<TRanges...>* UnderlyingRange;
  };

  using iterator = TIterator;
  using value_type = typename iterator::value_type;

  TZippedRange(TRanges... rs) : storage{std::move(rs)...} {}

  TIterator begin() const {
    return BeginImpl<>(std::index_sequence_for<TRanges...>{});
  }

  TSentinel end() const {
    return Sentinel;
  }

private:
  template<size_t... Is>
  TIterator BeginImpl(std::index_sequence<Is...>) const {
    return TIterator{this, std::begin(std::get<Is>(storage))...};
  }
  std::tuple<TRanges...> storage;
};

template<class... TRanges>
TZippedRange(TRanges...) -> TZippedRange<std::decay_t<TRanges>...>;

template<class... TRanges>
auto Zip(const TRanges&... ranges) {
  return TZippedRange(TRangeView{ranges}...);
}

/*******************************************************************************
*                                 ToContainer                                 *
*******************************************************************************/

template<class TContainer>
auto ToVector(const TContainer& c) {
  std::vector<typename TRangeTraits<TContainer>::value_type> result;
  for (auto v : c) {
    result.emplace_back(std::move(v));
  }
  return result;
}

}  // namespace utils
