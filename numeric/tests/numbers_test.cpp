#include "numbers.cpp"
#include <iostream>
#include <random>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>


template <typename T, int... Ints>
Number<T, Ints...> random_number(int max){
    using Num = Number<T, Ints...>;

    // Precompute all the possible combinations of sqrts
    auto array = precompute_array<Ints...>();

    // Initialize the random number generators
    std::random_device rd;
    std::mt19937 gen(rd());    
    int min = 1;
    std::uniform_int_distribution<> dis_pos(min, max);
    std::uniform_int_distribution<> dis(-max, max);
    std::uniform_int_distribution<> dis_sqrt(0, array.size()-1);

    unsigned n = dis_pos(gen);
    Num x{};
    for(unsigned i=0; i<n; i++){
        T num{dis(gen)}, den{dis_pos(gen)};
        int root{array[dis_sqrt(gen)]};
        FracRoot<T, Ints...> f{{num, den}, root};
        x += f;
    }
    return x;
}

template <typename T, int... Ints> bool randomized_is_pos_fractional(){
    
    unsigned n_random{100}, max{100};
    for(unsigned k=0; k<n_random; k++){
        auto x = random_number<T, Ints...>(max);
        
        int comparison1 = ((int)((double)x > 0 ))*2-1;
        int comparison2 = x.is_pos_fractional();
        if(comparison1 != comparison2){
            std::cout << "Error: Randomized testing failed for " << x << std::endl;
            return false;
        }
    }

    return true;
}

TEST_CASE("is_pos_fractional", "[custom]"){
    
    using T = long;
    FracRoot<T,2,5> a{{1,2},5};
    FracRoot<T,2,5> b{{-3,2},2};
    Number<T,2,5> x{a, b};

    REQUIRE(x.is_pos_fractional() == -1);
    REQUIRE((x-x).is_pos_fractional() == 0);
    REQUIRE((-x).is_pos_fractional() == 1);
}

TEST_CASE("Randomized  is_pos_fractional with mpz_class", "[custom]"){
    REQUIRE(randomized_is_pos_fractional<mpz_class,2,5>());
    REQUIRE(randomized_is_pos_fractional<mpz_class,2,5,13,17>());
    REQUIRE(randomized_is_pos_fractional<mpz_class,7>());
    REQUIRE(randomized_is_pos_fractional<int>());
}

TEST_CASE("is_pos_general", "[custom]"){
    using T = long;

    FracRoot<T,2,5> a{{1,2},5};
    FracRoot<T,2,5> b{{-3,2},10};
    Number<T,2,5> x{a, b}; 

    std::cout << x << "------Benchmwarks missing";
    REQUIRE(x.is_pos_general() == false);
    REQUIRE((x-x).is_pos_general() == false);
    REQUIRE((-x).is_pos_general() == true);
}

TEST_CASE("operators < > == for template <> ", "[custom]"){
    FracRoot<> a{{1,2},1};
    FracRoot<> b{{-3,4},1};
    FracRoot<> c{{0,4},1};
    Number<> x{a}, y{b}, z{c};
    REQUIRE(x>0);
    REQUIRE(y<0);
    REQUIRE(z==0);
}

TEST_CASE("operators < > == for template <2> ", "[custom]"){
    using T = int;
    using U = double;

    FracRoot<T,2> a{{1,2},2};
    FracRoot<T,2> b{{-3,4},1};
    FracRoot<T,2> c{{0,4},2};
    Number<T,2> x{a}, y{a,b}, z{c};

    REQUIRE((x>0) == ((U)x>0));
    REQUIRE((y>0) == ((U)y>0));
    REQUIRE((z<0) == ((U)z>0));
}

template <typename T, int... Ints> bool randomized_comparison_operators(){
    using U = double;
    unsigned n_random = 100;
    for(unsigned i=0; i<n_random; i++){
        auto x = random_number<T, Ints...>(100);
        auto y = random_number<T, Ints...>(100);
        
        if((x<y) != ((U)x < (U)y)){
            std::cout << "Error: Comparison operation < failed for "
                 << x << "(" << (U)x << ") and " << y << "(" << (U)y << ")" << std::endl;
            return false;
        }

        if((x>y) != ((U)x > (U)y)){
            std::cout << "Error: Comparison operation > failed for "
                 << x << "(" << (U)x << ") and " << y << "(" << (U)y << ")" << std::endl;
            return false;
        }
        
    }
    return true;
}

TEST_CASE("Randomized comparison operations", "[custom]"){
    REQUIRE(randomized_comparison_operators<mpz_class, 2, 3>());
    REQUIRE(randomized_comparison_operators<mpz_class, 2>());
    REQUIRE(randomized_comparison_operators<mpz_class, 2, 5, 13, 17>());
}

TEST_CASE("Number constructors", "[custom]"){
    using T = int;
    using U = double;

    Number<T, 2> x{2};
    Number<T, 1> y{2};
    REQUIRE(equal(2.0, (double)x));
    REQUIRE(equal(2.0, (double)y));

    FracRoot<T,2> a{{1,2},2}, b{{-3,4},1}, c{{0,4},2};
    Number<T,2> x1{a}, y1{a,b}, z{c};

    REQUIRE(equal((U)x1, 0.5*std::sqrt(2)));
    REQUIRE(equal((U)y1, 0.5*std::sqrt(2) - 0.75));
    REQUIRE(equal((U)z, 0.0));
}

