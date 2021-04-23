#include <iostream>
#include <sstream>
#include <iomanip>
#include "xml-cpp/parser.hpp"

void event_handler(parser::start_element_t const &event)
{
  std::cout << "<" << event.name;
  for (auto at : event.attributes)
  {
    std::cout << " " << std::quoted(at.first) << "=" << std::quoted(at.second);
  }
  std::cout << ">";
}

void event_handler(parser::end_element_t const &event)
{
  std::cout << "</" << event.name << ">";
}

void event_handler(parser::character_data_t const &event)
{
  std::cout << event.data;
}

template <typename T> void event_handler(T const &) {}

const char *xml = R"xml(<?xml version="1.0" encoding="utf-8"?>
<mime-type xmlns="http://www.freedesktop.org/standards/shared-mime-info" type="x-content/audio-cdda">
  <!--Created automatically by update-mime-database. DO NOT EDIT!-->
  <comment>audio CD</comment>
  <comment xml:lang="af">oudio-CD</comment>
  <comment xml:lang="ar">CD سمعي</comment>
  <comment xml:lang="be@latin">aŭdyjo CD</comment>
  <comment xml:lang="bg">CD — аудио</comment>
  <comment xml:lang="ca">CD d'àudio</comment>
  <comment xml:lang="cs">zvukové CD</comment>
  <comment xml:lang="da">lyd-cd</comment>
  <comment xml:lang="de">Audio-CD</comment>
  <comment xml:lang="el">CD ήχου</comment>
  <comment xml:lang="en_GB">audio CD</comment>
  <comment xml:lang="eo">Son-KD</comment>
  <comment xml:lang="es">CD de sonido</comment>
  <comment xml:lang="eu">Audio CDa</comment>
  <comment xml:lang="fi">ääni-CD</comment>
  <comment xml:lang="fo">audio CD</comment>
  <comment xml:lang="fr">CD audio</comment>
  <comment xml:lang="ga">dlúthdhiosca fuaime</comment>
  <comment xml:lang="gl">CD de son</comment>
  <comment xml:lang="he">תקליטור שמע</comment>
  <comment xml:lang="hr">Glazbeni CD</comment>
  <comment xml:lang="hu">hang CD</comment>
  <comment xml:lang="ia">CD audio</comment>
  <comment xml:lang="id">CD audio</comment>
  <comment xml:lang="it">CD audio</comment>
  <comment xml:lang="ja">オーディオ CD</comment>
  <comment xml:lang="kk">аудио CD</comment>
  <comment xml:lang="ko">오디오 CD</comment>
  <comment xml:lang="lt">garso CD</comment>
  <comment xml:lang="lv">audio CD</comment>
  <comment xml:lang="nl">audio-CD</comment>
  <comment xml:lang="nn">lyd-CD</comment>
  <comment xml:lang="oc">CD àudio</comment>
  <comment xml:lang="pl">CD-Audio</comment>
  <comment xml:lang="pt">CD áudio</comment>
  <comment xml:lang="pt_BR">CD de áudio</comment>
  <comment xml:lang="ro">CD audio</comment>
  <comment xml:lang="ru">Аудио CD</comment>
  <comment xml:lang="sk">Zvukové CD</comment>
  <comment xml:lang="sl">zvočni CD</comment>
  <comment xml:lang="sq">CD audio</comment>
  <comment xml:lang="sr">звучни ЦД</comment>
  <comment xml:lang="sv">ljud-cd</comment>
  <comment xml:lang="tr">Müzik CD'si</comment>
  <comment xml:lang="uk">звуковий CD</comment>
  <comment xml:lang="vi">đĩa CD âm thanh</comment>
  <comment xml:lang="zh_CN">音频 CD</comment>
  <comment xml:lang="zh_TW">音訊 CD</comment>
</mime-type>)xml";

int main()
{
  std::stringstream ss(xml);
  parser p(ss, "test.xml");
  for (auto event = p.next(); event != parser::eof; event = p.next())
  {
    std::visit([] (auto event) {
      event_handler(event);
    }, event);
  }

  return 0;
}
