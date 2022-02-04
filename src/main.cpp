//---------------------------------------------------------------------------
#include <iostream>
//---------------------------------------------------------------------------
#include "server.h"
//---------------------------------------------------------------------------
#include "sync_console.h"
//---------------------------------------------------------------------------



int main(int argc, char* argv[])
{
  std::cout << "in main() start" << std::endl;

  if (argc != 2) {
    std::cerr << "Usage: server <listen port>" << std::endl;
    return 1;
  }

  int n_port = std::atoi(argv[1]);

  std::cout << "Detected params: tcp port: " << n_port << std::endl;


  try
  {
    // число потоков возьмём исходя из числа CPU:
    const auto processor_count = std::thread::hardware_concurrency();
    std::cout << "num of CPU cores: " << processor_count << std::endl;

    t_server server(n_port, processor_count);

    int n_res = server.start_listen();
    
    if (n_res) {
      clog::log_err("error in start_listen()");
      return n_res;
    }
    clog::logout("listen() completed OK");

    n_res = server.run();
    if (n_res) {
      clog::log_err("error in run()");
      return n_res;
    }
    clog::logout("run() completed OK");

    clog::logout("press any key to stop and exit");
    std::getchar();

    clog::logout("before call stop()");
    server.stop();

    clog::logout("after stop()");
  }
  catch (const std::exception& aexc)
  {
    clog::log_err(std::string("caught exeption: ") + aexc.what());
    return -33;
  }

  return 0;
}
//---------------------------------------------------------------------------