template <typename U, int... Ints> bool number_operations(){
    FracRoot<U,Ints...> a{{1,2},5};
    FracRoot<U,Ints...> c{{4,2},10};
    Number<U,Ints...> x{a}, y{c};

    using T = double;
    return equal((T)x + (T)y ,(T)(x+y)) and equal((T)x * (T)y ,(T)(x*y));
}

template <typename U, int... Ints> bool randomized_number_operations(unsigned max){

    using T = double;

    unsigned n_random{100};
    for(unsigned k=0; k<n_random; k++){
        auto x = random_number<U, Ints...>(max);
        auto y = random_number<U, Ints...>(max);
        
        if(not equal((T)x + (T)y, (T)(x+y))){
            std::cout << "Error in randomized testing for number sum "
                 << x << " and " << y << std::endl;
            return false;
        }

        if(not equal((T)x * (T)y, (T)(x*y))){
            std::cout << "Error in randomized testing for number product "
                 << x << " and " << y << std::endl;
            return false;
        }
    }

    return true;
}

TEST_CASE("Number operations +*", "[custom]"){
    REQUIRE(number_operations<int, 2, 5>());
    REQUIRE(number_operations<long, 2, 5>());
    REQUIRE(number_operations<mpz_class, 2, 5>());
}

TEST_CASE("Randomized Number operations +*", "[custom]"){
    // When using int or long as data types, the sum of fractions
    // can overflow if too many denominators are different. Limiting 
    // that to 5 makes it unlikely to overflow

    REQUIRE(randomized_number_operations<long, 2, 5>(5));
    REQUIRE(randomized_number_operations<mpz_class, 2, 5>(100));

    REQUIRE(randomized_number_operations<long, 7, 13>(5));
    REQUIRE(randomized_number_operations<mpz_class, 7, 13>(100));
}

TEST_CASE("Unary Number operations -", "[custom]"){
    using T = int;
    using U = double;
    FracRoot<T,2,5> a{{1,2},5};
    FracRoot<T,2,5> b{{-3,2},2};
    Number<T,2,5> x{a,b};
    REQUIRE(equal(-(U)x, (U)(-x)));
}

TEST_CASE("is_zero", "[custom]"){
    using T = int;

    FracRoot<T,2,3> a{{1,2},2}, b{{0,1},3};
    Number<T,2,3> x{a}, y{a}, z{b};

    REQUIRE((x-y).is_zero());
    REQUIRE(z.is_zero());
}

TEST_CASE("conjugation", "[custom]"){
    using T = int;
    
    FracRoot<T,2,5> a{{3,1},1};
    FracRoot<T,2,5> b{{-3,2},2};
    FracRoot<T,2,5> c{{1,2},5};
    
    Number<T,2,5> z{a,b,c};
    Number<T,2,5> z2{a,-b,c};
    Number<T,2,5> z5{a,b,-c};

    REQUIRE(z.conjugate(2) == z2);
    REQUIRE(z.conjugate(5) == z5);
}

TEST_CASE("inverse", "[custom]"){

    Number<int,2,5> zi{1188};
    Number<long long,2,5> zl{1188};

    REQUIRE(zi*zi.inverse() != 1);
    REQUIRE(zl*zl.inverse() == 1);
    
    using T = int;
    FracRoot<T,2,5> a{{3,1},1};
    FracRoot<T,2,5> b{{-3,2},2};  
    FracRoot<T,2,5> c{{1,2},5};      
    Number<T,2,5> z{a,b,c};
    REQUIRE(z*z.inverse() == 1);
}

TEST_CASE("operator /", "[custom]"){
    using T = mpz_class;
    FracRoot<T,2,5> a{{3,1},1};
    FracRoot<T,2,5> b{{-3,2},2};   
    FracRoot<T,2,5> c{{1,2},5};     
    Number<T,2,5> x{a,b,c};
    Number<T,2,5> y{b,-c};
    Number<T,2,5> z{x/y};
    REQUIRE(y*z == x);
}

TEST_CASE("operator / int", "[custom]"){
    using T = int;

    FracRoot<T,2,5> a{{1891, 2}, 1};
    Number<T,2,5> x{1891}, y{a};
    REQUIRE(x/2 == y);
}

// TEST_CASE("Benchmark is_pos"){
//     using T = mpz_class;

//     FracRoot<T, 2,5,13,17> a{{1,2},5};
//     FracRoot<T, 2,5,13,17> b{{-9,4},13};
//     FracRoot<T, 2,5,13,17> c{{-2,3},34};
//     FracRoot<T, 2,5,13,17> d{{-21,10},2};
//     Number<T, 2,5,13,17> x{a,b,c,d};

//     BENCHMARK("is_pos_fractional"){
//         return is_pos_fractional<T, 2,5,13,17>(x);
//     };

//     BENCHMARK("is_pos_general"){
//         return is_pos_general<T, 2,5,13,17>(x);
//     };
// }
