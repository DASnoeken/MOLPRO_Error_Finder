#include <iostream>
#include <string>
#ifndef ERREXCEPTION_HH
#define ERREXCEPTION_HH
class ErrException{
	public:
		ErrException(const char* name):_what(name){}
		const char* what(){return _what.c_str();}
	private:
		std::string _what;
};
std::ostream& operator <<(std::ostream& os, const ErrException& pide){
	return(os<<"\033[1;31mError!\033[0m");
}
#endif
