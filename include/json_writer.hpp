#ifndef JSON_WRITER_HPP_INCLUDED
#define JSON_WRITER_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <exception>

struct JsonWriterStateError: public std::exception{
  std::string message;
  JsonWriterStateError():message("Incorrect JSON writer state"){}
  JsonWriterStateError(const std::string& what):message(what){};
  virtual ~JsonWriterStateError()throw(){}
  virtual const char* what()const throw(){ return message.c_str(); };
};

class JsonWriter{
public:
  enum State{
    S_EMPTY,
    S_ARRAY_EMPTY,
    S_ARRAY_NONEMPTY,
    S_OBJ_EMPTY,
    S_OBJ_HAS_NAME,
    S_OBJ_NONEMPTY,
    S_FINISHED
  };
  enum flags{
    F_AUTO_CLOSE = 0x1,
    F_STRICT = 0x2,
    F_VERIFY_ON_EXIT = 0x4
  };
private:
  typedef std::vector<State> StateStack;
  State state;
  StateStack stack;
  std::ostream& stream;
  int flags;
public:
  JsonWriter(std::ostream & os, int flags_ = 0)
    :state(S_EMPTY)
    ,stream(os)
    ,flags(flags_){};
  ~JsonWriter();
  bool is_auto_close()const{ return (flags & F_AUTO_CLOSE) != 0; };
  bool is_strict()const{ return (flags & F_STRICT) != 0; };
  bool is_verify_on_exit()const{ return (flags & F_VERIFY_ON_EXIT) != 0; };

  void value(int value);
  void value(long value);
  void value(double value);
  void value(float value);
  void value(const std::string &value);
  void value(const char* value );
  void value(bool value);
  void value(char value);
  void value_null();

  void begin_array();
  void end_array();
  void begin_object();
  void end_object();
  void name(const std::string& name); //Put name of the field
  void assert_finished();

  size_t level()const //return current number of opened braces
      {return stack.size();};
  void close_all(size_t to_level=0); //close several braces until specified level (0=close all braces)
  void reset(); //support for reuse
private:
  void assert_state(State s)const  
      {if (state != s) throw JsonWriterStateError();};
  void put_str( const std::string &s );
  void push_state();
  void pop_state();
  void switch_state_value(bool is_atomic);
  void put_char(char c);
};

//Manipulators: Put these objects to the JsonWriter to 
//achieve special effects
struct BeginArray{};
struct EndArray{};
struct BeginObject{};
struct EndObject{};
struct NullValue{};
struct Name{
  const std::string& name;
  explicit Name( const std::string& n):name(n){};
};
extern BeginArray begin_array;
extern EndArray end_array;
extern BeginObject begin_object;
extern EndObject end_object;
extern NullValue null_value;

/*This is actually more convenient than template fucntion 
  and gives better error messages
*/
#define SIMPLE_JSON_VALUE(type)					\
  inline JsonWriter & operator<<(JsonWriter &w, type v)		\
  {								\
    w.value(v); return w;					\
  }

/**Declare pipe operator for the simple types*/
SIMPLE_JSON_VALUE(int);
SIMPLE_JSON_VALUE(long);
SIMPLE_JSON_VALUE(double);
SIMPLE_JSON_VALUE(float);
SIMPLE_JSON_VALUE(const std::string&);
SIMPLE_JSON_VALUE(const char*);
SIMPLE_JSON_VALUE(bool);
#undef SIMPLE_JSON_VALUE

/**Implementation of manipulators*/
#define SIMPLE_JSON_MANIPULATOR(type, method)				\
  inline JsonWriter& operator<<(JsonWriter &w, const type &_)		\
  {									\
    w.method(); return w;						\
  }
/**Manipulators, that are implemented by the single method call 
   without argumens. Make them inline.
*/
SIMPLE_JSON_MANIPULATOR(BeginArray, begin_array);
SIMPLE_JSON_MANIPULATOR(EndArray, end_array);
SIMPLE_JSON_MANIPULATOR(BeginObject, begin_object);
SIMPLE_JSON_MANIPULATOR(EndObject, end_object);
SIMPLE_JSON_MANIPULATOR(NullValue, value_null);
#undef SIMPLE_JSON_MANIPULATOR

/**Field name is less simple*/
inline JsonWriter & operator<<(JsonWriter &w, const Name &n)
{
  w.name(n.name); return w;
}

#endif //JSON_WRITER_HPP_INCLUDED
