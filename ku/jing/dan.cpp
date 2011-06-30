#include <iostream>
#include <ku/dan/dan.hpp>
#include "type_traits.hpp"
#include "func_traits.hpp"
#include "backtrace.hpp"

using namespace std;
using namespace ku::jing;
using namespace ku::dan;

struct s0 {};

void f0() {}
double f1(int) { return 0.0; }
int f2(int, double) { return 0; }

void f10(int) {}
int f10(double) { return 0; }

template <typename T> void tf0() {}

TEST(jing, demangle)
{
  EXPECT_EQ(demangle(typeid(s0).name()), "s0");
}

TEST(jing, type_traits)
{
  EXPECT_EQ(type_traits(42).name(),  "int");
  EXPECT_EQ(type_traits(s0()).name(), "s0");
}

TEST(jing, func_traits)
{
  EXPECT_EQ(func_traits(f0).arity(), 0u);
  EXPECT_EQ(func_traits(f0).name(), "f0");
  EXPECT_EQ(func_traits(f0).fullname(), "void f0()");
  EXPECT_EQ(func_traits(tf0<int>).name(), "tf0<int>");
  EXPECT_EQ(func_traits(tf0<int>).fullname(), "void tf0<int>()");
  EXPECT_EQ(func_traits(f1).result(), "double");
  EXPECT_EQ(func_traits(f1).is_template(), false);
  EXPECT_EQ(func_traits(tf0<int>).is_template(), true);
  EXPECT_EQ(func_traits(f1).arg<1>().name(), "int");
  EXPECT_EQ(func_traits(f2).arg<2>().name(), "double");
  //EXPECT_EQ(func_traits<int(double)>(f10).name());
}

std::vector<std::string> btfunc1() { return backtrace(); }
std::vector<std::string> btfunc2() { return btfunc1(); }

TEST(jing, backtrace)
{
  std::vector<std::string> vs(btfunc2());
  EXPECT_TRUE(vs[0].find("btfunc1()") != std::string::npos);
  EXPECT_TRUE(vs[1].find("btfunc2()") != std::string::npos);
}

