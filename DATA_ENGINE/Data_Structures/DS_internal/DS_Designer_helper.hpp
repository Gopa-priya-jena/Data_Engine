#pragma once
#include <Data_Structures/DS_internal/DS_Design_internal.hpp>
#include <memory>
namespace DS
{
  template <typename OFFSET_Type = u32, class Metadata = void,
            class Allocator = std::allocator<void>, std::size_t Alignment = 0,
            bool Internal_alignment = false, class... Base_Types>
  struct DS_Handle_maker
  {

    using Index = std::make_integer_sequence<u64, sizeof...( Base_Types )>;
    using grp = TS::Type_Group<Base_Types...>;
    template <typename Wrapper, u64 Index>
    using WrappedType = typename Wrapper::template Type<Index>;

    // prepare Designer class{}
    using Design = TS::design<OFFSET_Type, Metadata, Allocator, Alignment,
                              Internal_alignment>;

    // prepare metadata data structure
    template <u64... Indices>
    static consteval auto get_type( std::index_sequence<Indices...> )
        -> DS::Data_Structure_Design<Design,
                                     WrappedType<Base_Types, Indices>...>;

    struct SGD
    {
      using DS_handle_Design = decltype( get_type( Index{} ) );
      using Type = DS_handle_Design::Single_Group_Data_Structure_Design;
    };
    struct SOA
    {
      using DS_handle_Design = decltype( get_type( Index{} ) );
      using Type = DS_handle_Design::SOA_Design;
    };
    // during multiblock Base_Types  = groups
    struct MGS
    {
      static consteval u64 Total_element_count()
      {
        u64 res = 0;
        ( ( res += Base_Types::E_count ), ... );
        return res;
      }
      static consteval auto Per_group_ele_count()
      {
        std::array<u64, sizeof...( Base_Types )> ele;
        auto                                     i = 0;
        ( ( ele[ i++ ] = Base_Types::E_count ), ... );
        return ele;
      }
      static consteval auto Per_element_group_table()
      {
        // each elements group
        auto ele_count = Per_group_ele_count();
        std::array<u64, Total_element_count()> table;
        for ( sz x = 0, y = 0; x < table.size(); x++ )
        {
          if ( ele_count[ y ] == 0 )
          {
            y++;
          }
          table[ x ] = y;
          ele_count[ y ]--;
        }
        return table;
      }
      static consteval auto Pre_sum_table()
      {
        std::array<u64, sizeof...( Base_Types )> table;
        auto ele_count = Per_group_ele_count();
        u64  offset = 0;
        for ( sz j = 0; j < ele_count.size(); j++ )
        {
          offset += ele_count[ j ];
          table[ j ] = offset;
        }
        return table;
      }

      static consteval const u64 ele_index( u64 i )
      {
        auto table = Per_element_group_table();
        auto pre_sum = Pre_sum_table();
        i64  prev_tables = table[ i ];
        prev_tables = prev_tables - 1;
        u64 index = 0;
        u64 offset = 0;
        if ( prev_tables != -1 )
        {
          offset += pre_sum[ prev_tables ];
        }
        index = ( i - offset );
        if ( index )
        {
          index -= 1;
        }
        return index;
      }
      struct gi
      {
        u64 group, index;
      };

      static consteval auto Index_table()
      {
        std::array<gi, Total_element_count()> table;
        auto gtable = Per_element_group_table();
        for ( sz i = 0; i < table.size(); i++ )
        {
          table[ i ].group = gtable[ i ];
          table[ i ].index = ele_index( i );
        }
        return table;
      }

      struct design_info
      {
        static constexpr u64  group_count = sizeof...( Base_Types );
        static constexpr u64  element_count = Total_element_count();
        static constexpr auto group_ele_count = Per_group_ele_count();
        static constexpr auto ele_group_table = Per_element_group_table();
        static constexpr auto prev_sum_table = Pre_sum_table();
        static constexpr auto ele_index_map = Index_table();
        // static constexpr auto group_index_map = gi_map();
        using group_info = TS::Type_Group<Base_Types...>;
      };
      template <u64 i>
      static consteval auto type_at()
      {
        constexpr u64 g = design_info::ele_index_map[ i ].group;
        constexpr u64 ix = design_info::ele_index_map[ i ].index;
        return typename grp::template //
            Type<g>::template Type<ix>{};
      }

      using Group_table =
          TS::group_helper<Per_element_group_table(), Base_Types...>;

      template <u64 i>
      using Type_at = decltype( type_at<i>() );

      using Design_MGS = TS::design<OFFSET_Type, Metadata, Allocator, Alignment,
                                    Internal_alignment, design_info>;

      template <u64... Indices>
      static consteval auto get_Gtype( std::index_sequence<Indices...> )
          -> DS::Data_Structure_Design<
              Design_MGS, WrappedType<Type_at<Indices>, Indices>...>;

      using Type = decltype( get_Gtype(
          std::make_integer_sequence<u64, Total_element_count()>{} ) )::
          Multi_Group_Data_Structure_Design;
    };
  };
} // namespace DS
