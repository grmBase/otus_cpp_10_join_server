//---------------------------------------------------------------------------
#include <stdio.h>
#include <iostream>
//---------------------------------------------------------------------------
#include "tcp_connect.h"
#include "sync_console.h"
#include "server.h"
#include "database.h"
//---------------------------------------------------------------------------


tcp_connect::tcp_connect(boost::asio::ip::tcp::socket a_socket, t_server& a_server)
  : m_socket(std::move(a_socket)),
    m_server(a_server)
{
}
//---------------------------------------------------------------------------


tcp_connect::~tcp_connect()
{
  clog::logout("in destructor of tcp_connect");
}
//---------------------------------------------------------------------------


void tcp_connect::start()
{
  do_read();
}
//---------------------------------------------------------------------------


void tcp_connect::tcp_connect::do_read()
{

  auto self(shared_from_this());
  //m_socket.async_read_some(boost::asio::buffer(m_read_buf, c_un_buf_length),
  m_socket.async_read_some(boost::asio::buffer(&m_char, 1),
 
    [this, self](boost::system::error_code a_error, std::size_t a_readed)
    {
      if (a_error) {
        clog::log_err("error in asyn_read_some(). code: " + std::to_string(a_error.value()) + ", transl: " + a_error.message());
        return; 
      }
    

      if (!a_readed) {
        clog::log_err("zero receied?");
        return;
      }


      // такой перевод строки просто "скипаем". ждём слешь n
      if (m_char == '\r') {
        do_read();
        return;
      }

      if (m_char != '\n') {
        m_buf_not_ready_yet += std::string(1, m_char);

        do_read();
        return;
      }

      handle_ready_string(m_buf_not_ready_yet);

      m_buf_not_ready_yet = "";
      return;
      
    });
}
//---------------------------------------------------------------------------


void tcp_connect::start_write_answer(const std::string& astr_answer)
{
  m_str_answer = astr_answer + "\r\n";
  do_write();
}
//---------------------------------------------------------------------------


// обрабатываем готовую строковую инструкцию:
void tcp_connect::handle_ready_string(const std::string& astr_instr)
{

  std::vector<std::string> vecParts;
  {
    std::string strCurr;
    std::istringstream input;
    input.str(astr_instr);
    while(std::getline(input, strCurr, ' '))
    {
      if (!strCurr.empty()) {
        vecParts.push_back(strCurr);
      }
    }
  }

  // проверим, что есть хоть одна строка - тип запроса:
  if(vecParts.size() < 1) {
    clog::logout("num of parts less than 1. Count: " + std::to_string(vecParts.size()));

    start_write_answer("num of parts less than 1. Count: " + std::to_string(vecParts.size()));
    return;
  }

  // дебаг:
  //for (auto& curr : vecParts) {
  //  clog::logout("instr parts: '" + curr + "'");
  //}


  /*
  auto space_pos = astr_instr.find_first_of(' ');
  if(space_pos == std::string::npos) {
    clog::log_err("can't locate space");

    start_write_answer("error: can't locate space in income string: " + astr_instr);
    return;
  }

  const std::string str_inst_cmd = astr_instr.substr(0, space_pos + 1);
  */


  const std::string str_inst_cmd = vecParts[0];

  if (str_inst_cmd == "INSERT") {

    clog::logout("<< instuction INSERT");
    
    if (vecParts.size() < 4) {
      clog::logout("num of parts less than 4. Count: " + std::to_string(vecParts.size()));

      start_write_answer("num of parts less than 4. Count: " + std::to_string(vecParts.size()));
      return;
    }

    std::string str_table_name = vecParts[1];
    int n_key = std::atoi((vecParts[2]).c_str());
    std::string str_value = vecParts[3];



    // Тут наверно как-то закинули задание в отдельный пул. Пока вернём ок
    auto self(shared_from_this());
    boost::asio::post(m_server.get_pool(), [this, self, str_table_name, n_key, str_value]
      {
        std::string str_answer = m_server.get_db().insert(str_table_name, n_key, str_value);

        start_write_answer(str_answer);
      }
    );

    // Тут наверно как-то закинули задание в отдельный пул. Пока вернём ок
    //start_write_answer("OK");

    return;
  }
  else if (str_inst_cmd == "TRUNCATE") {

    clog::logout("<< instuction TRUNCATE");

    if (vecParts.size() < 2) {
      clog::logout("num of parts less than 1. Count: " + std::to_string(vecParts.size()));

      start_write_answer("num of parts less than 1. Count: " + std::to_string(vecParts.size()));
      return;
    }
    std::string str_table_name = vecParts[1];


    // Тут наверно как-то закинули задание в отдельный пул. Пока вернём ок
    auto self(shared_from_this());
    boost::asio::post(m_server.get_pool(), [this, self, str_table_name]
    {
      std::string str_answer = m_server.get_db().truncate(str_table_name);

      start_write_answer(str_answer);
    }
    );

    return;
  }
  else if (str_inst_cmd == "INTERSECTION") {
    clog::logout("<< instuction INTERSECTION");

    auto self(shared_from_this());
    boost::asio::post(m_server.get_pool(), [this, self]
      {
        std::string str_answer = m_server.get_db().intersection();

        start_write_answer(str_answer);
      }
    );

    return;
  }
  else if (str_inst_cmd == "SYMMETRIC_DIFFERENCE") {
    clog::logout("<< instuction SYMMETRIC_DIFFERENCE");

    auto self(shared_from_this());
    boost::asio::post(m_server.get_pool(), [this, self]
      {
        std::string str_answer = m_server.get_db().symmetric_difference();

        start_write_answer(str_answer);
      }
    );

    return;
  }
  else {
    clog::logout("detected some unsupported instruction: " + str_inst_cmd);
    start_write_answer("detected some unsupported instruction: " + str_inst_cmd);
    return;
  }

  return;
}
//---------------------------------------------------------------------------


void tcp_connect::do_write()
{
  auto self(shared_from_this());
  boost::asio::async_write(m_socket, boost::asio::buffer(m_str_answer.c_str(), m_str_answer.size()),
    [this, self](boost::system::error_code a_error, std::size_t a_written)
    {
      if(a_error) {
        clog::log_err("error in async_write(). code: " + std::to_string(a_error.value()) + ", transl: " + a_error.message());
        return;
      }

      clog::logout("num written: " + std::to_string(a_written) + ", value: " + std::string(m_str_answer.c_str(), a_written));

      // может всё уже дописали?
      if(a_written == m_str_answer.size()) {
        clog::logout("all written");

        do_read();

        return;
      }

      clog::logout("not all written, so continue writing...");

      m_str_answer = std::string(m_str_answer.c_str() + a_written);
      
    });
}
//---------------------------------------------------------------------------

