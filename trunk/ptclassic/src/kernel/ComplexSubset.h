// IMPORTANT: the following file is a SUBSET of Complex.h
// as distributed with libg++, version 1.37.
// It does not define the stream functions.
// The error handlers are also removed.
//
// $Id$
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of GNU CC.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.  No author or distributor
accepts responsibility to anyone for the consequences of using it
or for whether it serves any particular purpose or works at all,
unless he says so in writing.  Refer to the GNU CC General Public
License for full details.

Everyone is granted permission to copy, modify and redistribute
GNU CC, but only under the conditions described in the
GNU CC General Public License.   A copy of this license is
supposed to have been given to you along with GNU CC so you
can know your rights and responsibilities.  It should be in a
file named COPYING.  Among other things, the copyright notice
and this notice must be preserved on all copies.  
*/

#ifndef _Complex_h
#pragma once
#define _Complex_h 1


#include <math.h>

class Complex
{
protected:
  double           re;
  double           im;

public:

  inline double /* const */ real() const;
  inline double /* const */ imag() const;

  inline           Complex();
  inline           Complex(const Complex& y);
  inline           Complex(double r, double i=0);

  inline          ~Complex();

  inline Complex&  operator =  (const Complex& y);

  inline Complex&  operator += (const Complex& y);
  inline Complex&  operator += (double y);
  inline Complex&  operator -= (const Complex& y);
  inline Complex&  operator -= (double y);
  inline Complex&  operator *= (const Complex& y);
  inline Complex&  operator *= (double y);

  Complex&         operator /= (const Complex& y); 
  Complex&         operator /= (double y); 

  void             error(const char* msg) const;
};

// inline members

inline double /* const */ Complex::real() const { return re; }
inline double /* const */ Complex::imag() const { return im; }

inline Complex::Complex() {}
inline Complex::Complex(const Complex& y) :re(y.real()), im(y.imag()) {}
inline Complex::Complex(double r, double i) :re(r), im(i) {}

inline Complex::~Complex() {}

inline Complex&  Complex::operator =  (const Complex& y) 
{ 
  re = y.real(); im = y.imag(); return *this; 
} 

inline Complex&  Complex::operator += (const Complex& y)
{ 
  re += y.real();  im += y.imag(); return *this; 
}

inline Complex&  Complex::operator += (double y)
{ 
  re += y; return *this; 
}

inline Complex&  Complex::operator -= (const Complex& y)
{ 
  re -= y.real();  im -= y.imag(); return *this; 
}

inline Complex&  Complex::operator -= (double y)
{ 
  re -= y; return *this; 
}

inline Complex&  Complex::operator *= (const Complex& y)
{  
  double r = re * y.real() - im * y.imag();
  im = re * y.imag() + im * y.real(); 
  re = r; 
  return *this; 
}

inline Complex&  Complex::operator *= (double y)
{  
  re *=  y; im *=  y; return *this; 
}


// non-inline functions

Complex  /* const */ operator /  (const Complex& x, const Complex& y);
Complex  /* const */ operator /  (const Complex& x, double y);
Complex  /* const */ operator /  (double   x, const Complex& y);

Complex  /* const */ cos(const Complex& x);
Complex  /* const */ sin(const Complex& x);

Complex  /* const */ cosh(const Complex& x);
Complex  /* const */ sinh(const Complex& x);

Complex  /* const */ exp(const Complex& x);
Complex  /* const */ log(const Complex& x);

Complex  /* const */ pow(const Complex& x, long p);
Complex  /* const */ pow(const Complex& x, const Complex& p);
Complex  /* const */ sqrt(const Complex& x);
   
// inline functions

inline int /* const */ operator == (const Complex& x, const Complex& y)
{
  return x.real() == y.real() && x.imag() == y.imag();
}

inline int /* const */ operator == (const Complex& x, double y)
{
  return x.imag() == 0.0 && x.real() == y;
}

inline int /* const */ operator != (const Complex& x, const Complex& y)
{
  return x.real() != y.real() || x.imag() != y.imag();
}

inline int /* const */ operator != (const Complex& x, double y)
{
  return x.imag() != 0.0 || x.real() != y;
}

inline Complex /* const */ operator - (const Complex& x)
{
  return Complex(-x.real(), -x.imag());
}

inline Complex /* const */ conj(const Complex& x)
{
  return Complex(x.real(), -x.imag());
}

inline Complex /* const */ operator + (const Complex& x, const Complex& y)
{
  return Complex(x.real() + y.real(), x.imag() + y.imag());
}

inline Complex /* const */ operator + (const Complex& x, double y)
{
  return Complex(x.real() + y, x.imag());
}

inline Complex /* const */ operator + (double x, const Complex& y)
{
  return Complex(x + y.real(), y.imag());
}

inline Complex /* const */ operator - (const Complex& x, const Complex& y)
{
  return Complex(x.real() - y.real(), x.imag() - y.imag());
}

inline Complex /* const */ operator - (const Complex& x, double y)
{
  return Complex(x.real() - y, x.imag());
}

inline Complex /* const */ operator - (double x, const Complex& y)
{
  return Complex(x - y.real(), -y.imag());
}

inline Complex /* const */ operator * (const Complex& x, const Complex& y)
{
  return Complex(x.real() * y.real() - x.imag() * y.imag(), 
                 x.real() * y.imag() + x.imag() * y.real());
}

inline Complex /* const */ operator * (const Complex& x, double y)
{
  return Complex(x.real() * y, x.imag() * y);
}

inline Complex /* const */ operator * (double x, const Complex& y)
{
  return Complex(x * y.real(), x * y.imag());
}

inline double /* const */ real(const Complex& x)
{
  return x.real();
}

inline double /* const */ imag(const Complex& x)
{
  return x.imag();
}

inline double /* const */ abs(const Complex& x)
{
  return hypot(x.real(), x.imag());
}

inline double /* const */ norm(const Complex& x)
{
  return (x.real() * x.real() + x.imag() * x.imag());
}

inline double /* const */ arg(const Complex& x)
{
  return atan2(x.imag(), x.real());
}

inline Complex /* const */ polar(double r, double t = 0.0)
{
  return Complex(r * cos(t), r * sin(t));
}

#endif
