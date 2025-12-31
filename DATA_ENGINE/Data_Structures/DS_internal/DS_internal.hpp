#pragma once
#include <Data_Structures/Optimied_Tuple.hpp>
#include <cstring>
namespace DS
{

  // TODO: add requirment
  template <typename C, typename... T>
  struct Metadata_Header_Type
  {
    C                 Common_Data;
    cross_tuple<T...> E_Data;
  };
  template <typename... T>
  struct Metadata_Header_Type<void, T...>
  {
    cross_tuple<T...> E_Data;
  };
  template <typename C>
  struct Metadata_Header_Type<C, void>
  {
    C Common_Data;
  };

  // alloc_info
  struct alloc_info
  {
    u64 sz, aligned_sz;
  };

  struct Content
  {
    u8 DATA[];
  };

  template <typename OFFSET_Type = u32>
  struct INDEX
  {
    OFFSET_Type _offset;
    OFFSET_Type _size;
  };
  template <class C>
  concept is_index = requires( C x ) {
    { INDEX{} } -> std::same_as<C>;
  };
  template <u64 element_count, typename OFFSET_Type, class Metadata_Type>
  struct Header
  {
  public:
    u64                                           Body_size{ 0 };
    std::array<INDEX<OFFSET_Type>, element_count> Body_Index{};
    Metadata_Type                                 Metadata;
  };
  template <u64 element_count, typename OFFSET_Type>
  struct Header<element_count, OFFSET_Type, void>
  {

    u64                                           Body_size{ 0 };
    std::array<INDEX<OFFSET_Type>, element_count> Body_Index{};
  };

  // optimize header
  // optimize alignment
  // fix Ds
  template <class C>
  concept is_header = requires( C x ) {
    { Header{ x } } -> std::same_as<C>;
  };
  template <u64         element_count, typename OFFSET_Type = u16,
            std::size_t alignment = std::hardware_destructive_interference_size,
            class Metadata = void>
  struct Data_Struture_Storage_Block
  {

    alignas( alignment ) Header<element_count, OFFSET_Type, Metadata> HEADER;

  public:
    Content _content;

    void set_index( std::array<alloc_info, element_count> &index_size )
    {
      // index calculationis done here
      u64 offset = 0;
      for ( u64 i = 0; i < element_count; i++ )
      {
        HEADER.Body_Index[ i ]._offset = offset;
        HEADER.Body_Index[ i ]._size = index_size[ i ].sz;
        offset += ( index_size[ i ].aligned_sz );
      }
    }
    explicit Data_Struture_Storage_Block(
        u64 SIZE, std::array<alloc_info, element_count> &index_size )
    {
      HEADER.Body_size = SIZE;
      set_index( index_size );
    }
    template <u64 index>
    [[nodiscard]] inline u8 &Acess_Data()
    {

      return _content.DATA[ HEADER.Body_Index[ index ]._offset ];
    }
    template <u64 index>

    [[nodiscard]] inline u64 Acess_Size() const
    {
      return HEADER.Body_Index[ index ]._size;
    }
    template <u64 index>
    inline void Set_Zero() const
    {
      std::memset( &Acess_Data( index ), 0, Acess_Size( index ) );
    }
    inline void Set_Zero_all()
    {
      std::memset( &_content.DATA[ 0 ], 0,
                   HEADER.Body_size - ( sizeof( HEADER ) ) );
    }
    inline bool check_pos( u64 index, void *pos ) const
    {
      void *spos = ( &Acess_Data( index ) );
      void *lpos = ( &Acess_Data( index ) + Acess_Size( index ) );
      if ( ( pos >= spos ) && ( pos <= lpos ) )
        return true;
      else
        return false;
    }
  };

} // namespace DS
