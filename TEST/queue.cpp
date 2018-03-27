// queue::push/pop
#include <iostream>       // std::cin, std::cout
#include <queue>          // std::queue

int main ()
{
  std::queue<int> myqueue;
  int myint[9] = {0,1,2,3,4,5,6,7,8};

  std::cout << "Please enter some integers (enter 0 to end):\n";

 for(int i =0;i<9;i++)
 {
    myqueue.push(myint[i]) ;
 }

  std::cout << "myqueue contains: ";
  while (!myqueue.empty())
  {
    std::cout << ' ' << myqueue.front();
    myqueue.pop();
  }
  std::cout << '\n';

  return 0;
}