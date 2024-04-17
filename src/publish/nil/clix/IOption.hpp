#pragma once

namespace nil::clix {
struct IOption {
  struct Impl;

  IOption() = default;
  virtual ~IOption() noexcept = default;

  IOption(IOption &&) noexcept = delete;
  IOption &operator=(IOption &&) noexcept = delete;

  IOption(const IOption &) = delete;
  IOption &operator=(const IOption &) = delete;

  virtual void apply(const Impl &impl) const = 0;
};
} // namespace nil::clix
