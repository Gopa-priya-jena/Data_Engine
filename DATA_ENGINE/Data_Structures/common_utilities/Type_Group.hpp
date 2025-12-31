#pragma once
#include <Basic_Type.hpp>
#include <Data_Structures/common_utilities/Optimized_Tuple_helper_Templates.hpp>
#include <algorithm>
#include <tuple>
#include <utility>

namespace TS
{

  //  NOTE: type object
  template <typename... T>
  struct Type_object
  {
    // add all type related computation done at compile time
    using indices = std::make_integer_sequence<u64, ( sizeof...( T ) )>;
    using type_list = std::tuple<T...>;
    struct info
    {
      u64 Size, pos;
    };
    template <u64 i>
    using get_type_at = std::tuple_element<i, type_list>::type;

    static consteval const std::array<info, sizeof...( T )> sort_pos()
    {
      auto info_arr = get_index_arr();
      std::sort( info_arr.begin(), info_arr.end(),
                 []( const info &a, const info &b )
                 {
                   return a.Size < b.Size;
                 } );
      return info_arr;
    }
    template <u64 i>
    static consteval const u64 get_pos()
    {
      return sort_pos()[ i ].pos;
    }

    static consteval std::array<info, sizeof...( T )> get_index_arr()
    {
      std::array<info, sizeof...( T )> info_arr;

      u64 i = 0;
      ( ( info_arr[ i ].Size = sizeof( typename T::Type_ ),
          info_arr[ i ].pos = i, i++ ),
        ... );
      return info_arr;
    }
    template <u64 i>
    static consteval auto get_stype()
    {
      return get_type_at<get_pos<i>()>{};
    }
    template <u64 i>
    using Type = decltype( get_stype<i>() );

    static consteval auto skip_void_in_list()
    {
      std::array<i64, sizeof...( T ) + 1> v_list;
      i64                                 i = 1;
      v_list[ 0 ] = 0;
      ( ( ( !std::is_void_v<typename T::Type_> )
              ? ( v_list[ i++ ] = 1, v_list[ 0 ] += 1 )
              : v_list[ i++ ] = -1 ),
        ... );
      return v_list;
    }
    static consteval i64 get_nonvoid_type_count()
    {
      return skip_void_in_list()[ 0 ];
    }
    template <u64 N>
    static consteval u64 get_type_without_void()
    {
      auto       res = 0, element_sequence = 0;
      const auto x = skip_void_in_list();
      for ( auto i = 1; i < x.size(); i++ )
      {
        if ( x[ i ] == 1 )
        {
          element_sequence++;
          if ( N == element_sequence )
          {
            res = i - 1;
            break;
          }
        }
      }
      return res;
    }
    template <u64 i>
    using V_less_type = get_type_at<get_type_without_void<i>()>;
  };

  template <typename... T>
  struct Type_Group
  {
    template <typename t>
    static consteval bool check()
    {
      return ( ( std::is_same_v<t, T> ) || ... );
    }
    static const u64 E_count = sizeof...( T );
    template <u64 N>
    using Type = std::tuple_element<N, std::tuple<T...>>::type;
  };

  using null_group = Type_Group<void>;
} // namespace TS
