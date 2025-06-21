#include <utility>
#include <numeric>
#include <array>
#include <cmath>

constexpr int search(int start, int end, int target){
    int mid = (start+end)/2;
    if(mid*mid == target)
        return mid;
    else if(start + 1 == end)
        return start;
    else if(mid*mid > target)
        return search(start, mid, target);
    else 
        return search(mid, end, target);
}

constexpr int nearest_perfect_sqrt(int N){
    return search(0, N, N);
}


constexpr std::pair<int, int> find_fractional_approximation(int N, int num_iterations){
    
    int a = nearest_perfect_sqrt(N);
    int r = N - a*a;

    int num = a;
    int den = 1;
    for(int i=0; i<num_iterations; i++){
        int temp_num = num;
        num = a*(den*a + num) + r*den;
        den = den*a + temp_num;
        int gcd = std::gcd(num, den);
        num /= gcd;
        den /= gcd;
    }

    return {num, den};

}

template <typename Integer>
struct Limits{
    Integer lower_num, lower_den, upper_num, upper_den;
};

template <typename Integer>
constexpr Limits<Integer> find_bounds(Integer N, int num_iterations){
    
    Limits<Integer> limits{};
    Integer a = nearest_perfect_sqrt(N);
    Integer r = N - a*a;

    Integer num = a;
    Integer den = 1;
    for(int i=0; i<num_iterations; i++){
        Integer temp_num = num;
        num = a*(den*a + num) + r*den;
        den = den*a + temp_num;
        Integer gcd = std::gcd(num, den);
        num /= gcd;
        den /= gcd;

        if(num*num < N*den*den){
            limits.lower_den = den;
            limits.lower_num = num;
        } else {
            limits.upper_den = den;
            limits.upper_num = num;
            
        }
    }

    return limits;

}

template <typename Integer>
constexpr Limits<Integer> find_bounds(Integer N, double precision){
    
    Limits<Integer> limits{};
    Integer a = nearest_perfect_sqrt(N);
    Integer r = N - a*a;

    Integer num = a;
    Integer den = 1;
    double eps = 2*precision;
    bool upper_updated{false}, lower_updated{false};
    do {
        Integer temp_num = num;
        num = a*(den*a + num) + r*den;
        den = den*a + temp_num;
        Integer gcd = std::gcd(num, den);
        num /= gcd;
        den /= gcd;

        if(num*num < N*den*den){
            limits.lower_den = den;
            limits.lower_num = num;
            lower_updated = true;
        } else {
            limits.upper_den = den;
            limits.upper_num = num;
            upper_updated = true;
            
        }
        if(lower_updated and upper_updated)
            eps = (double)limits.lower_num/limits.lower_den - (double)limits.upper_num/limits.upper_den;;
        // eps = eps < 0? -eps: eps;
        
    } while(std::abs(eps) > precision);

    return limits;

}

template <typename T>
constexpr int return2([[maybe_unused]] T x){
    return 2;
}


// Variadic
template <int... Ints>
constexpr int pow2v(){
    return (return2(Ints) * ...);
}


template <int N>
constexpr int pow2(){
    return pow2<N-1>()*2;
}

template <>
constexpr int pow2<0>(){
    return 1;
}

template <typename U>
constexpr U precompute_array_N(){
    U fractions{};

    for(unsigned i=0; i<fractions.size(); i++)
        fractions[i] = 1;

    return fractions;
}


template <typename U, int A, int... Ints>
constexpr U precompute_array_N(){
    
    constexpr int pow = pow2v<A, Ints...>()/2;
    U fractions{};

    auto new_fractions = precompute_array_N<U, Ints...>();
    for(unsigned i=0; i<fractions.size(); i++){
        fractions[i] = new_fractions[i]*(((i/pow) % 2 == 0)? fractions[i] = 1: fractions[i] = A);
    }
            
    
    return fractions;
}

template <int... Ints>
constexpr std::array<int, pow2<sizeof...(Ints)>()> precompute_array(){
    return precompute_array_N<std::array<int, pow2<sizeof...(Ints)>()>, Ints...>();
}


template <typename T, int...Ints>
constexpr std::array<Limits<T>, (Ints * ...)> populate_array(){
    constexpr int size = pow2v<Ints...>();
    std::array<int, size> fractions = precompute_array<Ints...>();

    std::array<Limits<T>, (Ints * ...)> hash{};
    for(int i=1; i<size; i++){
        int j = fractions[i];
        hash[j-1] = find_bounds<T>(j, 1e-6);
    }
    hash[0] = Limits<T>{1,1,1,1};
    return hash;
}


template <typename T>
constexpr std::array<Limits<T>, 1> populate_array(){
    std::array<Limits<T>, 1> hash{};
    hash[0] = Limits<T>{1,1,1,1};
    return hash;
}
