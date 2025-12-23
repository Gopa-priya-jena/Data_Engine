#pragma once

#include <type_traits>

namespace DS
{

  template <class C>
  concept Base_requirment =
      std::is_default_constructible_v<C> &&
      std::is_trivially_default_constructible_v<C> && requires( C c ) {
        typename C::metadata;
        // c.init();
        c.Initialize();
        c.Terminate();
      };

} // namespace DS
