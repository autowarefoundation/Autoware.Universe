// Copyright 2024 TIER IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef AUTOWARE__MTR__FIXED_QUEUE_HPP_
#define AUTOWARE__MTR__FIXED_QUEUE_HPP_

#include <cstddef>
#include <deque>
#include <iterator>

namespace autoware::mtr
{

template <typename T>
class FixedQueue
{
public:
  using size_type = typename std::deque<T>::size_type;
  using reference = typename std::deque<T>::reference;
  using const_reference = typename std::deque<T>::const_reference;
  using iterator = typename std::deque<T>::iterator;
  using riterator = typename std::reverse_iterator<iterator>;
  using const_iterator = typename std::deque<T>::const_iterator;
  using rconst_iterator = typename std::reverse_iterator<const_iterator>;

  explicit FixedQueue(size_type size) : queue_(size) {}

  void push_back(const T && t) noexcept
  {
    queue_.pop_front();
    queue_.push_back(t);
  }

  void push_back(const T & t) noexcept
  {
    queue_.pop_front();
    queue_.push_back(t);
  }

  void push_front(const T && t) noexcept
  {
    queue_.pop_back();
    queue_.push_front(t);
  }

  void push_front(const T & t) noexcept
  {
    queue_.pop_back();
    queue_.push_front(t);
  }

  reference front() noexcept { return queue_.front(); }
  const_reference front() const noexcept { return queue_.front(); }

  reference back() noexcept { return queue_.back(); }
  const_reference back() const noexcept { return queue_.back(); }

  iterator begin() noexcept { return queue_.begin(); }
  const_iterator begin() const noexcept { return queue_.begin(); }

  iterator end() noexcept { return queue_.end(); }
  const_iterator end() const noexcept { return queue_.end(); }

  riterator rbegin() noexcept { return queue_.rbegin(); }
  rconst_iterator rbegin() const noexcept { return queue_.rbegin(); }

  riterator rend() noexcept { return queue_.rend(); }
  rconst_iterator rend() const noexcept { return queue_.rend(); }

  size_type size() const noexcept { return queue_.size(); }

private:
  std::deque<T> queue_;
};
}  // namespace autoware::mtr
#endif  // AUTOWARE__MTR__FIXED_QUEUE_HPP_
