#pragma once
#include <Basic_Type.hpp>
#include <concepts>

namespace TS
{

  template <typename T, u64 N>
  struct acess
  {
    using Type_ = T;
    const static u64 key = N;
  };

  template <class C>
  concept is_acess = requires( C x ) {
    { acess{ x } } -> std::same_as<C>;
  };

} // namespace TS
