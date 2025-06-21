#pragma once

#include "numbers.hpp"
#include "approx_sqrt.cpp"
#include <vector>
#include <cassert>
#include <gmpxx.h>



template <typename T, int... Ints>
template <typename... U>
Number<T, Ints...>::Number(U... digit_list){
    // std::cout << "Entered constructor T... digit_list" << std::endl;
    Number x{};
    (x += ... += digit_list);
    digits = std::move(x.digits);
    // number_double = (double)(*this);
}

template <typename T, int... Ints>
Number<T, Ints...>::Number(Number const& other):digits{other.digits}{}

template <typename T, int... Ints>
Number<T, Ints...>::Number(Number&& other):
            digits{std::move(other.digits)}{
    

}

// Constructor delegation
template <typename T, int... Ints>
Number<T, Ints...>::Number(int num, int den, int root):
    Number(FracRoot<T, Ints...>{{num, den}, root}){}


template <typename T, int... Ints>
Number<T, Ints...>& Number<T, Ints...>::operator=(Number const& other){
    if(this == &other)
        return *this;
    digits = other.digits;
    // number_double = (double)(*this);
    return *this;
}


template <typename T, int... Ints>
Number<T, Ints...>& Number<T, Ints...>::operator=(Number&& other){
    if(this == &other)
        return *this;
        
    digits = std::move(other.digits);
    
    return *this;
}







template <typename T, int... Ints>
bool Number<T, Ints...>::is_zero() const{
    return digits.empty();
}

template <typename T, int... Ints>
Number<T, Ints...> Number<T, Ints...>::conjugate(int prime) const {


    Number<T, Ints...> conj{}; 
    
    for(auto& digit: digits){
        auto& root = digit.first;
        auto& fracroot = digit.second;
        if(root % prime == 0){
            conj += -fracroot;
        } else {
            conj += fracroot;
        }
    }
    return conj;
}

template <typename T, int... Ints>
Number<T, Ints...> Number<T, Ints...>::inverse() const {
    // This has numerical precision problems
    Number<T, Ints...> num{1}, den{*this};

    ([&](auto prime){
        Number<T, Ints...> conj = den.conjugate(prime);
        num = num*conj;
        den = den*conj;
        // std::cout << "num, den: " << num << " " << den << std::endl;
    }(Ints), ...);

    return num*Fraction<T>{den.digits[1].get_den(),den.digits[1].get_num()};

}


template <typename T, int... Ints>
int Number<T, Ints...>::is_pos_fractional() const {
    // Returns 1 if it's positive, -1 if negative, and 0 if it cannot be determined
    constexpr auto frac_approx = populate_array<long, Ints...>();

    Fraction<T> f_lower{0,1}, f_upper{0,1};
    for(auto& [root, digit]: digits){
        Fraction<T> a{digit.get_num(), digit.get_den()};
        Limits<long> lims = frac_approx[root-1];

        // std::cout << root << std::endl;
        Fraction<T> lower{(T)lims.lower_num, (T)lims.lower_den};
        Fraction<T> upper{(T)lims.upper_num, (T)lims.upper_den};

        if(a.get_num() > 0){
            f_lower = f_lower + lower*a;
            f_upper = f_upper + upper*a;
        } else {
            f_lower = f_lower + upper*a;
            f_upper = f_upper + lower*a;
        }
        
        // std::cout << f_lower << ":" << lower << " " << root << " " << upper << std::endl;
        

    }

    if(f_lower.get_num() > 0)
        return 1;
    if(f_upper.get_num() < 0)
        return -1;
    else
        return 0;
}

template <typename T, int A, int... Ints>
bool is_pos_general_f(Number<T, A, Ints...> const & x){

    if(x == 0){
        return false;
    }
        

    Number<T, Ints...> a{}, b{}; 
    
    for(auto& [root, digit]: x.digits){
        T num = digit.get_num();
        T den = digit.get_den();
        if(root % A == 0){
            b += FracRoot<T, Ints...>{Fraction<T>{num, den}, root/A};
        } else {
            a += FracRoot<T, Ints...>{Fraction<T>{num, den}, root};
        }
    }

    if(a == 0){
        return is_pos_general_f(b);
    } else if(b == 0){
        return is_pos_general_f(a);
    }

    bool a_pos = is_pos_general_f(a);
    bool b_pos = is_pos_general_f(b);

    if(a_pos and b_pos) return true;
    if(!a_pos and !b_pos) return false;

    Number<T, Ints...> c = a*a - b*b*A; // numeric precision problem!
    bool c_pos = is_pos_general_f(c);
    return (a_pos and c_pos) or (!a_pos and !c_pos);
}

