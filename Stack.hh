#ifndef STACK_HH
#define STACK_HH
#include <deque>
template<class T>
class Stack{
	public:		
	Stack(){}            	
	
	void push(T c){ 
		s.push_back(c);
	}
	
	void pop(){ 
		if (Stack::empty()) {
			std::cout << "Stack::pop() Error: stack is empty" << std::endl ;
		}    
		s.pop_back();
	}

	int nitems() { return s.size() ; }
    bool empty() { return (s.size()==0) ; }
    
    void inspect(){
		typename std::deque<T>::iterator iter;
		iter = s.begin();
		while(iter!=s.end()){
			std::cout<<*iter<<std::endl;
			++iter;
		}			
	}
	
	T returner(int i){
		c = s[i];
		return c;	
	}
    
	private:
    std::deque<T> s;
    T c;
};
#endif
