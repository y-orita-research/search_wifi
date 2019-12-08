//======================================================//
// Searching all available Wi-Fi (ESSID and dBm)        //
// Author : Y. ORITA                                    //
//======================================================//
// FOR COMPILE                                          //
// g++ -o wifi -std=c++11 wifi_recv_tmp.cpp             //  
//======================================================//


#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std; 

//PLEASE change the interface name ( e.g. wlp3s0 ).
const char* cmd = "iwlist wlp3s0 scan | grep -e Signal -e ESSID"; 

const char *filename = "search_result.dat";
const char del1 = '"';
const char del2 = ' ';

int last1, last2;
string str;
vector<string> _result;


int split_maker(int num1, int num2, string s);
int split_maker(int num1, int num2, string s)
{
    if((num1 != -1) && (num2 != -1)){
        if(num1 <= num2){
            return num1;
        }else{
            return num2;
        }   
    }else if((num1 == -1) && (num2 == -1)){
        return s.size();
    }else{
        if(num1 >= num2){
            return num1;            
        }else{
            return num2;
        }
    }
}



int main(int argc, char** argv)
{
    FILE* wifi_fp;
    char wifi_buf[128];
    ofstream dat_fp; 


    while(1){
        
        dat_fp.open(filename, std::ios::out);
      
        if ((wifi_fp = popen(cmd, "r")) != NULL){
	        while (fgets(wifi_buf, sizeof(wifi_buf), wifi_fp) != NULL){
	            //printf("%s", wifi_buf);
	            dat_fp << wifi_buf << endl;	  
	        }
	        pclose(wifi_fp);
        }

        dat_fp.close();   


        int row_count = 0;
        vector<vector<string>> _values;

        ifstream ifs(filename);
        if (!ifs){
            std::cout << "CANNOT open data file..." << std::endl;
            return -1;
        }

        //Read a line of the dat file
        while(getline(ifs, str)){ 
            
            if(row_count % 2 == 0){
                _result.clear();
            }

            //Skip blank lines
            if(str.empty()){
                continue;
            }   

            int first = 0;
            last1 = str.find_first_of(del1);
            last2 = str.find_first_of(del2);
            int last;

            last = split_maker(last1, last2, str);

            while(first < str.size()){

                string substr(str, first, last - first);

                if(!substr.empty()){
	                _result.push_back(substr);
                }

                first = last + 1;
                last1 = str.find_first_of(del1, first);
                last2 = str.find_first_of(del2, first);

                last = split_maker(last1, last2, str);

            }     
                    
            if(row_count % 2 != 0){
                _values.push_back(_result);
            }
            row_count++;
        
        }

        cout << "|=== Find " << _values.size() << " Wi-Fi access points ===========|" << endl;
        cout << "|============= The details are as follows ==|" << endl;  
     
        // for(unsigned int i = 0; i < _values.size(); i++){
        //    for(unsigned int j = 0; j < _values[i].size(); j++){
        //        cout << "_values["<< i << "]" << "[" << j  << "]" <<" is :";
        //        cout << _values[i][j] << endl;
        //    }   
        //    cout << endl;
        // }
        
        int dbm = 0;
        for(unsigned int i = 0; i < _values.size(); i++){
            sscanf(_values[i][2].c_str(), "level=%d", &dbm);
            cout << " ESSID : " << _values[i][5] << " |======> " << dbm << " dbm " << endl;
        }
        cout << "|===========================================|" << endl;

       //sleep(2);

    }
 
  return 0;
}