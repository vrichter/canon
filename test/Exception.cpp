/********************************************************************
**                                                                 **
** Copyright (C) 2014 Viktor Richter                               **
**                                                                 **
** File   : test/Exceptions.cpp                                    **
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

#include "utils/Exception.h"

#include "gtest/gtest.h"

namespace {

using ::canon::utils::Exception;

TEST(ExceptionTest, Constructor) {
  std::string error_text = "ERROR_TEXT";
  try {
    throw Exception(error_text);
  } catch (const Exception &e) {
    EXPECT_STREQ(e.what(),error_text.c_str());
  }
}

}