// This will only match when there's an empty parameter stack
template <typename T, int... Ints>
bool is_pos_general_f(Number<T, Ints...> const& x){
    if(x == 0) {
        return false;
    } else {
        auto& [root, digit] = *x.digits.begin();
        return digit.get_num()*digit.get_den() > 0;

    }   
}

template <typename T, int... Ints>
bool Number<T, Ints...>::is_pos_general() const{
    return is_pos_general_f(*this);
}

template <typename T, int... Ints>
bool Number<T, Ints...>::is_pos() const {
#if APPROX_COMPARISON == 1
    int fractional_compare = is_pos_fractional();
    if(fractional_compare == 0)
        return this->is_pos_general();
    else 
        return fractional_compare > 0;
#else 
    return this->is_pos_general();
#endif
}


    
template <typename T, int... Ints>
bool Number<T, Ints...>::operator==(Number const& x) const{
    // std::cout << "Entering == number" << std::endl;
    Number y{*this-x};
    return y.is_zero();
}

template <typename T, int... Ints>
bool Number<T, Ints...>::operator==(int x) const{
    Number y{x};
    return *this==y;
}

template <typename T, int... Ints>
template <typename U>
bool Number<T, Ints...>::operator!=(U const& x) const{
    return !(*this==x);
}

template <typename T, int... Ints>
bool Number<T, Ints...>::operator>(Number const& x) const{
    return (*this-x).is_pos();
}

template <typename T, int... Ints>
bool Number<T, Ints...>::operator>(int x) const{
    Number y{x};
    y -= *this;
    return !y.is_pos() and !(y==0);
}


template <typename T, int... Ints>
bool Number<T, Ints...>::operator<(Number const& x) const{
    return !(*this==x) and !(*this>x);
}

template <typename T, int... Ints>
bool Number<T, Ints...>::operator<(int x) const{
    return !(*this==x) and !(*this>x);
}


template <typename T, int... Ints>
template <typename U>
bool Number<T, Ints...>::operator>=(U const& x) const{
    return !(*this < x);
}



template <typename T, int... Ints>
template <typename U>
bool Number<T, Ints...>::operator<=(U const& x) const{
    return !(*this > x);
}


template <typename T, int... Ints>
Number<T, Ints...> Number<T, Ints...>::operator+(T x) const{
    Number novo{*this};

    // If the fraction is zero, do nothing
    if(x == 0) 
        return novo;

    if(novo.digits.count(1) == 0){
        novo.digits[1] = FracRoot<T, Ints...>({x,1},1);
    } else {
        FracRoot<T, Ints...>& digit = novo.digits[1];
        digit = digit + x;
    }
    return novo;
}


template <typename T, int... Ints>
Number<T, Ints...>& Number<T, Ints...>::operator+=(T x){
    *this = *this + x;
    number_double = (double)(*this);
    return *this;
}



template <typename T, int... Ints>
Number<T, Ints...> Number<T, Ints...>::operator+(Fraction<T> const& x) const{
    return (*this) + FracRoot<T, Ints...>(x, 1);
}


template <typename T, int... Ints>
Number<T, Ints...>& Number<T, Ints...>::operator+=(Fraction<T> const& x){
    *this = *this + x;
    number_double = (double)(*this);
    return *this;
}


template <typename T, int... Ints>
Number<T, Ints...> Number<T, Ints...>::operator+(FracRoot<T, Ints...> const& x) const{
    Number novo{*this};
    int r = x.get_root();

    // If the fraction is zero, do nothing
    if(x.get_num() == 0) 
        return novo;

    if(novo.digits.count(r) == 0){
        novo.digits[r] = x;
    } else {
        FracRoot<T, Ints...>& digit = novo.digits[r];
        digit = digit + x;
    }
    return novo;
}


template <typename T, int... Ints>
Number<T, Ints...>& Number<T, Ints...>::operator+=(FracRoot<T, Ints...> const& x){
    *this = *this + x;
    number_double = (double)(*this);
    return *this;
}


