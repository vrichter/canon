/********************************************************************
**                                                                 **
** Copyright (C) 2014 Viktor Richter                               **
**                                                                 **
** File   : test/Subjects.cpp                                    **
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

#include "utils/Subject.h"

#include "gtest/gtest.h"

namespace {

typedef ::canon::utils::Subject<int> Subject;
typedef ::canon::utils::CompositeSubject<int> CompositeSubject;

class Subscriber {
public:
  std::vector<int> history;
  void update(int new_data) { history.push_back(new_data); }
};

TEST(SubjectTest, Constructor) {
  EXPECT_NO_THROW(Subject());
  EXPECT_NO_THROW(CompositeSubject({std::make_shared<Subject>(),
                                    std::make_shared<Subject>(),
                                    std::make_shared<Subject>()}));
}

TEST(SubjectTest, NoSubscribers) {
  Subject subject;
  EXPECT_NO_THROW(subject.notify(1));
}

TEST(SubjectTest, Connect) {
  Subject subject;
  Subscriber subscriber;
  // connect
  auto connection = subject.connect([ &subscriber ](int data) { subscriber.update(data);} );
  EXPECT_TRUE(connection.connected());
  EXPECT_TRUE(subscriber.history.empty());
  // notify
  subject.notify(1);
  subject.notify(2);
  EXPECT_EQ(2,subscriber.history.size());
  EXPECT_EQ(1,subscriber.history.at(0));
  EXPECT_EQ(2,subscriber.history.at(1));
}

TEST(SubjectTest, DisconnectConnection) {
  Subject subject;
  Subscriber subscriber;
  // connect
  auto connection = subject.connect([ &subscriber ](int data) { subscriber.update(data);} );
  EXPECT_TRUE(connection.connected());
  EXPECT_TRUE(subscriber.history.empty());
  // notify
  subject.notify(1);
  // disconnect
  connection.disconnect();
  EXPECT_FALSE(connection.connected());
  subject.notify(2);
  EXPECT_EQ(1,subscriber.history.size());
  EXPECT_EQ(1,subscriber.history.front());
}

TEST(SubjectTest, DisconnectSubject) {
  Subject subject;
  Subscriber subscriber;
  // connect
  auto connection = subject.connect([ &subscriber ](int data) { subscriber.update(data);} );
  EXPECT_TRUE(connection.connected());
  EXPECT_TRUE(subscriber.history.empty());
  // notify
  subject.notify(1);
  // disconnect
  subject.disconnect(connection);
  EXPECT_FALSE(connection.connected());
  subject.notify(2);
  EXPECT_EQ(1,subscriber.history.size());
  EXPECT_EQ(1,subscriber.history.front());
}

TEST(SubjectTest, DeleteSubject) {
  std::unique_ptr<Subject> subject(new Subject());
  Subscriber subscriber;
  // connect
  auto connection = subject->connect([ &subscriber ](int data) { subscriber.update(data);} );
  EXPECT_TRUE(connection.connected());
  EXPECT_TRUE(subscriber.history.empty());
  // notify
  subject->notify(1);
  // delete
  subject.reset();
  EXPECT_FALSE(connection.connected());
}


}
