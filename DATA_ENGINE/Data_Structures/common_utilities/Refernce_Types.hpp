#pragma once

namespace TS
{
  template <typename T>
  struct Ref_get
  {
    using Type = T &;
  };
  template <>
  struct Ref_get<void>
  {
    using Type = void;
  };
} // namespace TS