template <typename T, int... Ints>
Number<T, Ints...> Number<T, Ints...>::operator+(Number const& a) const{
    auto it1 = digits.begin();
    auto it2 = a.digits.begin();

    Number sum;
    while(it1!=digits.end() and it2!=a.digits.end()){
        int root1 = it1->first;
        int root2 = it2->first;

        if(root1 == root2){
            FracRoot<T, Ints...> plus = it1->second + it2->second;
            if(plus != 0)
                sum.digits[root1] = plus;

            it1++;
            it2++;
        } else if(root1 < root2){
            sum.digits[root1] = it1->second;
            it1++;
        } else {
            sum.digits[root2] = it2->second;
            it2++;
        }
    }

    while(it1 != digits.end()){
        sum.digits[it1->first] = it1->second;
        it1++;
    }

    while(it2 != a.digits.end()){
        sum.digits[it2->first] = it2->second;
        it2++;
    }
    return sum;
}


template <typename T, int... Ints>
Number<T, Ints...>& Number<T, Ints...>::operator+=(Number const& x){
    *this = *this+x;
    number_double = (double)(*this);
    return *this; 
}



template <typename T, int... Ints>
Number<T, Ints...> Number<T, Ints...>::operator-() const{

    Number result;
    for(auto &[root, frac]: digits){
        result.digits[root] = -frac;
    }

    return result;
}


template <typename T, int... Ints>
Number<T, Ints...> Number<T, Ints...>::operator-(Number const& a) const{
    auto it1 = digits.begin();
    auto it2 = a.digits.begin();

    Number result;
    while(it1!=digits.end() and it2!=a.digits.end()){
        int root1 = it1->first;
        int root2 = it2->first;

        if(root1 == root2){
            FracRoot<T, Ints...> dif = it1->second - it2->second;
            if(dif != 0)
                result.digits[root1] = dif;

            it1++;
            it2++;
        } else if(root1 < root2){
            result.digits[root1] = it1->second;
            it1++;
        } else {
            result.digits[root2] = -it2->second;
            it2++;
        }
    }

    while(it1 != digits.end()){
        result.digits[it1->first] = it1->second;
        it1++;
    }

    while(it2 != a.digits.end()){
        result.digits[it2->first] = -it2->second;
        it2++;
    }
    return result;
}


template <typename T, int... Ints>
Number<T, Ints...>& Number<T, Ints...>::operator-=(Number const& x){
    *this = *this-x;
    number_double = (double)(*this);
    return *this; 
}


// template <typename T, int... Ints>
// template <typename U>
// Number<T, Ints...>::operator U() const{
//     U decimal{};
//     for(auto& [root, digit]: digits){
//         T num = digit.get_num();
//         T den = digit.get_den();
//         decimal += (U)num/(U)den*std::sqrt((U)root);
//     }
        
//     return decimal;
// }


template <typename T, int... Ints>
template <typename U>
Number<T, Ints...>::operator U() const{
    if constexpr (std::is_same<T, mpz_class>::value){
        U decimal{};
        for(auto& [root, digit]: digits){
            U a = digit.get_num().get_d();
            U b = digit.get_den().get_d();
            decimal += a/b*std::sqrt((U)root);
        }
        return decimal;
    } else {

        U decimal{};
        for(auto& [root, digit]: digits)
            decimal += (U)digit.get_num()/(U)digit.get_den()*std::sqrt((U)root);
        return decimal;
        

    }
}



template <typename T, int... Ints>
Number<T, Ints...> Number<T, Ints...>::operator*(Number const& x) const{
    Number product{};
    for(auto& [root1, digit1]: digits)
        for(auto& [root2, digit2]: x.digits)
            product += digit1*digit2;

    // Restore the canonical form by removing zeros
    std::vector<int> erase;
    for(auto& [root, fracroot]: product.digits){
        if(fracroot.get_num() == 0){
            erase.push_back(root);
        }
    }

    for(auto& root: erase){
        product.digits.erase(root);
    }
    
    return product;
}

template <typename T, int... Ints>
Number<T, Ints...> Number<T, Ints...>::operator/(Number const& other) const{
    return (*this)*other.inverse();
}


template <typename T, int... Ints>
Number<T, Ints...> Number<T, Ints...>::operator/(int x) const{
    assert(x != 0);
    return (*this)*Fraction<T>{1,x};
}


template <typename T, int... Ints>
template <typename U>
Number<T, Ints...> Number<T, Ints...>::operator*(U const& x) const{
    Number product{};
    for(auto& [root, digit]: digits)
        if(x<0){
            product += (-digit)*(-x);
        } else {
            product += digit*x;

        }
        

        
        
    return product;
}

template <typename T, int... Ints>
std::size_t Number<T, Ints...>::get_hash(){
    int h = 0;
    for(auto& [root, digit]: digits){
        h += root + digit.get_num().get_si()*113 + digit.get_den().get_si()*99135941;
        h = h%3994111;
    }

    return h;
}