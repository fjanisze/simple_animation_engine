#include "animation.hpp"
#include <gmock.h>

class animation_functions_test_small : public ::testing::Test
{
public:
    int get_y(int x)
    {
        double m=1;
        return m*(x+2)-3;
    }
    animation_engine::animation_functions functions;
    animation_functions_test_small():functions(sf::Vector2f(-2,-3),sf::Vector2f(-5,-6))
    {
    }
};

TEST_F(animation_functions_test_small,first_test)
{
    std::vector<sf::Vector2f> positions;
    ASSERT_EQ(4,functions.calculate_interpolation(positions));
    //Check the limit values
    ASSERT_EQ(-2,positions[0].x);
    ASSERT_EQ(-3,positions[0].y);
    ASSERT_EQ(-5,positions.back().x);
    ASSERT_EQ(-6,positions.back().y);
    //Check some values in the middle
    ASSERT_EQ(-4,positions[1].y);
    ASSERT_EQ(-5,positions[2].y);
}


class animation_functions_tests : public ::testing::Test
{
public:
    int get_y(int x)
    {
        double m=-0.54705882352941176470588235294118;
        return m*(x-120)-60;
    }
    animation_engine::animation_functions functions;
    animation_functions_tests():functions(sf::Vector2f(-50,33),sf::Vector2f(120,-60))
    {
    }
};

TEST_F(animation_functions_tests,first_test)
{
    std::vector<sf::Vector2f> positions;
    ASSERT_EQ(171,functions.calculate_interpolation(positions));
    //Check the limit values
    ASSERT_EQ(-50,positions[0].x);
    ASSERT_EQ(33,positions[0].y);
    ASSERT_EQ(120,positions.back().x);
    ASSERT_EQ(-60,positions.back().y);
    //Check some values in the middle
    ASSERT_EQ(get_y(positions[10].x),positions[10].y);
    ASSERT_EQ(get_y(positions[100].x),positions[100].y);
    ASSERT_EQ(get_y(positions[150].x),positions[150].y);
}

int main(int argc,char** argv)
{
    ::testing::InitGoogleMock(&argc,argv);
    return RUN_ALL_TESTS();
}
