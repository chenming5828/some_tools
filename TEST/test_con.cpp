
#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <unistd.h>

std::mutex mtx;
std::condition_variable cv;


void print_id (int id) {
  std::unique_lock<std::mutex> lck(mtx);
  while (1) 
  {
      cv.wait(lck);
      std::cout << "thread " << id << '\n';
      break;
  }
  // ...
  
}

void go() {
  std::unique_lock<std::mutex> lck(mtx);
  cv.notify_all();
}

int main ()
{
  std::thread threads[10];
  // spawn 10 threads:
  for (int i=0; i<10; ++i)
    threads[i] = std::thread(print_id,i);

  std::cout << "10 threads ready to race...\n";
  usleep(10000);
  go();                    

  for (auto& th : threads) th.join();

  return 0;
}