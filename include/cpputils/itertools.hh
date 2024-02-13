// Inspired by https://github.com/biowpn/itertools

#pragma once

#include <cpputils/meta.hh>

#include <type_traits>
#include <tuple>
#include <iterator>
#include <vector>
#include <utility>

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

  constexpr TMappedView(TNestedView r, Fn mapper) : NestedView{r}, Mapper{mapper} {}

  struct TIterator {
    using iterator_category = std::input_iterator_tag;

    constexpr TIterator(const TMappedView* r, TNestedIterator begin)
      : UnderlyingRange{r}, NestedIterator{begin} {}

    constexpr TIterator operator++() {
      ++NestedIterator;
      return *this;
    }

    constexpr value_type operator*() {
        return UnderlyingRange->Mapper(*NestedIterator);
    }

    friend constexpr bool operator==(const TIterator &lhs, const TIterator &rhs) {
      return lhs.UnderlyingRange == rhs.UnderlyingRange && lhs.NestedIterator == rhs.NestedIterator;
    }
    friend constexpr bool operator==(const TIterator &lhs, const TSentinel&) {
      return lhs.IsEnd();
    }
    friend constexpr bool operator==(const TSentinel&, const TIterator &rhs) {
      return rhs.IsEnd();
    }
    friend constexpr bool operator!=(const TIterator &lhs, const TIterator &rhs) {
      return !(lhs == rhs);
    }
    friend constexpr bool operator!=(const TIterator &lhs, const TSentinel &rhs) {
      return !(lhs == rhs);
    }
    friend constexpr bool operator!=(const TSentinel &lhs, const TIterator &rhs) {
      return !(lhs == rhs);
    }
  private:
    constexpr bool IsEnd() const { return NestedIterator == std::end(UnderlyingRange->NestedView); }
    const TMappedView* UnderlyingRange;
    TNestedIterator NestedIterator;
  };

  constexpr TIterator begin() const {
    return TIterator(this, NestedView.begin());
  }

  constexpr TSentinel end() const {
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

    constexpr TIterator operator++() {
      IteratorTuple = utils::meta::TransformTuple(IteratorTuple, [](auto x) { return ++x; });
      return *this;
    }

    constexpr value_type operator*() {
      return utils::meta::TransformTuple(IteratorTuple, [](auto x) { return *x; });
    }

    friend constexpr bool operator==(const TIterator &lhs, const TIterator &rhs) {
      return lhs.IsEnd() or rhs.IsEnd();
    }
    friend constexpr bool operator==(const TIterator &lhs, const TSentinel&) {
      return lhs.IsEnd();
    }
    friend constexpr bool operator==(const TSentinel&, const TIterator &rhs) {
      return rhs.IsEnd();
    }
    friend constexpr bool operator!=(const TIterator &lhs, const TIterator &rhs) {
      return !(lhs == rhs);
    }
    friend constexpr bool operator!=(const TIterator &lhs, const TSentinel &rhs) {
      return !(lhs == rhs);
    }
    friend constexpr bool operator!=(const TSentinel &lhs, const TIterator &rhs) {
      return !(lhs == rhs);
    }

  private:
    template<class... TBegins>
    constexpr TIterator(const TZippedView* r, TBegins... begins) : IteratorTuple{begins...}, UnderlyingView{r} {}

    constexpr bool IsEnd() const {
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

  constexpr TZippedView(TViews... rs) : storage{std::move(rs)...} {}

  constexpr TIterator begin() const {
    return BeginImpl<>(std::index_sequence_for<TViews...>{});
  }

  constexpr TSentinel end() const {
    return Sentinel;
  }

private:
  template<size_t... Is>
  constexpr TIterator BeginImpl(std::index_sequence<Is...>) const {
    return TIterator{this, std::begin(std::get<Is>(storage))...};
  }
  std::tuple<TViews...> storage;
};

template<class... TViews>
TZippedView(TViews...) -> TZippedView<std::decay_t<TViews>...>;

template<class TNestedView, class Fn>
struct TFilteredView : TViewTag {
  struct TIterator;
  friend TIterator;

  using TNestedIterator = typename TRangeTraits<TNestedView>::iterator;

  using iterator = TIterator;
  using value_type = typename TRangeTraits<TNestedView>::value_type;

  TFilteredView(TNestedView r, Fn filter) : NestedView{r}, Filter{filter} {}

  struct TIterator {
    using iterator_category = std::input_iterator_tag;

    constexpr TIterator(const TFilteredView* r, TNestedIterator begin)
      : UnderlyingRange{r}, NestedIterator{begin} {
      if (!IsEnd()) {
        SeekNext();
      }
    }

    constexpr TIterator operator++() {
      ++NestedIterator;
      SeekNext();
      return *this;
    }

    constexpr value_type operator*() {
        return *NestedIterator;
    }

    friend constexpr bool operator==(const TIterator &lhs, const TIterator &rhs) {
      return lhs.NestedIterator == rhs.NestedIterator;
    }
    friend constexpr bool operator==(const TIterator &lhs, const TSentinel&) {
      return lhs.IsEnd();
    }
    friend constexpr bool operator==(const TSentinel&, const TIterator &rhs) {
      return rhs.IsEnd();
    }
    friend constexpr bool operator!=(const TIterator &lhs, const TIterator &rhs) {
      return !(lhs == rhs);
    }
    friend constexpr bool operator!=(const TIterator &lhs, const TSentinel &rhs) {
      return !(lhs == rhs);
    }
    friend constexpr bool operator!=(const TSentinel &lhs, const TIterator &rhs) {
      return !(lhs == rhs);
    }
  private:
    constexpr bool IsEnd() const { return NestedIterator == std::end(UnderlyingRange->NestedView); }

    constexpr void SeekNext() {
      while (!IsEnd() && !UnderlyingRange->Filter(*NestedIterator)) {
        ++NestedIterator;
      }
    }

    const TFilteredView* UnderlyingRange;
    TNestedIterator NestedIterator;
  };

  constexpr TIterator begin() const {
    return TIterator(this, NestedView.begin());
  }

  constexpr TSentinel end() const {
    return Sentinel;
  }


private:
  TNestedView NestedView;
  Fn Filter;
};

template<class TView, class Fn>
TFilteredView(TView, Fn) -> TFilteredView<std::decay_t<TView>, std::decay_t<Fn>>;

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

template<class TRange, class Fn>
auto Filter(const TRange& r, Fn filter) {
  return detail::TFilteredView{MakeView(r), filter};
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
