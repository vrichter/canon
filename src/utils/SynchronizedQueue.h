/********************************************************************
**                                                                 **
** File   : src/utils/SynchronizedQueue.h                          **
** Authors: Viktor Richter                                         **
**                                                                 **
**                                                                 **
** GNU LESSER GENERAL PUBLIC LICENSE                               **
** This file may be used under the terms of the GNU Lesser General **
** Public License version 3.0 as published by the                  **
**                                                                 **
** Free Software Foundation and appearing in the file LICENSE.LGPL **
** included in the packaging of this file.  Please review the      **
** following information to ensure the license requirements will   **
** be met: http://www.gnu.org/licenses/lgpl-3.0.txt                **
**                                                                 **
********************************************************************/

#ifndef CANON_SYNCHRONIZEDQUEUE_H
#define CANON_SYNCHRONIZEDQUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>
#include <thread>
#include <iostream>

namespace canon {
namespace utils {

template <typename Data> class SynchronizedQueue {
public:
  typedef std::mutex Mutex;
  typedef std::unique_lock<Mutex> Lock;
  typedef std::condition_variable ConditionVariable;

  SynchronizedQueue(size_t maximum_size) : max_size(maximum_size), exit(false) {}

  ~SynchronizedQueue() {
    exit.store(true);
    condition.notify_all();
    Lock lock(mutex);
  }

  SynchronizedQueue& push(Data const &data) {
    if(max_size == 0) return *this;
    Lock lock(mutex);
    if (queue.size() >= max_size) {
      queue.pop();
    }
    queue.push(data);
    lock.unlock();
    condition.notify_one();
    return *this;
  }

  bool empty() const {
    Lock lock(mutex);
    return queue.empty();
  }

  bool try_pop(Data &popped_value) {
    Lock lock(mutex);
    if (queue.empty()) {
      return false;
    }
    popped_value = queue.front();
    queue.pop();
    return true;
  }

  bool pop(Data &data) {
    Lock lock(mutex);
    while (queue.empty()) {
      if (exit.load()) {
        return false;
      }
      condition.wait(lock);
    }
    if(queue.empty() || exit.load()){
      return false;
    } else {
      data = queue.front();
      queue.pop();
      return true;
    }
  }

private:
  std::queue<Data> queue;
  mutable Mutex mutex;
  ConditionVariable condition;
  size_t max_size;
  std::atomic<bool> exit;
};

} // namespace utils
} // namespace canon

#endif /* !CANON_SYNCHRONIZEDQUEUE_H */
