#include "gtest/gtest.h"
#include "json_writer.hpp"
#include <sstream>

TEST( SingleValues, Creation ){
    //testing serialization of the simple values
  using namespace std;
  stringstream s;
  JsonWriter w(s);

  w.value( 1 );
  EXPECT_EQ( s.str(), "1" );
  EXPECT_EQ( w.level(), (size_t)0 );
    
  s.str("");
  w.reset();

  w.value( true );
  EXPECT_EQ( s.str(), "true" );
  EXPECT_EQ( w.level(), (size_t)0 );

  s.str("");
  w.reset();

  w.value( false );
  EXPECT_EQ( s.str(), "false" );
  EXPECT_EQ( w.level(), (size_t)0 );

  s.str("");
  w.reset();

  w.value_null();
  EXPECT_EQ( s.str(), "null" );
  EXPECT_EQ( w.level(), (size_t)0 );

  s.str("");
  w.reset();

  w.value( "hello" );
  EXPECT_EQ( s.str(), "\"hello\"" );
  EXPECT_EQ( w.level(), (size_t)0 );

  s.str("");
  w.reset();

  w.value( 3.1415 );
  s.flush();
  double x;
  s >> x;

  EXPECT_EQ( x, 3.1415 ); //preciosion must be enough for exact representation
  EXPECT_EQ( w.level(), (size_t)0 );
}

TEST( SingleValues, StringEscaping ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);

  w.value( "hello\nit\nis\tme" );
  EXPECT_EQ( s.str(), "\"hello\\nit\\nis\\tme\"" );
}

TEST( Arrays, General ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);

  w.begin_array();
  EXPECT_EQ( w.level(), 1 );
  w.value(-1);
  w.value("hello");
  w.value("");
  w.value_null();
  w.value(false);
  w.end_array();
  EXPECT_EQ( w.level(), 0 );

  EXPECT_EQ( s.str(), "[-1,\"hello\",\"\",null,false]" );
}

TEST( Arrays, Empty ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);
  w.begin_array();
  w.end_array();
  EXPECT_EQ( s.str(), "[]");
  EXPECT_EQ( w.level(), 0 );
}

TEST( Objects, Empty ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);
  w.begin_object();
  EXPECT_EQ( w.level(), 1 );
  w.end_object();
  EXPECT_EQ( w.level(), 0 );
  EXPECT_EQ( s.str(), "{}" );
}

TEST( Objects, General ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);
  
  w.begin_object();
  w.name( "a" );
  w.value( "hello" );
  w.name( "b" );
  w.value( 1000 );
  w.name( "c" );
  w.value_null();
  w.name( "d" );
  w.value( true );
  w.end_object();
  
  EXPECT_EQ( s.str(), "{\"a\":\"hello\",\"b\":1000,\"c\":null,\"d\":true}" );
}
