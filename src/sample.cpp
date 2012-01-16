#include "json_writer.hpp"
#include <iostream>
#include <sstream>
#include <list>

////////////////////////////////////////////////////////////////////////////////
// Tricks
////////////////////////////////////////////////////////////////////////////////
template <class T>
JsonWriter & operator<<(JsonWriter& w, std::pair<T,T> begin_end)
{
  T &begin = begin_end.first;
  T &end = begin_end.second;
  w << begin_array;
  while(begin != end){
    w << (*begin);
    ++begin;
  }
  w << end_array;
  return w;
}
////////////////////////////////////////////////////////////////////////////////
// Testing
////////////////////////////////////////////////////////////////////////////////
int main( int argc, const char * argv[] )
{
  using namespace std;
  JsonWriter w(cout);
  w.begin_array();
  w.value(10);
  w.value("Hello\nit is me");
  w.value(3.14159268);
  w.begin_object();
  w.name("type");
  w.value(1);
  w.name("sub-array");
  w.begin_array();
  w.value(true);
  w.value(false);
  w.value_null();
  w.end_array();
  w.end_object();
  w.end_array();
  w.assert_finished();

  cout << endl;
  w.reset();

  w <<begin_object
     <<Name("format")<<"Test JSON file"
     <<Name("version")<<"1.0"
     <<Name("description")<<"Some file"
     <<Name("data");
  w<<begin_array;
  for (int i =0; i<10;++i)
    w << i;
  w.close_all();
  w.assert_finished();

  cout<<endl;
  w.reset();

  std::list<std::string> l;
  l.push_back("asd");
  l.push_back("asdasd");
  w << make_pair(l.begin(), l.end());
  w.close_all();
  w.assert_finished();
  return 0;
}
