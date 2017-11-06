
#include "mystring.h"

#include <iostream>
#include <cstring>
#include <map>
#include <memory>

template <typename T>
class unique_ptr {
private:
  T * ptr_;
public:
  unique_ptr(T * array = nullptr) noexcept : ptr_(array) {}
  unique_ptr(unique_ptr const & other) = delete;
  unique_ptr(unique_ptr && other) = delete;

  T * operator*() const noexcept{
    return ptr_;
  }

  ~unique_ptr(){
    std::default_delete<T[]>(ptr_);
  }

};

// MyString::String member functions

MyString::String::String(const char * str = "") : len(strlen(str)), str(new char[len+1]) {
  strcpy(this->str, str);
}

MyString::String::String(MyString::String const & other): MyString::String{other.str} {}

MyString::String & MyString::String::operator=(MyString::String other) {
  std::swap(this->str, other.str);
  std::swap(this->len, other.len);
  return *this;
}

MyString::String::String(MyString::String && other) noexcept : len(other.len), str(other.str) {
  other.str = nullptr;
  other.len = 0;
}

MyString::String::~String() {
  delete[] str;
}

const char * MyString::String::c_str() const noexcept {
  return str;
}

size_t MyString::String::length() const noexcept {
  return len;
}

// MyString::String non-member functions

MyString::String operator+(MyString::String const & lhs, MyString::String const & rhs) {
  // Hát igen, nincs finally de helyette van RAII ami jobb is.
  // Egyébként valóban itt a unique_ptr-t használtam volna mert az RAII itt nekünk
  unique_ptr<char> newstr{new char[ lhs.len + rhs.len + 1]};
  strcpy(*newstr, lhs.str);
  strcpy(*newstr+lhs.len, rhs.str);

  MyString::String tmp(*newstr);

  return tmp;
}

std::ostream& operator<<(std::ostream & os, MyString::String const & str) {
  os << str.str;
  return os;
}

std::istream& operator>>(std::istream & is, MyString::String & str) {
  is.read(str.str, str.len);
  return is;
}

// MyString::cmp member functions

bool MyString::String::cmp::operator()(const char * a, const char * b){
  return strcmp(a, b) < 0;
}

// MyString::SharedPtr member functions

MyString::SharedPtr::SharedPtr(String* ptr) : ptr_{ptr} {
  if (ptr_){
    auto search = x_map.find(ptr_->c_str());
    if( search != x_map.end()) {
      SharedPtr & stored = search->second;
      delete ptr_;
      ptr_ = stored.ptr_;
      refcount_ = stored.refcount_;
      ++ *refcount_;
    } else{
      refcount_ = new int{1};
      x_map.insert(std::make_pair(ptr_->c_str(), *this));
    }
  } else
    refcount_ = nullptr;
}

MyString::SharedPtr::SharedPtr(MyString::SharedPtr const &the_other) noexcept : ptr_{the_other.ptr_}, refcount_{the_other.refcount_} {
  if (ptr_)
    ++ *refcount_;
}


MyString::SharedPtr::SharedPtr(MyString::SharedPtr && other) noexcept : ptr_(other.ptr_), refcount_(other.refcount_) {
  other.ptr_ = nullptr;
  other.refcount_ = nullptr;
}

MyString::SharedPtr & MyString::SharedPtr::operator=(MyString::SharedPtr other) noexcept {
  std::swap(ptr_, other.ptr_);
  std::swap(refcount_, other.refcount_);
  return *this;
}

MyString::SharedPtr::~SharedPtr() {
  if (ptr_) {
    if(*refcount_ == 2){
      -- *refcount_;
      x_map.erase(ptr_->c_str());
    } else {
      -- *refcount_;
      if (*refcount_ == 0) {
        delete refcount_;
        delete ptr_;
      }
    }
  }
}

int MyString::SharedPtr::get_refcount() const noexcept {
  return *refcount_-1; //hiding the internal map pointer
}

MyString::String& MyString::SharedPtr::operator*() const noexcept {
  return *ptr_;
}

MyString::String* MyString::SharedPtr::operator->() const noexcept {
  return ptr_;
}

