// tests.cpp
//#include <string>
#include "Lexer/Lexer.h"
#include "Lexer/Token.h"
#include "Lexer/FunctionLex.cpp"
#include <gtest/gtest.h>
 
TEST(SquareRootTest, PositiveNos) { 
    //string str = "0123";
    //int SizeSubline = 0, LineSize = str.size(), LineIndex = 0;
    EXPECT_EQ(1, isletter('3'));
    //ASSERT_EQ(0, squareRoot(0.0));
}
 
TEST(SquareRootTest, NegativeNos) {
    //ASSERT_EQ(-1.0, squareRoot(-15.0));
    //ASSERT_EQ(-1.0, squareRoot(-0.2));
}
 
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}