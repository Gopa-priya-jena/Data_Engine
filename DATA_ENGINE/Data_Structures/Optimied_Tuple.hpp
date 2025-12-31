#pragma once
#include <Basic_Type.hpp>
#include <Data_Structures/common_utilities/Optimized_Tuple_helper_Templates.hpp>
#include <Data_Structures/common_utilities/Type_Group.hpp>
#include <utility>
namespace DS
{
  template <class... T>
    requires( TS::is_acess<T> && ... )
  // class
  class cross_tuple
  {
    template <typename X>
    using get_type = X::Type_;
    template <typename X>
    using get_key = X::key;
    using Indices = std::make_index_sequence<( sizeof...( T ) )>;
    using sorted_type = TS::Type_object<T...>;

    template <u64... i>
    static consteval auto get_tuple_type( std::index_sequence<i...> )
    {
      return std::tuple<typename sorted_type::template Type<i>::Type_...>{};
    }
    template <u64... i>
    static consteval auto get_info_tuple( std::index_sequence<i...> )
    {
      return std::tuple<typename sorted_type::template Type<i>...>{};
    }
    using Tuple_Type = decltype( get_tuple_type( Indices{} ) );

    using info_tuple = decltype( get_info_tuple( Indices{} ) );

    template <u64 i>
    using get_info_type_at = std::tuple_element<i, info_tuple>::type;

    template <u64... i>
    static consteval std::array<u64, sizeof...( T )>
    key_map( std::index_sequence<i...> )
    {
      std::array<u64, sizeof...( T )> a;
      ( ( a[ i ] = get_info_type_at<i>::key ), ... );

      return a;
    }

    template <u64 I>
    static consteval u64 map()
      requires( ( I == T::key ) || ... )

    {

      auto map = key_map( Indices{} );
      for ( auto i = 0; i < sizeof...( T ); i++ )
      {
        if ( map[ i ] == I )
        {
          return i;
        }
      }
      return 0;
    }
    Tuple_Type collection;

  public:
    template <u64 N>
    inline auto &get()
      requires( ( N == T::key ) || ... )
    {
      return std::get<map<N>()>( collection );
    }
  };
  template <typename... T>
  struct Optimized_tuple
  {

    template <typename T_type>
    using _acess = TS::acess<T_type, 0>;

    using Type = decltype( cross_tuple<_acess<T>...>::collection );
  };
} // namespace DS
