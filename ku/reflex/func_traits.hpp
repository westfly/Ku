#pragma once
#include <string>
#include <utility>
#include <ku/meta/func_traits.hpp>
#include <ku/reflex/type_traits.hpp>

namespace ku { namespace reflex {

namespace aux {

template <typename R, typename... Args>
struct func_traits
{
private:
  typedef R(function_type)(Args...);
  function_type* fp;

  std::string _fullname( )
  {
    Dl_info info;
    if (dladdr(reinterpret_cast<const void*>(fp), &info))
      return std::string(ku::reflex::demangle(info.dli_sname));
    return std::string();
  }
  
  inline bool _is_template( std::string const& name )
  {
    return '>' == name[name.find('(') - 1];
  }

public:
  func_traits( function_type f ) : fp(f)
  { }

  std::string fullname( )
  {
    std::string name(_fullname());
    // For non-template function, need to prepend result_type name
    if (!_is_template(name)) 
      return result().append(" ").append(name);
    else
      return name;
  }

  std::string name( )
  {
    std::string name(_fullname());
    // Remove result_type from template function name
    size_t begin = _is_template(name) ? result().size() + 1 : 0;
    // Remove parameter list
    name.erase(name.find('('));
    return std::string(name, begin);
  }

  inline size_t arity( )
  { 
    return sizeof...(Args);
  }

  inline std::string result( )
  {
    return type_traits<R>().name();
  }

  inline bool is_template( )
  {
    return _is_template(_fullname());
  }

  template <size_t N>
  inline auto arg( ) -> aux::type_traits<typename ku::meta::at<N - 1, Args...>::type>
  {
    return aux::type_traits<typename ku::meta::at<N - 1, Args...>::type>();
  }
};

} // namespace aux

template <typename R, typename... Args>
auto inline func_traits( R(&fp)(Args...) = *reinterpret_cast<R(*)(Args...)>(0) ) 
    -> aux::func_traits<R, Args...>
{
  return aux::func_traits<R, Args...>(fp);
}

} } // namespace ku::reflex
