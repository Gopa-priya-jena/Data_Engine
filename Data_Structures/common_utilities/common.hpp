#pragma once
#include <TYPE.hpp>
#include <tuple>

namespace TS
{

  template <typename T>
  struct maybe
  {
    T data;
  };

  template <>
  struct maybe<void>
  {
  };
  template <template <typename T, u64, typename... Args> class handle, u64 I,
            typename... Args>
  struct Type_impl
  {
    static const u64 Index = I;
    template <typename T>
    using Type = handle<T, I, Args...>;
  };

  template <template <typename T, u64, typename... Args> class handle,
            typename... Args>
  struct Type_decl
  {
    template <u64 I>
    using Type = Type_impl<handle, I, Args...>;
  };
  template <typename Common_Data, typename... T>
  struct group_info
  {
    static const u64 E_count = sizeof...( T );
    template <u64 N>
    using Type = std::tuple_element<N, std::tuple<T...>>::type;
    using C_Type = Common_Data;
  };

  template <const auto map_arr, typename... T>
  struct group_helper
  {
    static const u64 E_count = sizeof...( T );
    template <u64 N>
    using Type = std::tuple_element<N, std::tuple<T...>>::type;
    static auto map = map_arr;
  };

  template <typename Offset, typename metadata, typename allocator_,
            std::size_t align_size, bool internal_align, typename mg = void>
  struct design
  {
    using offset_type = Offset;
    using common_metadata = metadata;
    using allocator = allocator_;
    using mg_design = mg;
    static constexpr std::size_t Alignment_requirements = align_size;
    static constexpr bool        internal_alignment = internal_align;
  };

  template <typename I, u64 sz, auto T>
  struct reindex
  {
    template <I... i>
    static consteval auto gen_index( std::index_sequence<i...> )
    {
      return std::integer_sequence<I, T[ i ]...>{};
    }
    using Index = decltype( gen_index( std::make_index_sequence<sz>{} ) );
  };

} // namespace TS
