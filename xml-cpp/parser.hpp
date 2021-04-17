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

class parser
{
public:
  struct start_element
  {
    std::string_view name;
    std::vector<std::pair<std::string_view, std::string_view>> attributes;
  };

  struct end_element
  {
    std::string_view name;
  };

  struct character_data
  {
    std::string_view data;
  };

  struct start_namespace_decl
  {
    std::string_view prefix;
    std::string_view uri;
  };

  struct end_namespace_decl
  {
    std::string_view prefix;
  };

  struct eof
  {

  };

  using event_type = std::variant<start_element, end_element, character_data, start_namespace_decl, end_namespace_decl, eof>;


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


#endif //XML_CPP_PARSER_HPP
