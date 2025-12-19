#pragma once
#include <TYPE.hpp>
#include <concepts>
#include <type_traits>

namespace constrain
{

  template <typename T>
  concept FundamentalType = std::is_fundamental_v<T>;

  template <typename T, T A, T max>
  concept less_than = requires() {
    { A < max } -> std::convertible_to<bool>;
  };

  template <typename T>
  concept has_allocation = requires( T t ) {
    { t.allocate() } -> std::same_as<u64>;
  };
} // namespace constrain