// CharProxy

MyString::CharProxy & MyString::CharProxy::operator=(char c){
  unique_ptr<char> newstr{new char[host.str_ptr->length()+1]};
  strcpy(*newstr, host.str_ptr->c_str());
  (*newstr)[index] = c;

  host.str_ptr = MyString::SharedPtr{new MyString::String{*newstr}};

  return *this;
}

MyString::CharProxy& MyString::CharProxy::operator=(MyString::CharProxy const & proxy){
  unique_ptr<char> newstr{new char[host.str_ptr->length()+1]};
  strcpy(*newstr, host.str_ptr->c_str());
  (*newstr)[index] = (char)proxy;

  host.str_ptr = MyString::SharedPtr{new MyString::String{*newstr}};

  return *this;
}

MyString::CharProxy::operator char() const noexcept {
  return (host.str_ptr->c_str())[index];
}

// MyString member functions

std::map<const char *, MyString::SharedPtr, MyString::String::cmp> MyString::SharedPtr::x_map;

MyString::MyString(const char * str = "") : str_ptr{new MyString::String{str}} {}

MyString::MyString(MyString const & other) noexcept : str_ptr{other.str_ptr} {}

MyString::MyString(MyString && other) noexcept : str_ptr{std::move(other.str_ptr)} {}

MyString & MyString::operator=(MyString other) {
	std::swap(this->str_ptr, other.str_ptr);
	return *this;
}

MyString::~MyString(){}

MyString::CharProxy MyString::operator[](size_t index) {
  if(index >= length() || index < 0)
    throw std::out_of_range("Out of range indexing");
  return MyString::CharProxy{ *this, index};
}

size_t MyString::length() const noexcept {
	return this->str_ptr->length();
}

long MyString::use_count() const noexcept {
	return str_ptr.get_refcount();
}

// MyString non-member functions

MyString& operator+=(MyString & lhs, MyString const & rhs){
	lhs.str_ptr = MyString::SharedPtr(new MyString::String(*lhs.str_ptr + *rhs.str_ptr));
	return lhs;
}

MyString operator+(MyString const & lhs, MyString const & rhs){
	MyString newstring;
	newstring.str_ptr = MyString::SharedPtr(new MyString::String(*lhs.str_ptr + *rhs.str_ptr));
	return std::move(newstring);
}

MyString& operator+=(MyString & lhs, const char * str){
  lhs.str_ptr = MyString::SharedPtr(new MyString::String(*lhs.str_ptr + MyString::String{str}));
	return lhs;
}

MyString operator+(MyString const & lhs, const char * str){
  MyString newstring;
	newstring.str_ptr = MyString::SharedPtr(new MyString::String(*lhs.str_ptr + MyString::String{str}));
	return std::move(newstring);
}

MyString operator+(const char * str, MyString const & rhs) {
  MyString newstring;
	newstring.str_ptr = MyString::SharedPtr(new MyString::String(MyString::String{str} + *rhs.str_ptr));
	return std::move(newstring);
}

MyString& operator+=(MyString & lhs, char c){
  char str[2];
  str[0] = c;
  str[1] = '\0';
  lhs.str_ptr = MyString::SharedPtr(new MyString::String(*lhs.str_ptr + MyString::String{str}));
	return lhs;
}

MyString operator+(MyString const & lhs,  char c){
  char str[2];
  str[0] = c;
  str[1] = '\0';
  MyString newstring;
	newstring.str_ptr = MyString::SharedPtr(new MyString::String(*lhs.str_ptr + MyString::String{str}));
	return std::move(newstring);
}

MyString operator+(char c, MyString const & rhs ){
  char str[2];
  str[0] = c;
  str[1] = '\0';
  MyString newstring;
	newstring.str_ptr = MyString::SharedPtr(new MyString::String(MyString::String{str} + *rhs.str_ptr));
	return std::move(newstring);
}

std::ostream& operator<<(std::ostream & os, MyString const & str){
	os << *str.str_ptr;
	return os;
}

std::istream& operator>>(std::istream & is, MyString & str){
	is >> *str.str_ptr;
	return is;
}
