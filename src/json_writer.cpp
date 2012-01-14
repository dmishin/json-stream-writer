#include "json_writer.hpp"
////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////
#include <iomanip>

BeginArray begin_array;
EndArray end_array;
BeginObject begin_object;
EndObject end_object;
NullValue null_value;

void JsonWriter::reset()
{
  state = S_EMPTY;
  stack.clear();
}
JsonWriter::~JsonWriter()
{
  if (auto_close)
    close_all();
}
/**Close several levels of braces (curly and square)*/
void JsonWriter::close_all( size_t to_level )
{
  while( level() > to_level ){
    switch(state){
    case S_OBJ_NONEMPTY:
    case S_OBJ_EMPTY:
      end_object();
      break;
    case S_ARRAY_NONEMPTY:
    case S_ARRAY_EMPTY:
      end_array();
      break;
    default:
      throw JsonWriterStateError("close_all: not in the objet or array");
    }
  }
}
void JsonWriter::assert_finished()
{
  assert_state( S_FINISHED );
}
void JsonWriter::put_char( char c )
{
  switch (c){
  case '\0': stream << "\\u0000"; break;
  case '\r': stream << "\\r"; break;
  case '\n': stream << "\\n"; break;
  case '\t': stream << "\\t"; break;
  case '"':  stream << "\\\""; break;
  default: stream << c;
  }
}
/**Put a string to the stream, with correct escaping*/
void JsonWriter::put_str( const std::string &value)
{
  stream << '"';
  std::string::const_iterator i,e;
  for( i=value.begin(), e=value.end(); i!=e; ++i ){
    put_char(*i);
  }
  stream << '"';
}

/**How the state is changed after writing 1 value*/
void JsonWriter::switch_state_value()
{
  switch(state){
  case S_EMPTY: 
    state = S_FINISHED; 
    break;
  case S_ARRAY_EMPTY: 
    state = S_ARRAY_NONEMPTY; 
    break;
  case S_ARRAY_NONEMPTY:
    stream << ',';
    state = S_ARRAY_NONEMPTY;
    break;
  case S_OBJ_HAS_NAME:
    state = S_OBJ_NONEMPTY;
    break;
  default:
    throw JsonWriterStateError("Value is unexpected in this state");
  }
}
void JsonWriter::push_state()
{
  stack.push_back(state);
}
void JsonWriter::pop_state()
{
  if(stack.empty())
    throw JsonWriterStateError("State stack is empty, nothign to close");
  state = stack.back();
  stack.pop_back();
}
void JsonWriter::value( const char *value )
{
  switch_state_value();
  stream<<'"';
  while (*value){
    put_char(*value);
    ++value;
  };
  stream<<'"';
}

void JsonWriter::value( int value )
{
  switch_state_value();
  stream << value;
}
void JsonWriter::value( long value )
{
  switch_state_value();
  stream << value;
}
void JsonWriter::value( double value)
{
  switch_state_value();
  stream <<std::setprecision(16)<< value; //16 must be enough for exact storing of `double`
}
void JsonWriter::value( float value)
{
  switch_state_value();
  stream <<std::setprecision(8)<< value; //8 must be enough for exact storing of `float`
}
void JsonWriter::value( char value )
{
  switch_state_value();
  stream << '"';
  put_char( value );
  stream << '"';
}
void JsonWriter::value( const std::string &value)
{
  switch_state_value();
  put_str(value);
}
void JsonWriter::value( bool value )
{
  switch_state_value();
  stream<<(value?"true":"false");
}
void JsonWriter::value_null()
{
  switch_state_value();
  stream<<"null";
}
void JsonWriter::begin_array()
{
  switch_state_value();
  push_state();
  state = S_ARRAY_EMPTY;
  stream << '[';
}
void JsonWriter::end_array()
{
  switch(state){
  case S_ARRAY_EMPTY:
  case S_ARRAY_NONEMPTY:
    stream << ']';
    pop_state();
    break;
  default:
    throw JsonWriterStateError("Not in the array");
  }
}
void JsonWriter::begin_object()
{
  switch_state_value(); //objects are values too
  push_state();
  state = S_OBJ_EMPTY;
  stream << '{';
}
void JsonWriter::end_object()
{
  switch(state){
  case S_OBJ_EMPTY:
  case S_OBJ_NONEMPTY:
    stream << '}';
    pop_state();
    break;
  default:
    throw JsonWriterStateError("Not in the object");
  }
}
void JsonWriter::name( const std::string& sname)
{
  switch(state){
  case S_OBJ_NONEMPTY:
    stream << ',';
    break;
  case S_OBJ_EMPTY: break;
  default:
    throw JsonWriterStateError("Not in the object");
  };
  state = S_OBJ_HAS_NAME;
  put_str( sname );
  stream <<':';
}
