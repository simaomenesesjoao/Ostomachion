#pragma once
#include <utility>
#include <ostream>
#include "fraction.cpp"

template<typename T = int, int... Ints>
class FracRoot{
public:
    FracRoot();
    FracRoot(Fraction<T> const& fraction, int r);
    FracRoot(FracRoot&&);
    FracRoot(const FracRoot&);
    FracRoot& operator=(FracRoot&&);
    FracRoot& operator=(const FracRoot&);

    template<typename U, int... Args>
    friend std::ostream& operator<<(std::ostream& os, FracRoot<U, Args...> const & x);
    FracRoot operator*(FracRoot const & other) const;
    FracRoot operator*(Fraction<T> const & x) const;
    FracRoot operator*(T x) const;
    FracRoot operator-() const;
    FracRoot operator/(FracRoot const & other) const;
    FracRoot operator+(FracRoot const& b) const;
    FracRoot operator+(T b) const;
    FracRoot operator-(FracRoot const & other) const;
    // bool operator==(FracRoot x) const;
    // bool operator!=(FracRoot x) const;
    bool operator==(T x) const;
    bool operator!=(T x) const;
    T get_num() const;
    T get_den() const;
    int get_root() const;

private:
    Fraction<T> frac;
    int root;
    std::pair<int, int> remove_perfect_squares(int r) const;

};
