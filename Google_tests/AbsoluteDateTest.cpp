#include "gtest/gtest.h"
#include "../src/Gregorian.h"

// TEST() is useful when you want to write unit tests for static or global functions or simple classes.
TEST(AbsoluteDateTestSuite, ExampleDate){ // 12/2/2020 -> 737761
GregorianDate gregDate;
gregDate.SetMonth(12);
gregDate.SetDay(2);
gregDate.SetYear(2020);

EXPECT_EQ(gregDate.getAbsoluteDate(),737761);
}


TEST(AbsoluteDateTestSuite, IncorrectDate){ // 12/0/2020 -> 0
GregorianDate gregDate;
gregDate.SetMonth(12);
gregDate.SetDay(0);
gregDate.SetYear(2020);

ASSERT_EQ(gregDate.getAbsoluteDate(),0);
}