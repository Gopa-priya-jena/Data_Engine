#pragma once
#include <cstddef>
#if defined( __clang__ )
  #define GCC 0
  #define CLANG 1
  #define MSVC 0
#elif defined( __GNUG__ )
  #define CLANG 0
  #define GCC 1
  #define MSVC 0
#elif defined( _MSC_VER )
  #define MSVC 1
  #define CLANG 0
  #define GCC 0
#endif
#include <cstdint>
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
#if GCC
using u128 = unsigned __int128;
#endif

// signed  integer  variants
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;
#if GCC
using s128 = __int128;
#endif

// floating types
#if GCC
  #include <stdfloat>
using f16 = std::float16_t;
using f32 = std::float32_t;
using f64 = std::float64_t;
using f128 = std::float128_t;
#elif CLANG
  #include <bits/floatn.h>
using f16 = _Float16;
using f32 = _Float32;
using f64 = _Float64;
using f128 = _Float128;
#elif MSVC
using f16 = _Float16;
using f32 = float;
using f64 = double;
using f128 = long double;
#endif
using SZ = std::size_t;
