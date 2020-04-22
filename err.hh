#include <iostream>
#include <fstream>
#include <string.h>
#include <iomanip>
#include <string>
#include "ErrException.hh"
#include "Stack.hh"
#ifndef ERR_HH
#define ERR_HH
class err{
	public:
		err(const char* filename):_File(filename){
			if(_File.fail()){
				ErrException X("File failed to open!");
				std::cout<<X<<std::endl<<X.what()<<std::endl<<"Filename was: "<<filename<<std::endl;
				throw X;
			}
			getline(_File,outFileName);
			std::cout<<"Testing: "<<outFileName<<std::endl;
			_file.open(outFileName.c_str());
			if(_file.fail()){
				ErrException X("File failed to open!");
				std::cout<<X<<std::endl<<X.what()<<std::endl<<"Filename was: "<<outFileName<<std::endl;
				throw X;
			}
			errOutput.open("errors.mperr",std::ofstream::out|std::ofstream::trunc);
			T1diagErr.open("T1diag.mperr",std::ofstream::out|std::ofstream::trunc);
			itererr.open("itererr.mperr",std::ofstream::out|std::ofstream::trunc);
			setT1thresh(0.02);
			CorruptErr.open("corruption.mperr",std::ofstream::out|std::ofstream::trunc);
		}
		
		err(const char* filename,double d, int i):_File(filename){
			if(_File.fail()){
				ErrException X("File failed to open!");
				std::cout<<X<<std::endl<<X.what()<<std::endl<<"Filename was: "<<filename<<std::endl;
				throw X;
			}
			getline(_File,outFileName);
			std::cout<<"Testing: "<<outFileName<<std::endl;
			_file.open(outFileName.c_str());
			if(_file.fail()){
				ErrException X("File failed to open!");
				std::cout<<X<<std::endl<<X.what()<<std::endl<<"Filename was: "<<outFileName<<std::endl;
				throw X;
			}
			errOutput.open("errors.mperr",std::ofstream::out|std::ofstream::trunc);
			T1diagErr.open("T1diag.mperr",std::ofstream::out|std::ofstream::trunc);
			itererr.open("itererr.mperr",std::ofstream::out|std::ofstream::trunc);
			setT1thresh(d);
			setT1count_MAX(i+1);
			CorruptErr.open("corruption.mperr",std::ofstream::out|std::ofstream::trunc);
		}
		
		~err(){_File.close();_file.close();errOutput.close();T1diagErr.close();CorruptErr.close();itererr.close();}
		
