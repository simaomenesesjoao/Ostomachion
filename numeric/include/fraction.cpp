#pragma once
#include <ostream>
#include "fraction.hpp"
#include <gmpxx.h>
#include <numeric>

// General gcd template (for built-in types)
template <typename T>
inline T gcd_custom(const T& a, const T& b) {
    return std::gcd(a, b);
}

// Specialization for GMP's mpz_class
template <>
inline mpz_class gcd_custom(const mpz_class& a, const mpz_class& b) {
    mpz_class result;
    mpz_gcd(result.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
    return result;
}

template<typename T>
Fraction<T>::Fraction(T n, T d):num{n}, den{d}{
    T gcd = gcd_custom<T>(num, den);
    num /= gcd;
    den /= gcd;
    if(den<0){
        den = -den;
        num = -num;
    }
}

template<typename T>
bool Fraction<T>::operator==(Fraction const& other) const{
    return (num == other.get_num()) and (den == other.get_den());
}

// template<typename T>
// Fraction<T>& Fraction<T>::operator=(Fraction const& other){
//     num = other.num;
//     den = other.den;
//     T gcd = gcd_custom(num, den);
//     num /= gcd;
//     den /= gcd;
//     if(den<0){
//         den = -den;
//         num = -num;
//     }

//     return *this;

// }

template<typename T>
Fraction<T> Fraction<T>::operator-() const{
    return Fraction(-num, den);
}

template<typename T>
Fraction<T> Fraction<T>::operator*(Fraction const& other) const{
    return Fraction(other.num*num, other.den*den);        
}

template<typename T>
Fraction<T> Fraction<T>::operator/(Fraction const& other) const{
    return Fraction(other.den*num, other.num*den);        
}

template<typename T>
Fraction<T> operator+(Fraction<T> const& x, Fraction<T> const& y){
    return Fraction<T>(x.num*y.den + x.den*y.num, x.den*y.den);        
}

template<typename T>
Fraction<T>  operator-(Fraction<T> const& x, Fraction<T> const& y){
    return Fraction<T>(y.den*x.num - y.num*x.den, y.den*x.den);
}

template<typename T>
bool operator<(Fraction<T> const& x, Fraction<T> const& y){
    int num_negatives = (x.get_den() < 0) + (y.get_den() < 0);
    int sign = num_negatives % 2 == 0? 1: -1;
    return x.get_num()*y.get_den()*sign < y.get_num()*x.get_den()*sign;
}

template<typename T, typename U>
bool operator<(Fraction<T> const& x, U y){
    return x < Fraction{(T)y,(T)1};
}

template<typename T, typename U>
bool operator<(U x, Fraction<T> const& y){
    return Fraction{(T)x,(T)1} < y;
}

template<typename T>
T Fraction<T>::get_num() const{
    return num;
}
template<typename T>
T Fraction<T>::get_den() const{
    return den;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, Fraction <T> const & frac){
    return os << frac.num << "/" << frac.den;
}

