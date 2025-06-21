#pragma once
#include <ostream>

template <typename T>
class Fraction{

public:
    Fraction(T n, T d);

    T get_num() const;
    T get_den() const;

    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, Fraction<U> const & frac);

    Fraction operator-() const;
    // Fraction& operator=(Fraction const&);
    Fraction operator*(Fraction const& other) const;
    Fraction operator/(Fraction const& other) const;
    bool operator==(Fraction const& other) const;
    template <typename W, typename U> friend bool operator<(U, Fraction<T> const&);
    template <typename W, typename U> friend bool operator<(Fraction<W> const&, U);
    template <typename U> friend bool operator<(Fraction<U> const&, Fraction<U> const&);
    template <typename U> friend Fraction<U> operator+(Fraction<U> const& x, Fraction<U> const& y);
    template <typename U> friend Fraction<U> operator-(Fraction<U> const& x, Fraction<U> const& y);
    
private:
    T num, den;
};

template <typename T> T gcd_custom(const T& a, const T& b);