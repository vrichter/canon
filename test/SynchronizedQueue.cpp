/********************************************************************
**                                                                 **
** Copyright (C) 2014 Viktor Richter                               **
**                                                                 **
** File   : test/SynchronizedQueues.cpp                            **
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

#include "utils/SynchronizedQueue.h"

#include "gtest/gtest.h"

#include <future>

namespace {

typedef ::canon::utils::SynchronizedQueue<int> Queue;

TEST(SynchronizedQueueTest, Constructor) {
  // constructor does not throw
  EXPECT_NO_THROW(Queue(0));
  EXPECT_NO_THROW(Queue(1));
}

TEST(SynchronizedQueueTest, Empty) {
  // new queues are empty
  EXPECT_TRUE(Queue(0).empty());
  EXPECT_TRUE(Queue(1).empty());
  // max-size 0 queue is always empty
  EXPECT_TRUE(Queue(0).push(1).empty());
  // non-empty after push
  EXPECT_FALSE(Queue(1).push(1).empty());
  // empty again after pop
  int dst = 0;
  Queue q(1);
  q.push(1);
  q.pop(dst);
  EXPECT_TRUE(q.empty());
}

TEST(SynchronizedQueueTest, PushPop) {
  int dst = 0;
  Queue q(2);
  q.push(1);
  q.pop(dst);
  EXPECT_EQ(1, dst);
  EXPECT_TRUE(q.empty());

  q.push(1);
  q.push(2);
  q.push(3); // more than max elements. 1 gets popped
  q.pop(dst);
  EXPECT_EQ(2, dst);
  q.pop(dst);
  EXPECT_EQ(3, dst);
  EXPECT_TRUE(q.empty());
}

TEST(SynchronizedQueueTest, TryPop) {
  int dst = 0;
  Queue q(1);
  EXPECT_FALSE(q.try_pop(dst));
  EXPECT_EQ(0, dst);
  q.push(1);
  q.try_pop(dst);
  EXPECT_EQ(1, dst);
  EXPECT_TRUE(q.empty());
}

TEST(SynchronizedQueueTest, LockingOnEmpty) {
  Queue q(1);
  std::future<int> first_pop = std::async(std::launch::async, [&q]() {
    int i = 0;
    q.pop(i);
    return i;
  });
  std::future_status status = first_pop.wait_for(std::chrono::milliseconds(10));
  EXPECT_EQ(std::future_status::timeout,status);
  q.push(5);
  status = first_pop.wait_for(std::chrono::milliseconds(10));
  EXPECT_EQ(std::future_status::ready,status);
  EXPECT_EQ(5,first_pop.get());
}

TEST(SynchronizedQueueTest, Destruction) {
  std::unique_ptr<Queue> q(new Queue(1));
  std::future<std::pair<bool,int>> first_pop = std::async(std::launch::async, [&q]() {
    auto result = std::make_pair(true,0);
    result.first = q->pop(result.second);
    return result;
  });
  // make sure the queue does not get deleted before the thread locks
  std::future_status status = first_pop.wait_for(std::chrono::milliseconds(10));
  EXPECT_EQ(std::future_status::timeout, status);
  q.reset();
  status = first_pop.wait_for(std::chrono::milliseconds(10));
  EXPECT_EQ(std::future_status::ready,status);
  auto result = first_pop.get();
  EXPECT_FALSE(result.first);
  EXPECT_EQ(0,result.second);
}

}
