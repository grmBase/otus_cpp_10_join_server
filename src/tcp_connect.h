//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include <boost/asio.hpp>
//---------------------------------------------------------------------------
class t_server;
//---------------------------------------------------------------------------



class tcp_connect
  : public std::enable_shared_from_this<tcp_connect>
{
public:
  tcp_connect(boost::asio::ip::tcp::socket a_socket,
    t_server& a_server);

  // чтобы отслеживать удаление:
  ~tcp_connect();

  void start();

private:

  // обрабатываем готовую строковую инструкцию:
  void handle_ready_string(const std::string& astr_instr);

  // читаем что-то из сети
  void do_read();

  // пишем то, что в выходной строке
  void do_write();

  void start_write_answer(const std::string& astr_answer);

  boost::asio::ip::tcp::socket m_socket;


  
  uint8_t m_char; // конечно не оптимально читать побайтно, но так проще логика

  // кусок накопленных данных, ещё не сформировавшие полную инструкцию
  std::string m_buf_not_ready_yet;


  // ссылка на основной сервер
  t_server& m_server;



  // строка с полным ответом, который пишем в сеть
  std::string m_str_answer;
  // текущий указатель в строке сколько смогли уже отправили
  //const char* m_p_ptr;  решил попроще в учебном задании
};
//---------------------------------------------------------------------------