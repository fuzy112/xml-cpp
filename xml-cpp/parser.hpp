//
// Created by tsing on 2021/4/17.
//

#ifndef XML_CPP_PARSER_HPP
#define XML_CPP_PARSER_HPP

#include <expat.h>
#include <boost/coroutine2/coroutine.hpp>
#include <string_view>
#include <map>
#include <istream>
#include <variant>

#define XML_NAMESPACE_SEPARATOR '#'
#define XML_ENCODING "utf-8"

class qname {
public:
  explicit qname(const std::string &name)
  {
    auto pos = name.find(XML_NAMESPACE_SEPARATOR);
    if (pos != std::string::npos)
    {
      ns_ = name.substr(0, pos);
      name_ = name.substr(pos + 1);
    }
    else
    {
      name_ = name;
    }
  }

  qname(std::string ns, std::string name)
      : ns_ (std::move(ns)),
        name_(std::move(name))
  {
  }

  std::string const &namespace_() const
  {
    return ns_;
  }

  std::string const &name() const
  {
    return name_;
  }

private:
  std::string ns_;
  std::string name_;
};

class parser
{
public:
  template <typename T>
  struct event_base {
    template <typename U>
    constexpr bool operator==(event_base<U> const &) const
    {
      return std::is_same<T, U>::value;
    }

    template <typename U>
    constexpr bool operator!=(event_base<U> const &y) const
    {
      return !(*this == y);
    }
  };

  struct start_element_t : event_base<start_element_t>
  {
    std::string_view name;
    std::vector<std::pair<std::string_view, std::string_view>> attributes;
  };

  struct end_element_t : event_base<end_element_t>
  {
    std::string_view name;
  };

  struct character_data_t : event_base<character_data_t>
  {
    std::string_view data;
  };

  struct start_namespace_decl_t : event_base<start_namespace_decl_t>
  {
    std::string_view prefix;
    std::string_view uri;
  };

  struct end_namespace_decl_t : event_base<end_namespace_decl_t>
  {
    std::string_view prefix;
  };

  struct eof_t : event_base<eof_t>
  {
  };

  constexpr static const eof_t eof{};

  using event_type = std::variant<start_element_t, end_element_t, character_data_t, start_namespace_decl_t, end_namespace_decl_t, eof_t>;

  parser(std::istream &input, std::string name);

  ~parser();

  event_type next();
  event_type peek();

  event_type event() { return event_; }

private:
  std::istream &input_;
  std::string input_name_;
  std::vector<char> buffer_;
  XML_Parser p_;

  event_type event_;

  enum event_state {
    next_state_,
    peek_state_,
  } state_ = next_state_;

  boost::coroutines2::coroutine<event_type>::pull_type src_;
};

template <typename T> bool operator==(parser::event_base<T> const &x, parser::event_type const &y)
{
  return std::visit([&](auto const &y) { return x == y; }, y);
}

template <typename T> bool operator!=(parser::event_base<T> const &x, parser::event_type const &y)
{
  return std::visit([&](auto const &y) { return x != y; }, y);
}

template <typename T> bool operator==( parser::event_type const &y, parser::event_base<T> const &x)
{
  return std::visit([&](auto const &y) { return x == y; }, y);
}

template <typename T> bool operator!=(parser::event_type const &y, parser::event_base<T> const &x)
{
  return std::visit([&](auto const &y) { return x != y; }, y);
}

#endif //XML_CPP_PARSER_HPP
