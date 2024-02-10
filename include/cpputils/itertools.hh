// Inspired by https://github.com/biowpn/itertools

#pragma once

#include <cpputils/meta.hh>

#include <type_traits>
#include <tuple>
#include <iterator>
#include <vector>
#include <utility>
#include <optional>

namespace utils {

namespace detail {

/// Marker for lightweight, non-owning ranges, that must only be stored as values
struct TViewTag {};

template<typename TRange>
struct TRangeTraits {
  using iterator = std::decay_t<decltype(std::begin(std::declval<TRange>()))>;
  using value_type = std::decay_t<decltype(*std::declval<iterator>())>;

  using TEnd = std::decay_t<decltype(std::end(std::declval<TRange>()))>;

  static constexpr bool is_view = std::is_base_of_v<TViewTag, std::decay_t<TRange>>;
};


/// Helper type for sentinel `.end()` iterator
struct TSentinel {};
inline constexpr TSentinel Sentinel;

template<class TBegin, class TEnd>
struct TRangeView : TViewTag {
  using iterator = std::decay_t<TBegin>;
  using value_type = std::decay_t<decltype(*std::declval<iterator>())>;

  constexpr TRangeView(iterator b, TEnd e) : b{b}, e{e} {}

  constexpr iterator begin() const {
    return b;
  }

  constexpr TEnd end() const {
    return e;
  }

private:
  iterator b;
  TEnd e;
};

template<class TBegin, class TEnd>
TRangeView(TBegin, TEnd) -> TRangeView<std::decay_t<TBegin>, std::decay_t<TEnd>>;

/*******************************************************************************
*                                Mapped range                                 *
*******************************************************************************/

template<class TNestedView, class Fn>
struct TMappedView : TViewTag {
  struct TIterator;
  friend TIterator;

  using TNestedIterator = typename TRangeTraits<TNestedView>::iterator;

  using iterator = TIterator;
  using value_type =
    std::decay_t<
      std::invoke_result_t<
        Fn,
        decltype(*std::declval<TNestedIterator>())
      >
    >;

  TMappedView(TNestedView r, Fn mapper) : NestedView{r}, Mapper{mapper} {}

  struct TIterator {
    using iterator_category = std::input_iterator_tag;

    TIterator(const TMappedView* r, TNestedIterator begin)
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
    bool IsEnd() const { return NestedIterator == std::end(UnderlyingRange->NestedView); }
    const TMappedView* UnderlyingRange;
    TNestedIterator NestedIterator;
  };

  TIterator begin() const {
    return TIterator(this, NestedView.begin());
  }

  TSentinel end() const {
    return Sentinel;
  }


private:
  TNestedView NestedView;
  Fn Mapper;
};

template<class TView, class Fn>
TMappedView(TView, Fn) -> TMappedView<std::decay_t<TView>, std::decay_t<Fn>>;

/*******************************************************************************
*                                Zipped range                                 *
*******************************************************************************/

template<class... TViews>
struct TZippedView : TViewTag {
  struct TIterator {
    friend TZippedView;
    using value_type = std::tuple<typename TRangeTraits<TViews>::value_type...>;

    using TIteratorTuple = std::tuple<typename TRangeTraits<TViews>::iterator...>;
    using TEndTuple = std::tuple<typename TRangeTraits<TViews>::TEnd...>;

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
    TIterator(const TZippedView* r, TBegins... begins) : IteratorTuple{begins...}, UnderlyingView{r} {}

    bool IsEnd() const {
      return utils::meta::ReduceTuple(
        utils::meta::ApplyBinOp(IteratorTuple, UnderlyingView->storage, [] (auto x, auto y) { return x == std::end(y); }),
        false,
        std::bit_or{}
      );
    }

    TIteratorTuple IteratorTuple;
    const TZippedView* UnderlyingView;
  };

  using iterator = TIterator;
  using value_type = typename iterator::value_type;

  TZippedView(TViews... rs) : storage{std::move(rs)...} {}

  TIterator begin() const {
    return BeginImpl<>(std::index_sequence_for<TViews...>{});
  }

  TSentinel end() const {
    return Sentinel;
  }

private:
  template<size_t... Is>
  TIterator BeginImpl(std::index_sequence<Is...>) const {
    return TIterator{this, std::begin(std::get<Is>(storage))...};
  }
  std::tuple<TViews...> storage;
};

template<class... TViews>
TZippedView(TViews...) -> TZippedView<std::decay_t<TViews>...>;

}  // namespace utils::detail

/*******************************************************************************
*                             Itertools interface                             *
*******************************************************************************/

template<class TRange>
auto MakeView(const TRange& c) {
  if constexpr (detail::TRangeTraits<TRange>::is_view) {
    return c;
  } else {
    return detail::TRangeView{std::begin(c), std::end(c)};
  }
}

template<class TRange, class Fn>
auto Map(const TRange& r, Fn mapper) {
  return detail::TMappedView{MakeView(r), mapper};
}

template<class... TRanges>
auto Zip(const TRanges&... ranges) {
  return detail::TZippedView(MakeView(ranges)...);
}

/*******************************************************************************
*                                 ToContainer                                 *
*******************************************************************************/

template<class TRange>
auto ToVector(const TRange& c) {
  std::vector<typename detail::TRangeTraits<TRange>::value_type> result;
  for (auto v : c) {
    result.emplace_back(std::move(v));
  }
  return result;
}

}  // namespace utils