		void read(){
			std::string line,end,T1D;
			double T1;
			int itercount=0;
			int iterind=0;
			int T1count=0;
			while(getline(_file,line)){		
				if(line.find("ERROR")!= std::string::npos || line.find("Error")!=std::string::npos){
					std::cout<<"\033[1;31mERROR FOUND!\033[0m"<<std::endl;
					errors.push(outFileName);
					break;
				}
				if(line.find("No convergence")!=std::string::npos){		//Checks Hartree-Fock convergence
					std::cout<<"\033[1;31mCONVERGENCE PROBLEM FOUND!\033[0m"<<std::endl;
					errors.push(outFileName);
					break;
				}
				if(line.find("Starting RCCSD calculation")!=std::string::npos || line.find("Starting UCCSD calculation")!=std::string::npos || line.find("Starting CCSD calculation")!=std::string::npos){
					iterind++;
					while(getline(_file,line)){							//Checks if number of ccsd(t) iterations exceed 9, because then it usually shows suspicious behavior
						itercount++;
						if(line.find("Norm of t1 vector:")!=std::string::npos){
							itercount=itercount-4;
							if(itercount>9){
								std::cout<<"\033[1;31mWarning!\033[0m Suspicious number of iterations! Was: "<<itercount<<" at index: "<<iterind<<std::endl;
								itererr<<outFileName<<" : iterations : "<<itercount<<" : index : "<<iterind<<std::endl;
							}
							itercount=0;	//reset
							break;
						}
					}
				}
				if(line.find("T1 diagnostic:")!=std::string::npos && T1thresh!=0){	//Checks results from T1 diagnostic
					T1count++;
					end=line.substr(line.find("T"),line.size());
					T1D=end.substr(end.find(":")+3,end.size());
					if(!T1D.empty()){
						T1=std::stod(T1D);
					}else{
						std::cout<<"\033[1;31mWARNING! Potentially corrupted file!\033[0m"<<std::endl;
						CorruptErr<<outFileName<<std::endl;
						continue;
					}
					if ((T1>=T1thresh && T1count<T1count_MAX) || (T1>=T1cp && T1count>=T1count_MAX)){
						std::cout<<"\033[1;31mWarning!\033[0m Dangerous value of T1 diagnostic!"<<std::endl;
						std::cout<<"T1 diagnostic = "<<T1<<std::endl<<"T1D number in file = "<<T1count<<std::endl;
						if(option.find("nocp")!=std::string::npos && T1count<T1count_MAX){			
							T1diagErr<<std::setw(50)<<std::left<<outFileName<<"   "<<std::setw(15)<<std::left<<std::setprecision(12)<<T1<<"   "<<T1count<<std::endl;
							errors.push(outFileName);
						}
						if(option.find("nocp")==std::string::npos){
							T1diagErr<<std::setw(50)<<std::left<<outFileName<<"   "<<std::setw(15)<<std::left<<std::setprecision(12)<<T1<<"   "<<T1count<<std::endl;
							errors.push(outFileName);
						}
						if(option.find("ycp")==std::string::npos && argc==5 && T1count>=T1count_MAX){
							T1diagErr<<std::setw(50)<<std::left<<outFileName<<"   "<<std::setw(15)<<std::left<<std::setprecision(12)<<T1<<"   "<<T1count<<std::endl;
							errors.push(outFileName);
						}
					}
				}
				if(line.find("Variable memory released")!=std::string::npos){	
					
					if(getline(_file,line)){
						std::cout<<"\033[1;31mPotentially corrupted ending!\033[0m"<<std::endl;
						CorruptErr<<outFileName<<" For Ending"<<std::endl;
						break;
					}else{
						std::cout<<"\033[1;32mNo Errors found!\033[0m"<<std::endl;
						line="Variable memory released";
						break;
					}
				}
			}
			if(line.find("Variable memory released")==std::string::npos && _file.eof()){	//Other errors and crashes
				std::cout<<"\033[1;31mERROR FOUND!\033[0m"<<std::endl;
				errors.push(outFileName);
			}
			newFile();
		}
		
		void newFile(){
			_File>>outFileName;
			if(_File.eof()){
				std::cout<<"\033[1;32mThat's All, Folks!\033[0m"<<std::endl;
			}else{
				std::cout<<"Testing: "<<outFileName<<std::endl;
				_file.close();
				_file.open(outFileName.c_str());
				if(_file.fail()){
					ErrException X("File failed to open!");
					std::cout<<X<<std::endl<<X.what()<<std::endl<<"Filename was: "<<outFileName<<std::endl;
					throw X;
				}
				read();
			}
		}
		
		void output(){
			std::cout<<"Errors were found in the following files:"<<std::endl;
			errors.inspect();
			for(int i=0;i<errors.nitems();i++){
				errOutput<<errors.returner(i)<<std::endl;
			}
		}
		void setT1thresh(double d){
			T1thresh=d;
			T1diagErr<<"T1 diagnostic threshold set to: "<<T1thresh<<std::endl;
		}
		
		void setOption(std::string s){
			option=s;
		}
		void setArgc(int a){
			argc=a;
		}
		void setT1cp(double d){
			T1diagErr<<"T1 counterpoise diagnostic threshold set to: "<<d<<std::endl;
			T1cp=d;
		}
		void setT1count_MAX(int i){
			T1count_MAX = i;
		}
		
	private:
		std::ifstream _file;			//MOLPRO output files are here
		std::ifstream _File;			//This is the ls.ls file
		std::ofstream itererr;			//Suspicious amount of iterations outputfile
		std::ofstream errOutput;		//In this file alle the files with errors will be listed
		std::ofstream T1diagErr;		//Contain Errors in files related to the T1 diagnostic
		std::ofstream CorruptErr;		//Potentially corrupted files
		std::string outFileName;		//MOLPRO output file names
		Stack<std::string> errors;		//Container for all the error files. To be printed
		double T1thresh;				//Threshold value for the T1 diagnostic
		double T1cp;
		std::string option;				//Only option 'nocp' and 'ycp' possible. nocp ignores counterpoise T1. ycp gets separate value for counterpoise
		int argc;						//Number of arguments + 1
		int T1count_MAX;				//maximum number of T1's to find
};
#endif
