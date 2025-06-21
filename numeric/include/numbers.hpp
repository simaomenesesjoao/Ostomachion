#pragma once
#include <ostream>
#include <map>
#include "fracroot.cpp"

#ifndef APPROX_COMPARISON
#define APPROX_COMPARISON 1
#endif

template <typename T = int, int... Ints>
class Number{
    
public:
    using Type = T;
    
    // Constructors and assignment
    Number(){};
    template <typename... U> explicit Number(U... digit_list);
    Number(int, int, int);
    Number(Number const& x);
    Number(Number&&);
    Number& operator=(Number const&);
    Number& operator=(Number&&);

    // Binary and unary operators
    Number  operator+(T x) const;
    Number& operator+=(T x);
    Number  operator+(Fraction<T> const& x) const;
    Number& operator+=(Fraction<T> const& x);
    Number  operator+(FracRoot<T, Ints...> const& x) const;
    Number& operator+=(FracRoot<T, Ints...> const& x);
    Number  operator+(Number const& a) const;    
    Number& operator+=(Number const& x);
    Number  operator-() const;
    Number  operator-(Number const& a) const;
    Number& operator-=(Number const& x);
    Number  operator*(Number const& x) const;
    Number  operator/(Number const& x) const;
    Number  operator/(int x) const;
    template <typename U> Number operator*(U const& x) const;

    // Comparison operators
    template <typename U, int...Args>
    friend std::ostream& operator<<(std::ostream & os, Number<U, Args...> const& number);
    bool operator==(Number const& x) const;
    bool operator==(int x) const;
    template <typename U> bool operator!=(U const& x) const;
    bool operator>(Number const& x) const;
    template <typename U> bool operator>=(U const& x) const;
    template <typename U> bool operator<=(U const& x) const;
    bool operator>(int x) const;
    bool operator<(Number const& x) const;
    bool operator<(int x) const;
    bool is_zero() const;

    template <typename U> explicit operator U() const;
    Number conjugate(int) const;
    Number inverse() const;
    std::size_t get_hash();

    template <typename U, int A, int... Ints2>
    friend bool is_pos_general_f(Number<U, A, Ints2...> const&);

    template <typename U, int... Ints2>
    friend bool is_pos_general_f(Number<U, Ints2...> const &);
    

    bool is_pos_general() const;
    int is_pos_fractional() const;
    bool is_pos() const;

    double number_double;
private:
    std::map<int, FracRoot<T,Ints...>> digits;
};

template <typename T, typename U> bool equal(T x, U y);


template <typename T, int... Ints>
std::ostream& operator<<(std::ostream & os, Number<T, Ints...> const& number){
    if(number.digits.empty()){
        os << "Empty";
    }
    for(auto& digit: number.digits){
        os << digit.second << " "; 
    }
    return os;
}


template <typename W, typename U>
bool equal(W x, U y){
    double tolerance = 1e-6;
    return std::abs(x-y) < tolerance;
}