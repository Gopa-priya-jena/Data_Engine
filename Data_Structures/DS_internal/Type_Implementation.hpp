
#pragma once
#include <DS_internal/DS_Designer_helper.hpp>
#include <TYPE.hpp>
#include <type_traits>
namespace DS
{
  template <class Derived, u64 index, class Type, class _meta = void>
  struct Type_Implementation
  {

  public:
    using metadata = _meta;
    [[nodiscard]] inline Type &data()
    {
      return reinterpret_cast<Type &>(
          static_cast<Derived &>( *this ).template Acess_DATA<index>() );
    }
    [[nodiscard]] inline u64 getsize()
    {
      return static_cast<Derived &>( *this ).template Acess_DATA<index>(
          index );
    }
    inline TS::Ref_get<metadata>::Type getmetadata()
    {
      ;
      if constexpr ( !std::is_void_v<metadata> )
        return static_cast<Derived &>( *this )
            .template Acess_SelfMetadata<index>();
    }
    inline TS::Ref_get<metadata>::Type getcommonmetadata()
    {
      ;
      if constexpr ( !std::is_void_v<metadata> )
        return static_cast<Derived &>( *this )
            .template Acess_CommonMetadata<index>();
    }
  };

  template <class REF, u64 index, class Type>
    requires std::is_fundamental_v<Type>
  struct static_Type : Type_Implementation<REF, index, Type, void>
  {
    using Base = Type_Implementation<REF, index, Type, i64>;
    [[nodiscard]] inline constexpr u64 allocate()
    {
      ;
      return sizeof( Type );
    }
    static_Type &operator=( Type &&t )
    {
      this->data() = t;
      return *this;
    }

    static_Type &operator=( Type &t )
    {
      this->data() = t;
      return *this;
    }
    void Terminate()
    {
    }
    void Initialize()

    {
      if constexpr ( std::is_void_v<typename Base::metadata> )
      {
        Base::getmetadata() = 38;
        Base::getcommonmetadata() = 0;
      }
      else
      {
        return;
      }
    }
  };
  template <typename Type>
  using Basic = TS::Type_decl<static_Type, Type>;
} // namespace DS
