#pragma once
#include <TYPE.hpp>
#include <common_utilities/Type_Group.hpp>
#include <type_traits>

namespace DS
{

  template <typename... T>
    requires( std::is_pointer_v<T> && ... )
  class Pointer_array
  {
    using Pointer_Group = TS::Type_Group<T...>;
    void *Pointer_Array[ sizeof...( T ) ];

  public:
    Pointer_array()
    {
      for ( sz i = 0; i < sizeof...( T ); i++ )
      {
        Pointer_Array[ i ] = nullptr;
      }
    }
    Pointer_array( T &...pointers )
    {
      auto i = 0;
      ( ( Pointer_Array[ i++ ] = pointers ), ... );
    }
    template <u64 N>
    inline auto &Data()
    {
      return *reinterpret_cast<Pointer_Group::template Type<N>>(
          Pointer_Array[ N ] );
    }
    template <u64 N>
    inline auto *ptr()
    {
      return reinterpret_cast<Pointer_Group::template Type<N>>(
          Pointer_Array[ N ] );
    }
    template <u64 N>
    inline auto &at()
    {
      return reinterpret_cast<Pointer_Group::template Type<N> &>(
          Pointer_Array[ N ] );
    }
  };

} // namespace DS
