#pragma once
#include <iostream>
#include "points_vectors.hpp"
#include "shoelace.hpp"

// SIMAO: separar em angles e points

template <typename Num>
Angle<Num>::Angle(int x, int y){
    int norm2 = x*x + y*y;
    cos = Num{x, norm2, norm2};
    sin = Num{y, norm2, norm2};
    // angle_double = (double)(*this); SIMAO: remover isto
}

template <typename Num>
Angle<Num>::Angle(Num const& cos, Num const& sin):
    sin(sin), cos(cos){
        angle_double = (double)(*this);
    }

template <typename Num>
Angle<Num>::Angle(){};

template <typename Num>
Angle<Num>::Angle(Angle&& other):sin(std::move(other.sin)),
                                 cos(std::move(other.cos)){}
template <typename Num>
Angle<Num>::Angle(const Angle& other):sin(other.sin), cos(other.cos){}

template <typename Num>
Angle<Num>& Angle<Num>::operator=(Angle&& other){
    if(this == &other)
        return *this;

    sin = std::move(other.sin);
    cos = std::move(other.cos);
    return *this;
}

template <typename Num>
Angle<Num>& Angle<Num>::operator=(const Angle& other){
    if(this == &other)
        return *this;

    sin = other.sin;
    cos = other.cos;
    return *this;
}



template <typename Num>
Num Angle<Num>::get_cos() const{ return cos;}

template <typename Num>
Num Angle<Num>::get_sin() const{ return sin;}



template <typename Num>
Num& Angle<Num>::get_cos_mod() { return cos;}

template <typename Num>
Num& Angle<Num>::get_sin_mod() { return sin;}

template <typename Num>
const Num& Angle<Num>::get_cos_mod_const() const { return cos;}

template <typename Num>
const Num& Angle<Num>::get_sin_mod_const() const { return sin;}


template <typename Num>
Angle<Num> Angle<Num>::operator+(Angle const& a) const{
    Num new_cos = cos*a.cos - sin*a.sin;
    Num new_sin = sin*a.cos + cos*a.sin;
    return Angle(new_cos, new_sin);
}

template <typename Num>
Angle<Num> Angle<Num>::operator-(Angle const& a) const{
    Num new_cos = cos*a.cos + sin*a.sin;
    Num new_sin = sin*a.cos - cos*a.sin;
    return Angle(new_cos, new_sin);
}

template <typename Num>
Angle<Num> Angle<Num>::operator-() const{
    return Angle(-cos, -sin);
}

template <typename Num>
bool Angle<Num>::operator==(Angle const& a) const{
    return sin == a.sin && cos == a.cos;
}


template <typename Num>
bool Angle<Num>::operator!=(Angle const& a) const{
    return !(a==*this);
}

template <typename Num>
bool Angle<Num>::operator<(Angle const& other) const{
    // Is this lesser than other?
    bool larger_than_180 = is_larger_than_180();
    bool other_larger_than_180 = other.is_larger_than_180();
    if(other_larger_than_180 and not larger_than_180)
        return true;
    else if(not other_larger_than_180 and larger_than_180)
        return false;
    else {
        // Both angles are smaller than 180 or both are larger than 180. 
        // Clockwise => this > other (shoelace area < 0)
        // Anticlock => this < other (shoelace area > 0)
        // this ->- other 
        //    ^    /   
        //     \  v
        //     (0,0)
        
        return shoelace_area<Num>({{Num{0},Num{0}}, {cos, sin}, {other.get_cos(), other.get_sin()}}) > 0;
    }

}


// template <typename Num>
// bool Angle<Num>::operator>(Angle const& other) const{
//     // Is this greater than other? Reasoning is similar to <
//     bool larger_than_180 = is_larger_than_180();
//     bool other_larger_than_180 = other.is_larger_than_180();
//     if(other_larger_than_180 and not larger_than_180)
//         return false;
//     else if(not other_larger_than_180 and larger_than_180)
//         return true;
//     else {
//         // Both angles are smaller than 180 or both are larger than 180. 
//         // Clockwise => this > other (shoelace area < 0)
//         // Anticlock => this < other (shoelace area > 0)
//         // this ->- other 
//         //    ^    /   
//         //     \  v
//         //     (0,0)
        
