#include <iostream>
#include "mystring.h"

// ezt nem sikerült :/
#define nl std::endl

auto& out = std::cout;

int main(){
  // :'D ...

  out << "MyString s = 'hello', t = 'wello';" << nl;
  MyString s = "hello", t = "wello";
  out << "s[0] = t[0];" << nl;
  s[0] = t[0];
  out << "s: " << s << " t: " << t << nl << nl;

  out << "MyString x = 'hello';" << nl;
  MyString x = "hello";

  out << "MyString a = 'hello';" << nl;
  MyString a = "hello";


  out << "MyString y = x;" << nl << nl;
  MyString y = x;

  out << "y at [0]: "  << y[0] <<  nl;

  out << "x: " << x << nl;     /* hello */
  out << "a: " << a << nl;     /* hello */
  out << "y: " << y << nl;     /* hello */
  out << "x shares: " << x.use_count() << nl;
  out << "a shares: " << a.use_count() << nl;
  out << "y shares: " << y.use_count() << nl;

  out << nl << "y[0] = 'c';" << nl;
  y[0] = 'c'; /* hello */

  out << "x: " << x << nl;     /* hello */
  out << "a: " << a << nl;     /* hello */
  out << "y: " << y << nl;     /* cello */
  out << "x shares: " << x.use_count() << nl;
  out << "a shares: " << a.use_count() << nl;
  out << "y shares: " << y.use_count() << nl;

  out << nl << "y = x + y;" << nl;
  y = x + y;
  out << "x: " << x << nl;     /* hello */
  out << "a: " << a << nl;     /* hello */
  out << "y: " << y << nl;
  out << "x shares: " << x.use_count() << nl;
  out << "a shares: " << a.use_count() << nl;
  out << "y shares: " << y.use_count() << nl;

  out << nl << "y = x + 'blah';" << nl;
  y = x + "blah";
  out << "x: " << x << nl;     /* hello */
  out << "a: " << a << nl;     /* hello */
  out << "y: " << y << nl;
  out << "x shares: " << x.use_count() << nl;
  out << "a shares: " << a.use_count() << nl;
  out << "y shares: " << y.use_count() << nl;

  out << nl << "y = 'ello';" << nl;
  out << "y = 'h' + y;" << nl;
  y = "ello";
  y = 'h' + y;
  out << "x: " << x << nl;     /* hello */
  out << "a: " << a << nl;     /* hello */
  out << "y: " << y << nl;     /* hello */
  out << "x shares: " << x.use_count() << nl;
  out << "a shares: " << a.use_count() << nl;
  out << "y shares: " << y.use_count() << nl;
}

