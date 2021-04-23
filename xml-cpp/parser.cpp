//
// Created by tsing on 2021/4/17.
//

#include "parser.hpp"
#include <system_error>
#include <memory>
#include <utility>

using coro = boost::coroutines2::coroutine<parser::event_type>;

static void start_element_handler(void *user_data, const XML_Char *name, const XML_Char **atts)
{
  auto &sink = *static_cast<coro::push_type *>(user_data);

  parser::start_element_t event;
  event.name = name;
  for (auto iter = atts; iter[0] != nullptr; iter += 2) {
    event.attributes.emplace_back(iter[0], iter[1]);
  }

  sink(std::move(event));
}

static void end_element_handler(void *user_data, const XML_Char *name)
{
  auto &sink = *static_cast<coro::push_type *>(user_data);

  parser::end_element_t event {{}, name};
  sink(event);
}

static void character_data_handler(void *user_data, const XML_Char *s, int len)
{
  auto &sink = *static_cast<coro::push_type *>(user_data);

  parser::character_data_t event;
  event.data = std::string_view(s, len);
  sink(event);
}

static void start_namespace_decl_handler(void *user_data, const XML_Char *prefix, const XML_Char *uri)
{
  auto &sink = *static_cast<coro::push_type *>(user_data);

  parser::start_namespace_decl_t event {{}, prefix ? prefix : "", uri };
  sink(event);
}

static void end_namespace_decl_handler(void *user_data, const XML_Char *prefix)
{
  auto &sink = *static_cast<coro::push_type *>(user_data);

  parser::end_namespace_decl_t event {{}, prefix ? prefix : "" };
  sink(event);
}

struct parse_xml {
  std::istream &input;
  const std::string &name;
  std::vector<char>& buffer;
  XML_Parser &p;

  void operator()(coro::push_type &sink)
  {
    // stop here
    sink({});

    XML_SetUserData(p, &sink);
    XML_SetElementHandler(p, &start_element_handler, &end_element_handler);
    XML_SetCharacterDataHandler(p, &character_data_handler);
    XML_SetNamespaceDeclHandler(p, &start_namespace_decl_handler, &end_namespace_decl_handler);

    buffer.resize(4096);

    while (input.read(buffer.data(), buffer.size())) {
      switch (XML_Parse(p, buffer.data(), buffer.size(), false))
      {
      case XML_STATUS_ERROR:
        throw std::runtime_error("xml error");
        break;
      case XML_STATUS_OK:
        break;
      case XML_STATUS_SUSPENDED:
        assert(0);
        break;
      }
    }

    switch (XML_Parse(p, buffer.data(), input.gcount(), true))
    {
    case XML_STATUS_ERROR:
      throw std::runtime_error("xml error");
      break;
    case XML_STATUS_OK:
      break;
    case XML_STATUS_SUSPENDED:
      assert(0);
      break;
    }

    sink(parser::eof_t { } );
  }
};

parser::parser(std::istream &input, std::string name)
    : input_(input),
      input_name_(std::move(name)),
      src_(parse_xml { input_, input_name_, buffer_, p_ })
{
  p_ = XML_ParserCreateNS(XML_ENCODING, XML_NAMESPACE_SEPARATOR);
  if (p_ == nullptr)
    throw std::system_error(ENOMEM, std::system_category());
}

parser::~parser()
{
  XML_ParserFree(p_);
}

parser::event_type parser::next()
{
  if (state_ == next_state_) {
    src_();
    event_ = src_.get();
    return event_;
  } else {
    state_ = next_state_;
    return event_;
  }
}

parser::event_type parser::peek()
{
  if (state_ == next_state_) {
    src_();
    event_ = src_.get();
    state_ = peek_state_;
    return event_;
  } else {
    return event_;
  }
}

