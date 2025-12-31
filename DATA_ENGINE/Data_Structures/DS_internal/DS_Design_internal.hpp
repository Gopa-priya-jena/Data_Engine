#pragma once
#include <Data_Structures/DS_internal/DS_constrains.hpp>
#include <Data_Structures/DS_internal/DS_internal.hpp>
#include <Data_Structures/Pointer_Array.hpp>
#include <Data_Structures/common_utilities/Refernce_Types.hpp>
#include <Data_Structures/common_utilities/Type_Group.hpp>
#include <Data_Structures/common_utilities/common.hpp>
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace DS
{
  // TODO: write alignment based allocator
  // NOTE: this gives structural design
  // TODO: write requirements

  template <typename Design_Info, class... Base_Types>
  class Data_Structure_Design
  {
    using OFFSET_Type = typename Design_Info::offset_type;
    using Metadata = typename Design_Info::common_metadata;
    using Allocator = Design_Info::allocator;
    static constexpr size_t Alignment = Design_Info::Alignment_requirements;
    static constexpr bool Internal_alignment = Design_Info::internal_alignment;

  public:
    // NOTE: forward declaration of all implementation
    class Single_Group_Data_Structure_Design;
    class SOA_Design;
    class Multi_Group_Data_Structure_Design;
    // NOTE: forward declaration ends here
  private:
    template <typename Derived, typename Wrapper>
    using BaseType = typename Wrapper::template Type<Derived>;
    template <typename Wrapper>
    using get_index = Wrapper::Index;
    // base types
    template <typename T>
    using SG_Base = BaseType<Single_Group_Data_Structure_Design, T>;
    template <typename T>
    using SOA_Base = BaseType<SOA_Design, T>;
    template <typename T>
    using MG_Base = BaseType<Multi_Group_Data_Structure_Design, T>;
    template <typename... T>
      requires( ( Base_requirment<T> ) && ... )
    using Base_group_gen = TS::Type_Group<T...>;
    // metadata   object  design
    // @base type
    // @types
    template <template <typename c> typename B, typename... T>
    using m_object =
        TS::Type_object<TS::acess<typename B<T>::metadata, T::Index>...>;

    using SGm_object = m_object<SG_Base, Base_Types...>;

    // for meta data deduction
    template <typename meta_object, typename _metadata = Metadata>
    static consteval auto get_metadata()
    {
      if constexpr ( 0 == meta_object::get_nonvoid_type_count() )
      {
        if constexpr ( std::is_void_v<_metadata> )
          return;
        else
          return Metadata_Header_Type<_metadata, void>{};
      }
      else
      {
        return [ & ]<u64... i>( std::index_sequence<i...> )
        {
          return Metadata_Header_Type<
              _metadata, typename meta_object::template V_less_type<i>...>{};
        }( std::make_integer_sequence<
            u64, meta_object::get_nonvoid_type_count()>{} );
      }
    }

    using _sgmetadata = decltype( get_metadata<SGm_object, Metadata>() );

    struct MG_processing
    {
      using base_grp = TS::Type_Group<Base_Types...>;
      using design_info = Design_Info::mg_design;
      template <typename... T>
      using mg_mobj = m_object<MG_Base, T...>;

      template <u64... i>
      static consteval auto make_mobject( std::integer_sequence<u64, i...> )
      {
        return mg_mobj<typename base_grp::template Type<i>...>{};
      }

      template <u64 G, u64 grp_sz>
      static consteval auto gen_arr()
      {
        std::array<u64, grp_sz> ele_index;
        for ( sz i = 0, j = 0; i < design_info::ele_group_table.size(); i++ )
        {
          if ( G == design_info::ele_group_table[ i ] )
            ele_index[ j++ ] = i;
        }
        return ele_index;
      }

      template <u64 G>
      using Grp_index =
          TS::reindex<u64, design_info::group_ele_count[ G ],
                      gen_arr<G, design_info::group_ele_count[ G ]>()>::Index;

      template <u64 G>
      static consteval auto gen_mobject()
      {
        return make_mobject( Grp_index<G>{} );
      }

      template <u64 G>
      static consteval auto group_mdata()
      {
        static_assert( G < design_info::group_count, "high group count" );
        return get_metadata<
            decltype( gen_mobject<G>() ),
            typename design_info::group_info::template Type<G>::C_Type>();
      }

      template <u64... i>
      static consteval auto metadata_list( std::index_sequence<i...> )
      {
        return TS::Type_Group<decltype( group_mdata<i>() )...>{};
      }

      using mete_typelist = decltype( metadata_list(
          std::make_index_sequence<design_info::group_count>{} ) );
    };

  public:
    // class implementations
    class Single_Group_Data_Structure_Design : public SG_Base<Base_Types>...
    {
      using Base_group = Base_group_gen<SG_Base<Base_Types>...>;
      using DS_Type =
          Data_Struture_Storage_Block<sizeof...( Base_Types ), OFFSET_Type,
                                      Alignment, _sgmetadata>;

      DS_Type  *Data_Structure;
      Allocator allocator;

      DS_Type *Allocate()
      {
        u64                                             BODY_Size = 0;
        std::array<alloc_info, sizeof...( Base_Types )> element_size;
        auto                                            i = 0;
        auto unit_alloc = [ & ]( u64 alloc_sz )
        {
          element_size[ i ].sz = alloc_sz;
          if constexpr ( Internal_alignment )
          {
            element_size[ i ].aligned_sz =
                ( ( alloc_sz + ( Alignment - 1 ) ) / Alignment ) * Alignment;
          }
          else
          {
            element_size[ i ].aligned_sz = alloc_sz;
          }
          BODY_Size += element_size[ i ].aligned_sz;
          i++;
        };

        ( ( unit_alloc( SG_Base<Base_Types>::allocate() ) ), ... );

        u64  Size = ( sizeof( Data_Structure->HEADER ) + BODY_Size );
        auto ptr = reinterpret_cast<DS_Type *>( allocator.allocate( Size ) );
        return new ( ptr ) DS_Type( Size, element_size );
      }

      void Dealloc( DS_Type *ptr )
      {
        auto sz = ptr->HEADER.Body_size;
        allocator.deallocate( ( u8 * )ptr, sz );
      }

    public:
      template <u64 Index>
      using Type = TS::Type_Group<SG_Base<Base_Types>...>::template Type<Index>;

      template <u64 Index>
      [[nodiscard( "TS::acess to data discared" )]] inline u8 &Acess_DATA()
      {
        ;
        return Data_Structure->template Acess_Data<Index>();
      }

      template <u64 Index>
      [[nodiscard( "TS::acess to data discared" )]] inline u64 Acess_Size()
      {
        return Data_Structure->template Acess_Size<Index>();
      }

      inline auto &Acess_CommonMetadata()
        requires( !std::is_void_v<Metadata> )
      {
        ;
        return Data_Structure->HEADER.Metadata.Common_Data;
      }

      template <u64 N>
      inline auto &Acess_SelfMetadata()
        requires( SGm_object::get_nonvoid_type_count() != 0 )
      {
        ;
        return Data_Structure->HEADER.Metadata.E_Data.template get<N>();
      }

      Single_Group_Data_Structure_Design()
      {
        Data_Structure = Allocate();
        Data_Structure->Set_Zero_all();
        ( ( SG_Base<Base_Types>::Initialize() ), ... );
      }

      ~Single_Group_Data_Structure_Design()
      {
        Dealloc( Data_Structure );
        ( ( SG_Base<Base_Types>::Terminate() ), ... );
      }
    };

    class SOA_Design : public SOA_Base<Base_Types>...
    {
      using Base_group = Base_group_gen<SOA_Base<Base_Types>...>;
      using pointer_type = TS::Type_Group<Data_Struture_Storage_Block<
          1, u8, Alignment, typename SOA_Base<Base_Types>::metadata>...>;

      Pointer_array<Data_Struture_Storage_Block<
          1, u8, Alignment, typename SOA_Base<Base_Types>::metadata> *...>
                          Data_Structure_Array;
      TS::maybe<Metadata> common_metadata;
      Allocator           allocator;

      template <u64 I>
      auto Allocate()
      {
        std::array<alloc_info, 1> element_size;
        element_size[ 0 ].sz = Base_group::template Type<I>::allocate();
        u64 Size =
            ( sizeof( decltype( pointer_type::template Type<I>::HEADER ) ) +
              element_size[ 0 ].sz );

        auto ptr = static_cast<pointer_type::template Type<I> *>(
            std::aligned_alloc( Alignment, Size ) );
        return new ( ptr ) pointer_type::template Type<I>( Size, element_size );
      }

      void Dealloc( auto &Data )
      {
        auto sz = Data->HEADER.Body_size;
        allocator.deallocate( ( u8 * )Data, sz );
      }

    public:
      template <u64 Index>
      using Type =
          TS::Type_Group<SOA_Base<Base_Types>...>::template Type<Index>;

      template <u64 Index>
      [[nodiscard( "TS::acess to data discared" )]] inline u8 &Acess_DATA()
      {
        return Data_Structure_Array.template ptr<Index>()
            ->template Acess_Data<0>();
      }

      template <u64 Index>
      [[nodiscard( "TS::acess to data discared" )]] inline u64 Acess_Size()
      {
        return Data_Structure_Array.template ptr<Index>()
            ->template Acess_Size<0>();
      }

      // a potential bug
      template <u64 N>
      inline TS::Ref_get<typename Base_group::template Type<N>::metadata>::Type
      acess_SelfMetadata()
      {
        ;
        if constexpr ( !std::is_void_v<
                           typename Base_group::template Type<N>::metadata> )
          return Data_Structure_Array.template ptr<N>()->HEADER.Metadata;
      }

      template <u64 N>
      inline TS::Ref_get<typename Base_group::template Type<N>::metadata>::Type
      acess_CommonMetadata()
      {
        ;
        if constexpr ( !std::is_void_v<Metadata> )
          return &common_metadata;
      }

      SOA_Design()
      {
        [ & ]<u64... i>( std::index_sequence<i...> )
        {
          ( ( Data_Structure_Array.template at<i>() = Allocate<i>() ), ... );
        }( std::make_integer_sequence<u64, sizeof...( Base_Types )>{} );
        ( ( SOA_Base<Base_Types>::Initialize() ), ... );
      }

      ~SOA_Design()
      {
        [ & ]<u64... i>( std::index_sequence<i...> )
        {
          ( ( Dealloc( Data_Structure_Array.template at<i>() ),
              Data_Structure_Array.template at<i>() = nullptr ),
            ... );
        }( std::make_integer_sequence<u64, sizeof...( Base_Types )>{} );
        ( ( SOA_Base<Base_Types>::Terminate() ), ... );
      }
    };

    // NOTE: where multiple grouping is needed for class
    //  and have differed allocation and access pattern

    class Multi_Group_Data_Structure_Design : public MG_Base<Base_Types>...
    {
      // get Gtable index each  group to generate pointer type
      using Base_group = Base_group_gen<MG_Base<Base_Types>...>;

      template <u64 G>
      using pointer_type = Data_Struture_Storage_Block<
          MG_processing::design_info::group_ele_count[ G ], OFFSET_Type,
          Alignment, typename MG_processing::mete_typelist::template Type<G>>;

      template <u64... i>
      static consteval auto ptr_type( std::index_sequence<i...> )
      {
        return Pointer_array<pointer_type<i> *...>{};
      }

      template <u64 G>
      using Grp_elements = MG_processing::template Grp_index<G>;
      using Pointer_Array = decltype( ptr_type(
          std::make_integer_sequence<
              u64, MG_processing::design_info::group_count>{} ) );

      Pointer_Array Data_Structure_Array;
      Allocator     allocator;

      template <u64 G>
      auto Allocate()
      {
        auto G_alloc =
            [ this ]<u64... I>(
                std::array<alloc_info,
                           MG_processing::design_info::group_ele_count[ G ]>
                    &element_size,
                std::integer_sequence<u64, I...> ) -> u64
        {
          u64  BODY_Size = 0;
          auto i = 0;
          auto unit_alloc = [ & ]( u64 alloc_sz )
          {
            element_size[ i ].sz = alloc_sz;
            if constexpr ( Internal_alignment )
            {
              auto align_count = ( alloc_sz + ( Alignment - 1 ) ) / Alignment;
              element_size[ i ].aligned_sz = align_count * Alignment;
            }
            else
            {
              element_size[ i ].aligned_sz = alloc_sz;
            }
            BODY_Size += element_size[ i ].aligned_sz;
            i++;
          };
          ( unit_alloc( Base_group::template Type<I>::allocate() ), ... );
          return BODY_Size;
        };

        std::array<alloc_info, MG_processing::design_info::group_ele_count[ G ]>
            element_size;

        u64  BODY_Size = G_alloc( element_size, Grp_elements<G>{} );
        u64  Size = ( sizeof( pointer_type<G>::HEADER ) + BODY_Size );
        auto ptr =
            reinterpret_cast<pointer_type<G> *>( allocator.allocate( Size ) );
        return new ( ptr ) pointer_type<G>( Size, element_size );
      }

      void Dealloc( auto &Data )
      {
        auto sz = Data->HEADER.Body_size;
        allocator.deallocate( ( u8 * )Data, sz );
        Data = nullptr;
      }

    public:
      template <u64 Index>
      using Type = TS::Type_Group<MG_Base<Base_Types>...>::template Type<Index>;

      template <u64 Index>
      [[nodiscard( "ACESS to data discared" )]] inline u8 &Acess_DATA()
      {
        return Data_Structure_Array
            .template ptr<
                MG_processing::design_info::ele_index_map[ Index ].group>()
            ->template Acess_Data<
                MG_processing::design_info::ele_index_map[ Index ].index>();
      }

      template <u64 Index>
      [[nodiscard( "ACESS to data discared" )]] inline u64 Acess_Size()
      {
        return Data_Structure_Array
            .template ptr<
                MG_processing::design_info::ele_index_map[ Index ].group>()
            ->template Acess_Size<
                MG_processing::design_info::ele_index_map[ Index ].index>();
      }

      // a potential bug
      // TODO: repair return type
      template <u64 Index>
      inline TS::Ref_get<
          typename Base_group::template Type<Index>::metadata>::Type
      Acess_SelfMetadata()
      {
        return Data_Structure_Array
            .template ptr<
                MG_processing::design_info::ele_index_map[ Index ].group>()
            ->HEADER.Metadata.E_Data.template get<Index>();
      }

      template <u64 Index>
      inline TS::Ref_get<
          typename MG_processing::design_info::group_info::template Type<
              MG_processing::design_info::ele_index_map[ Index ]
                  .group>::C_Type>::Type
      Acess_CommonMetadata()
      {
        return Data_Structure_Array
            .template ptr<
                MG_processing::design_info::ele_index_map[ Index ].group>()
            ->HEADER.Metadata.Common_Data;
      }

      Multi_Group_Data_Structure_Design()
      {
        [ & ]<u64... i>( std::integer_sequence<u64, i...> )
        {
          ( ( Data_Structure_Array.template at<i>() = Allocate<i>() ), ... );
        }( std::make_index_sequence<
            MG_processing::design_info::group_count>{} );
        ( ( MG_Base<Base_Types>::Initialize() ), ... );
      }

      ~Multi_Group_Data_Structure_Design()
      {
        [ & ]<u64... i>( std::integer_sequence<u64, i...> )
        {
          ( Dealloc( Data_Structure_Array.template at<i>() ), ... );
        }( std::make_index_sequence<
            MG_processing::design_info::group_count>{} );
        ( ( MG_Base<Base_Types>::Terminate() ), ... );
      }
    };
  };
} // namespace DS
