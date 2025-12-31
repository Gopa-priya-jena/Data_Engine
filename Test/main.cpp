#include "testbed.hpp"
#include <TYPE.hpp>
#include <iostream>
#include <utility>

void prnt( u64 index, const auto &ref )
{
  std::cout << typeid( ref ).name() << "\t" << ref << "\n";
}
template <u64... I>
inline void loop( auto &d, std::index_sequence<I...> )
{
  ( ( d.template Type<I>::data() = I, //
      prnt( I, d.template Type<I>::data() ) ),
    ... );
};
inline void Test()
{
  testing::Data1 d1;
  testing::Data2 d2;
  testing::Data3 d3;
  loop( d1, std::make_integer_sequence<u64, lc>{} );
  loop( d2, std::make_integer_sequence<u64, lc>{} );
  loop( d3, std::make_integer_sequence<u64, gc>{} );
}
int main()
{
  auto i = 1;
  while ( i-- )
    Test();
}
