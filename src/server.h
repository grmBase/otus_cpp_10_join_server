//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include <boost/asio.hpp>
//---------------------------------------------------------------------------
#include <boost/asio/thread_pool.hpp> // для организации пула потоков
#include <boost/asio/post.hpp>
//---------------------------------------------------------------------------
#include "database.h"
//---------------------------------------------------------------------------


class t_server
{
  public:
    t_server(uint16_t a_port, size_t a_num_of_threads);

    ~t_server();

    // запускаем слушание порта:
    int start_listen();

    // то что вызывют нитки когда крутятся
    void asio_thread_work();

    int run();

    int stop();



    // 
    t_database& get_db();
    // 
    boost::asio::thread_pool& get_pool();


  private:

    void do_accept();



    // *********** данные ***************
    uint16_t m_port;

    // основной объект asio
    boost::asio::io_context m_io_context;

    // слушатель входящих запросов tcp
    boost::asio::ip::tcp::acceptor m_acceptor;

    // здесь храним список рабочих нитей
    std::vector<std::thread> m_threads;



    // пул потоков "медленных, пользовательских" функция
    boost::asio::thread_pool m_pool;

    // имитация ядра базы данных
    t_database m_db;

};
//---------------------------------------------------------------------------