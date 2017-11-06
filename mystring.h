#ifndef MY_STRING
#define MY_STRING

#include <iostream>
#include <map>

class MyString final {

public:

	MyString(const char *) ;
	MyString(MyString const &) noexcept ;
	MyString& operator=(MyString);
	MyString(MyString &&) noexcept ;
	~MyString();

	//	Capacity
	size_t length() const noexcept;
	size_t size() const noexcept {	return length(); }

	// Share ptr

	long use_count() const noexcept;

///// Non-member functions

friend MyString& operator+=(MyString & , MyString const &);
friend MyString operator+(MyString const &, MyString const & );

friend MyString& operator+=(MyString & , const char *);
friend MyString operator+(MyString const &, const char * );
friend MyString operator+(const char * , MyString const &);

friend MyString& operator+=(MyString & , char );
friend MyString operator+(MyString const &,  char );
friend MyString operator+(char, MyString const & );

friend std::ostream& operator<<(std::ostream & , MyString const & );
friend std::istream& operator>>(std::istream & , MyString & );

private:

	class String {
    private:
      size_t len;
      char * str;
    public:

    String(const char *);

    String(String const & other);

    String& operator=(String other);

    String(String && other) noexcept;

    ~String();

    size_t length() const noexcept;

    const char * c_str() const noexcept;

    friend String operator+(String const &, String const &);
    friend std::ostream& operator<<(std::ostream & , String const &);
    friend std::istream& operator>>(std::istream & , String & );

    struct cmp {
      bool operator()(const char * a, const char * b);
    };

  };

  friend String operator+(String const &, String const &);
  friend std::ostream& operator<<(std::ostream & , String const &);
  friend std::istream& operator>>(std::istream & , String & );



  class SharedPtr {
    private:
      String * ptr_;
      int * refcount_;

    public:
      explicit SharedPtr(String * ptr = nullptr);
      SharedPtr(SharedPtr const &the_other) noexcept;
      SharedPtr(SharedPtr && other) noexcept ;
      SharedPtr & operator=(SharedPtr other) noexcept;

      ~SharedPtr();

      int get_refcount() const noexcept;
      String& operator*() const noexcept;
      String* operator->() const noexcept;

    private:
      static std::map<const char *, SharedPtr, String::cmp> x_map;

  };

  SharedPtr str_ptr;

  friend class CharProxy;

public:
  class CharProxy {
    private:
      MyString & host;
      size_t index;
    public:

      CharProxy(MyString & host, size_t index) noexcept : host(host), index(index) {}

      CharProxy& operator=(char c);
      CharProxy& operator=(CharProxy const & p);

      operator char() const noexcept;
  };

  	// 	Element access
	CharProxy operator[](size_t);
};

MyString& operator+=(MyString &, MyString const &);
MyString operator+(MyString const &, MyString const &);

MyString& operator+=(MyString & , const char *);
MyString operator+(MyString const &, const char * );
MyString operator+(const char * , MyString const &);
MyString& operator+=(MyString & , char );
MyString operator+(MyString const &,  char );
MyString operator+(char, MyString const & );

std::ostream& operator<<(std::ostream &, MyString const &);

std::istream& operator>>(std::istream &, MyString &);

#endif