//         return shoelace_area<Num>({{Num{0},Num{0}}, {cos, sin}, {other.get_cos(), other.get_sin()}}) < 0;
//     }

// }




template <typename Num>
bool Angle<Num>::is_larger_than_180() const{
    return shoelace_area<Num>({{Num{0},Num{0}}, {cos, sin}, {Num{2},Num{0}}}) > 0;
}


template <typename Num>
bool Angle<Num>::is_zero() const{
    return cos == 1 && sin == 0;
}


template <typename Num>
bool Angle<Num>::is_180() const{
    return cos == -1 && sin == 0;
}

template <typename Num>
std::ostream& operator<<(std::ostream& stream, Angle<Num> const& angle){
    stream << "ang=(" << angle.get_cos() << " " << angle.get_sin() << ")";
    return stream;    
}

template <typename Num>
template <typename T>
Angle<Num>::operator T() const{
    T pi = 2*std::acos(0.0);
    T angle = std::atan2((T)sin, (T)cos)/pi*180;
    if(angle<0)
        angle += 360;
    return angle;
};







template <typename Num>
Point<Num>::Point():x{Num{0}}, y{Num{0}}{}

template <typename Num>
Point<Num>::Point(int x, int y):x{Num{x}}, y{Num{y}}{}

template <typename Num>
Point<Num>::Point(Num const& x, Num const& y):x{x}, y{y}{}

template <typename Num>
Point<Num>::Point(Point&& other):x{std::move(other.x)}, y{std::move(other.y)}{}

template <typename Num>
Point<Num>::Point(const Point& other):x{other.x}, y{other.y}{}


template <typename Num>
Point<Num>& Point<Num>::operator=(Point&& other){
    x = std::move(other.x);
    y = std::move(other.y);
    return *this;
}

template <typename Num>
Point<Num>& Point<Num>::operator=(const Point& other){
    x = other.x;
    y = other.y;
    return *this;
}


template <typename Num>
Point<Num>::operator std::pair<Num, Num>() const {
    return {x, y};
}

template <typename Num>
Point<Num> Point<Num>::operator+(Point const& P) const{
    return Point{x + P.x, y + P.y};
}

template <typename Num>
Point<Num> Point<Num>::operator-(Point const& P) const{
    return Point{x - P.x, y - P.y};
}

template <typename Num>
Point<Num> Point<Num>::operator-() const{
    return Point{-x, -y};
}

template <typename Num>
Point<Num> Point<Num>::operator*(Num const& n) const{
    return Point{x*n, y*n};    
}

template <typename Num>
bool Point<Num>::operator==(Point const& P) const{
    return x == P.x && y == P.y;
}

template <typename Num>
Point<Num> Point<Num>::rotate(Angle<Num> const& a) const{
    return Point{x*a.get_cos() - y*a.get_sin(), x*a.get_sin() + y*a.get_cos()};  
}

template <typename Num>
void Point<Num>::print(){
    std::cout << "(" << (double)x << "," << (double)y << ")" << std::endl;
}

template <typename Num>
Num Point<Num>::get_x() const{
    return x;  
}

template <typename Num>
Num Point<Num>::get_y() const{
    return y;  
}

template <typename Num>
std::ostream& operator<<(std::ostream& stream, Point<Num> const& point){
    stream << "(x,y)=(" << point.get_x() << " " << point.get_y() << ")";
    return stream;    
}

template <typename Num>
Num Point<Num>::dot(Point<Num> const& other) const{
    return x*other.x + y*other.y;
}


template <typename Num>
Num Point<Num>::cross(Point<Num> const& other) const{
    return x*other.y - y*other.x;
}





