// *************************************************************************************************************
// *   IniFile
// *   Added & edited by
// *   Alexis Fagot
// *   from QC3 DAQ by
// *   Stefano Colafranceschi
// *   29/01/2015
// *************************************************************************************************************

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>
#include <iomanip>
#include <map>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/io.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "../include/CAENVMElib.h"
#include "../include/CAENVMEoslib.h"
#include "../include/CAENVMEtypes.h"

#include "../include/IniFile.h"

using namespace std;

// *************************************************************************************************************

IniFile::IniFile(){

}

// *************************************************************************************************************

IniFile::IniFile(string filename){
    SetFileName(filename);
}

// *************************************************************************************************************

IniFile::~IniFile(){

}

// *************************************************************************************************************

bool IniFile::CheckIfComment(string line){
    return ( line[0] == '#' );
}

// *************************************************************************************************************

bool IniFile::CheckIfGroup(string line,string& group){
    if( line[0] == '[' ){
        if( line[line.length()-1] == ']' ){ // The format is right
                group = line.substr(1,line.length()-2);
                return true;
        } else {
            Error = INI_ERROR_WRONG_GROUP_FORMAT;
            return true;
        }
    }
    return false;
}

// *************************************************************************************************************

bool IniFile::CheckIfToken(string line,string& key,string& value){
    Data32 p0 = 0;

    Data32 p1 = string::npos;
    p1 = line.find_first_of('=',p0);

    if(p1 != p0){
        key = line.substr(p0,(p1-p0));
        p0 = line.find_first_not_of('=',p1);
        if(p0 != string::npos){
            value = line.substr(p0,(line.size()-p0));
        } else {
            Error = INI_ERROR_MISSING_VALUE;
            return true;
        }
    } else {
        Error = INI_ERROR_WRONG_FORMAT;
        return false;
    }
    return true;
}

// *************************************************************************************************************

void IniFile::SetFileName(const string filename){
    FileName = filename;
}

// *************************************************************************************************************

int IniFile::Read(){
    ifstream ini(FileName.c_str());
    stringstream parser;
    string token, value, line, group;

    Error = INI_OK;

	// Loading the file into the parser
    if(ini){
		parser << ini.rdbuf();
		ini.close();
	} else {
        Error = INI_ERROR_CANNOT_OPEN_READ_FILE;
        cerr << "CANNOT OPEN INI FILE\n\n";
        return Error;
	}

	group = "";
	
    while(getline(parser,line) && (Error == INI_OK)){
		// Check if the line is comment
        if(!CheckIfComment(line) ){
			// Check for group
            if(!CheckIfGroup(line,group)){
				// Check for token
                if(CheckIfToken(line,token,value)){
					// Make the key in format group.key if the group is not empty
                    if(group.size() > 1)
                        token = group + "." + token;
                    FileData[token] = value;
                } else {
                    Error = INI_ERROR_WRONG_FORMAT;
                    return Error;
				}
			}	
		}			
	}

    for(IniFileDataIter Iter = FileData.begin(); Iter != FileData.end(); Iter++)
        cout << "INI: " << Iter->first << " = " << Iter->second << endl;

    return Error;
}

// *************************************************************************************************************

int IniFile::Write(){
    Error = INI_OK;
	
    return Error;
}

// *************************************************************************************************************


Data32 IniFile::addressType(const string groupname, const string keyname, const Data32 defaultvalue ){
    stringstream sstr;
    string key;
    long addressValue = defaultvalue;

    IniFileDataIter Iter;

    if(groupname.size() > 0)
        key = groupname + "." + keyname;

    Iter = FileData.find(key);

    if(Iter != FileData.end())
        addressValue = strtoul(Iter->second.c_str(),NULL,16);

    return addressValue;
}

// *************************************************************************************************************


long IniFile::intType(const string groupname, const string keyname, const long defaultvalue ){
    stringstream sstr;
    string key;
    long intValue = defaultvalue;
    string fileValue;

	IniFileDataIter Iter;

    if(groupname.size() > 0)
        key = groupname + "." + keyname;

    Iter = FileData.find(key);

    if(Iter != FileData.end()){
        int base = 0;
        fileValue = Iter->second;
        if(fileValue[1] == 'x')
            base = 16;
        else if(fileValue[1] == 'b'){
            base = 2;                   //In the case of binary values, the function doesn't
            fileValue.erase(0,2);       //understand 0b as a integer literals -> erase it
        }

        intValue = strtol(fileValue.c_str(),NULL,base);
	}

    return intValue;
}

// *************************************************************************************************************

long long IniFile::longType(const string groupname, const string keyname, const long long defaultvalue ){
    //stringstream sstr;
    string key;
    long long longValue = defaultvalue;
    string fileValue;

    IniFileDataIter Iter;

    if(groupname.size() > 0)
        key = groupname + "." + keyname;

    Iter = FileData.find(key);

    if(Iter != FileData.end()){
        int base = 0;
        fileValue = Iter->second;
        if(fileValue[1] == 'x')
            base = 16;
        else if(fileValue[1] == 'b'){
            base = 2;                   //In the case of binary values, the function doesn't
            fileValue.erase(0,2);       //understand 0b as a integer literals -> erase it
        }

        longValue = strtoll(fileValue.c_str(),NULL,base);
    }

    return longValue;
}

// *************************************************************************************************************

string IniFile::stringType( const string groupname, const string keyname, const string defaultvalue ){
    string key;
    string stringChain = defaultvalue;

	IniFileDataIter Iter;

    if(groupname.size() > 0)
        key = groupname + "." + keyname;

    Iter = FileData.find(key);

    if(Iter != FileData.end())
        stringChain = Iter->second;

    return stringChain;
}

// *************************************************************************************************************

float IniFile::floatType( const string groupname, const string keyname, const float defaultvalue ){
    stringstream sstr;
    string key;
    float floatValue = defaultvalue;

	IniFileDataIter Iter;

    if(groupname.size() > 0)
        key = groupname + "." + keyname;

    Iter = FileData.find(key);

    if(Iter != FileData.end())
        floatValue = strtof(Iter->second.c_str(),NULL);

    return floatValue;
}

// *************************************************************************************************************

string IniFile::GetErrorMsg(){
	return "";
}

