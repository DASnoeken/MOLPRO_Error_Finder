#include <iostream>
#include <stdlib.h>
#include <string>
#include "err.hh"
#include "ErrException.hh"

int setNumT1s();

int main(int argc,char** argv){
	std::cout<<"Hello there!\nWelcome to MOLPRO error finder version 2.5!\nPlease make sure your outputfiles end with \".out\".\nMade by Daan A. Snoeken."<<std::endl<<std::endl;
	const char* fn = "ls.ls";
	double T1=0;
	double T1cp=0;
	int T1done = setNumT1s();
	if(argc>1){
		std::string test=argv[1];
		std::string jobname;
		std::string command;
		std::cout<<argv[1]<<std::endl;
		if(test == "all"){
			command="ls *.out > ls.ls";
		}else{
			jobname=argv[1];
			command = "ls " + jobname + "*.out > ls.ls";
		}
		system(command.c_str());
		if(argc==3){
			T1 = std::stod(argv[2]);
			if(T1<0){
				ErrException X("Illegal value for T1 diagnostic threshold");
				std::cout<<X<<std::endl<<X.what()<<std::endl;
				system("\\rm ls.ls");
				throw X;
			}
		}
		if(argc==4){
			T1 = std::stod(argv[2]);
			std::string option(argv[3]);
			if(T1<0){
				ErrException X("Illegal value for T1 diagnostic threshold");
				std::cout<<X<<std::endl<<X.what()<<std::endl;
				system("\\rm ls.ls");
				throw X;
			}
			err errs(fn,T1,T1done);
			errs.setArgc(argc);
			errs.setOption(option);
			errs.read();
			errs.output();
			T1=-1;
		}
		if(argc==5){
			T1 = std::stod(argv[2]);
			T1cp = std::stod(argv[4]);
			std::string option(argv[3]);
			if(T1<0){
				ErrException X("Illegal value for T1 diagnostic threshold");
				std::cout<<X<<std::endl<<X.what()<<std::endl;
				system("\\rm ls.ls");
				throw X;
			}
			err errs(fn,T1,T1done);
			errs.setT1cp(T1cp);
			errs.setArgc(argc);
			errs.setOption(option);
			errs.read();
			errs.output();
			T1=-1;
		}
		
	}else{
		system("ls *.out > ls.ls");
	}
	
	if(T1>0){
		err errs(fn,T1,T1done);
		errs.setArgc(argc);
		errs.read();
		errs.output();
	}else if(T1==0){
		err errs(fn);
		errs.setArgc(argc);
		errs.read();
		errs.output();
	}
	system("rm ls.ls");
}

int setNumT1s(){
	std::cout<<"Please enter the number of calculations you requested that give a T1 value."<<std::endl<<"Note: I am aware that the CCSD(T) for example performs a RMP2 calculation first. So 1 CCSD(T) calculation counts as 2 for your answer here!"<<std::endl;
	int i;
	std::cin>>i;
	return i;
}
