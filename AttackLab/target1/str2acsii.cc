#include <iostream>
#include <string>

int main()
{
  ::std::string hex;
  ::std::cin >> hex;
  for (auto&& c: hex) {
    ::std::cout << ::std::hex << (int) c << " ";
  }
  ::std::endl(::std::cout);
  return 0;
}
