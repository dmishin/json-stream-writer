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
  EXPECT_EQ( w.level(), size_t(1) );
  w.value(-1);
  w.value("hello");
  w.value("");
  w.value_null();
  w.value(false);
  w.end_array();
  EXPECT_EQ( w.level(), 0u );

  EXPECT_EQ( s.str(), "[-1,\"hello\",\"\",null,false]" );
}

TEST( Arrays, Empty ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);
  w.begin_array();
  w.end_array();
  EXPECT_EQ( s.str(), "[]");
  EXPECT_EQ( w.level(), 0u );
}

TEST( Objects, Empty ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);
  w.begin_object();
  EXPECT_EQ( w.level(), 1u );
  w.end_object();
  EXPECT_EQ( w.level(), 0u );
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

TEST( Writer, NestedObjects ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);

  /* Trying to generate this:
     [{"fld":10,
       "arr":[{},2,3,4]},
      "test",
      []]
  */
  string original = "[{\"fld\":10,\"arr\":[{},2,3,4]},\"test\",[]]";

  w.begin_array();{
    EXPECT_EQ( w.level(), 1u );
    w.begin_object();{
      EXPECT_EQ( w.level(), 2u );
      w.name("fld");
      w.value( 10 );
      w.name("arr");
      w.begin_array();{
	w.begin_object();
	w.end_object();
	w.value(2);
	w.value(3);
	w.value(4);
      };w.end_array();
    };w.end_object();
    w.value("test");
    w.begin_array();
    w.end_array();
  };w.end_array();
  EXPECT_EQ( w.level(), 0u );
  EXPECT_EQ( s.str(), original );
}

TEST( Writer, ArrowAPI ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);
  //Test for more concise iostream-like API with "<<"
  w << begin_array << 1 << 2 << 3
    << begin_object 
    << Name("field1") << 10
    << Name("field2") << true
    << Name("array_field") 
    << begin_array
    << "the" << "array" << "sample"
    << end_array
    << end_object
    << end_array;
  //expected json:
  //[1,2,3,{"field1":10,"field2":true,"array_field":["the","array","sample"]}]
  EXPECT_EQ( s.str(), "[1,2,3,{\"field1\":10,\"field2\":true,\"array_field\":[\"the\",\"array\",\"sample\"]}]" );
}


TEST( Validation, WriteAfterFinish ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);

  w.value( "hello" );
  ASSERT_THROW( w.value("after-end"), JsonWriterStateError );

  w.reset();
  w.value( "hello" );
  ASSERT_THROW( w.value_null(), JsonWriterStateError );

  w.reset();
  w.value( "hello" );
  ASSERT_THROW( w.begin_object(), JsonWriterStateError );

  w.reset();
  w.value( "hello" );
  ASSERT_THROW( w.begin_array(), JsonWriterStateError );

  w.reset();
  w.value( "hello" );//attempt to write unexpected name
  ASSERT_THROW( w.name("field"), JsonWriterStateError );
}
TEST( Validation, AtInitialState ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);
  ASSERT_THROW( w.name("some field"), JsonWriterStateError );

  w.reset();
  ASSERT_THROW( w.end_object(), JsonWriterStateError );
  w.reset();
  ASSERT_THROW( w.end_array(), JsonWriterStateError );
}
TEST( Validation, InsideArray ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);
  w.begin_array();
  ASSERT_THROW( w.name("some field"), JsonWriterStateError );

  w.reset();
  w.begin_array();
  ASSERT_THROW( w.end_object(), JsonWriterStateError );
}
TEST( Validation, InsideObject ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);
  w.begin_object();
  ASSERT_THROW( w.value( 1 ), JsonWriterStateError ); //Value withut name in theobject

  w.reset();
  w.begin_object();
  ASSERT_THROW( w.value_null(), JsonWriterStateError );

  w.reset();
  w.begin_object();
  ASSERT_THROW( w.begin_array(), JsonWriterStateError );

  w.reset();
  w.begin_object();
  ASSERT_THROW( w.end_array(), JsonWriterStateError );

  w.reset();
  w.begin_object();
  ASSERT_THROW( w.begin_object(), JsonWriterStateError );

}

TEST( Validation, ObjectField ){
  using namespace std;
  stringstream s;
  JsonWriter w(s);

  w.begin_object();w.name("field");
  ASSERT_THROW( w.name("anouther name"), JsonWriterStateError ); //name after name
  w.reset();
  w.begin_object();w.name("field");
  ASSERT_THROW( w.end_object(), JsonWriterStateError ); //name without value
  w.reset();
  w.begin_object();w.name("field");
  ASSERT_THROW( w.end_array(), JsonWriterStateError ); //unexpected end
}
