#pragma once

#include <DS_internal/DS_Designer_helper.hpp>
#include <DS_internal/Type_Implementation.hpp>
#include <TYPE.hpp>

#define copy_0( ... ) __VA_ARGS__
#define copy_10( ... )                                                           \
  __VA_ARGS__                                                                  \
  __VA_ARGS__                                                                  \
  __VA_ARGS__                                                                  \
  __VA_ARGS__                                                                  \
  __VA_ARGS__                                                                  \
  __VA_ARGS__                                                                  \
  __VA_ARGS__                                                                  \
  __VA_ARGS__                                                                  \
  __VA_ARGS__                                                                  \
  __VA_ARGS__

#define copy_100( ... ) copy_10(copy_10(__VA_ARGS__))
#define copy_1000( ... ) copy_10(copy_100(__VA_ARGS__))
#define copy_10000( ... ) copy_10(copy_1000(__VA_ARGS__))
#define copy_100000( ... ) copy_10(copy_10000(__VA_ARGS__))

#define TypeTestlist    copy_100(Basic<i8>, Basic<i16>, Basic<i32>, Basic<i64>, Basic<u8>, Basic<u16>, \
         Basic<u32>, Basic<u64>, Basic<i16>, Basic<f32>, Basic<f64>, )         \
      Basic<u64>

#define glist                                                                  \
  copy_100(TS::group_info<i64, Basic<i8>, Basic<i16>, Basic<i32>, Basic<i64>>,       \
         TS::group_info<u64, Basic<u8>, Basic<u16>, Basic<u32>, Basic<u64>>,       \
         TS::group_info<f64, Basic<f32>, Basic<f32>, Basic<f64>,  Basic<f128>>, )                \
      TS::group_info<int, Basic<float>, Basic<i64>, Basic<u64>>

constexpr auto lc = 11 * 100 + 1;
constexpr auto gc = 11 * 100 + 2;
constexpr auto align_sz = 64;
namespace testing
{
  using namespace DS;

  using soa = DS_Handle_maker<u16, void, std::allocator<u8>, align_sz, true,
                              TypeTestlist>::SOA::Type;

  using sbd = DS_Handle_maker<u16, void, std::allocator<u8>, align_sz, true,
                              TypeTestlist>::SGD::Type;
  using mgs = DS_Handle_maker<u16, void, std::allocator<u8>, align_sz, true,
                              glist>::MGS::Type;

  class Data1 : public sbd
  {
  public:
    using U3x = sbd::Type<0>;
    template <u64 n>
    using Type = sbd::Type<n>;

    Data1()
    {
    }
    ~Data1()
    {
    }
  };
  class Data2 : public soa
  {

  public:
    using U3x = soa ::Type<0>;
    template <u64 n>
    using Type = soa::Type<n>;

    Data2()
    {
    }
    ~Data2()
    {
    }
  };

  class Data3 : public mgs
  {

  public:
    using U3x = mgs::Type<0>;
    template <u64 n>
    using Type = mgs::Type<n>;

    Data3()
    {
    }
    ~Data3()
    {
    }
  };

} // namespace testing
