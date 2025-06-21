#include "approx_sqrt.cpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <algorithm>

TEST_CASE("nearest_perfect_sqrt", "[custom]"){
    REQUIRE(nearest_perfect_sqrt(10)==3);
    REQUIRE(nearest_perfect_sqrt(6)==2);
    REQUIRE(nearest_perfect_sqrt(15)==3);
    REQUIRE(nearest_perfect_sqrt(16)==4);
    REQUIRE(nearest_perfect_sqrt(20)==4);
}

TEST_CASE("find_fractional_approximation", "[custom]"){
    constexpr std::pair<int, int> pair = find_fractional_approximation(2,1);
    REQUIRE(pair.first == 3);
    REQUIRE(pair.second == 2);

    constexpr std::pair<int, int> pair2 = find_fractional_approximation(2,2);
    REQUIRE(pair2.first == 7);
    REQUIRE(pair2.second == 5);

    constexpr std::pair<int, int> pair3 = find_fractional_approximation(2,3);
    REQUIRE(pair3.first == 17);
    REQUIRE(pair3.second == 12);

    constexpr std::pair<int, int> pair4 = find_fractional_approximation(6,4);
    REQUIRE(pair4.first == 218);
    REQUIRE(pair4.second == 89);
}

TEST_CASE("find_bounds", "[custom]"){
    constexpr long N = 21;
    constexpr Limits limits = find_bounds(N, 1e-6);
    constexpr double lower = (double)limits.lower_num/limits.lower_den;
    constexpr double upper = (double)limits.upper_num/limits.upper_den;
    REQUIRE(lower*lower < N);
    REQUIRE(upper*upper > N);
    REQUIRE(std::abs(lower*lower - N) < 1e-5);
    REQUIRE(std::abs(upper*upper - N) < 1e-5);
}

TEST_CASE("precompute_array", "[custom]"){
    std::array<int, 8> array = precompute_array<2, 3, 5>();
    std::sort(array.begin(), array.end());
    REQUIRE(array == std::array<int, 8>{1,2,3,5,6,10,15,30});    
}

TEST_CASE("populate_array", "[custom]"){
    using T = long;
    constexpr auto arr2 = precompute_array<2, 5, 13,17>();
    constexpr auto arr3 = populate_array<T, 2, 5, 13, 17>();

    for(auto& N: arr2){
        
        auto& limits = arr3[N-1];

        double lower = (double)limits.lower_num/limits.lower_den;
        double upper = (double)limits.upper_num/limits.upper_den;

        REQUIRE(std::abs(lower*lower - N) < 1e-4);
        REQUIRE(std::abs(upper*upper - N) < 1e-4);

    }
}

