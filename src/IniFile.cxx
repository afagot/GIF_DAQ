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
#include <sstream>

#include "../include/IniFile.h"
#include "../include/MsgSvc.h"
#include "../include/utils.h"

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
            MSG_ERROR("[IniFile-ERROR] Wrong group format : doesn't end with a ]");
            return true;
        }
    }
    return false;
}

// *************************************************************************************************************

bool IniFile::CheckIfToken(string line,string& key,string& value){
    size_t p0 = 0;

    size_t p1 = string::npos;
    p1 = line.find_first_of('=',p0);

    if(p1 != p0){
        key = line.substr(p0,(p1-p0));
        p0 = line.find_first_not_of('=',p1);
        if(p0 != string::npos){
            value = line.substr(p0,(line.size()-p0));
        } else {
            Error = INI_ERROR_MISSING_VALUE;
            MSG_ERROR("[IniFile-ERROR] A value is missing for the key "+key);
            return true;
        }
    } else {
        Error = INI_ERROR_WRONG_FORMAT;
        MSG_ERROR("[IniFile-ERROR] A key name is missing is the config file");
        return false;
    }
    return true;
}

// *************************************************************************************************************

void IniFile::SetFileName(string filename){
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
        MSG_ERROR("[IniFile-ERROR] Cannot open configuration file");
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
                    MSG_ERROR("[IniFile-ERROR] The missing key name is in group "+group);
                    return Error;
                }
            }
        }
    }

    return Error;
}

// *************************************************************************************************************

int IniFile::Write(){
    ofstream ini(FileName.c_str(), ios::out);
    Error = INI_OK;

    string last_group = "";

    for(IniFileDataIter Iter = FileData.begin(); Iter != FileData.end(); Iter++){
        size_t separator_pos = Iter->first.find_first_of('.');
        if (separator_pos == string::npos)
            continue;

        string group = Iter->first.substr(0, separator_pos);

        if(group != last_group){
            last_group = group;
            ini << "[" << group << "]\n";
        }
        string token = Iter->first.substr(separator_pos+1);
        string value = Iter->second;

        ini << token << "=" << value << "\n";
    }

    ini.close();
    return Error;
}

// *************************************************************************************************************

IniFileData IniFile::GetFileData(){
    return FileData;
}

// *************************************************************************************************************

Data32 IniFile::addressType(string groupname, string keyname, Data32 defaultvalue ){
    string key;
    long addressValue = defaultvalue;

    IniFileDataIter Iter;

    if(groupname.size() > 0)
        key = groupname + "." + keyname;

    Iter = FileData.find(key);

    if(Iter != FileData.end())
        addressValue = strtoul(Iter->second.c_str(),NULL,16);
    else {
        string defVal = UintTostring(defaultvalue);
        MSG_WARNING("[IniFile-WARING] "+key+" could not be found : default key used instead ("+defVal+")");
    }

    return addressValue;
}

// *************************************************************************************************************


long IniFile::intType(string groupname, string keyname, long defaultvalue ){
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
    else {
        string defVal = longTostring(defaultvalue);
        MSG_WARNING("[IniFile-WARING] "+key+" could not be found : default key used instead ("+defVal+")");
    }

    return intValue;
}

// *************************************************************************************************************

long long IniFile::longType(string groupname, string keyname, long long defaultvalue ){
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
    else {
        string defVal = longlongTostring(defaultvalue);
        MSG_WARNING("[IniFile-WARING] "+key+" could not be found : default key used instead ("+defVal+")");
    }

    return longValue;
}

// *************************************************************************************************************

string IniFile::stringType( string groupname, string keyname, string defaultvalue ){
    string key;
    string stringChain = defaultvalue;

    IniFileDataIter Iter;

    if(groupname.size() > 0)
        key = groupname + "." + keyname;

    Iter = FileData.find(key);

    if(Iter != FileData.end())
        stringChain = Iter->second;
    else
        MSG_WARNING("[IniFile-WARING] "+key+" could not be found : default key used instead ("+defaultvalue+")");

    return stringChain;
}

// *************************************************************************************************************

float IniFile::floatType( string groupname, string keyname, float defaultvalue ){
    string key;
    float floatValue = defaultvalue;

    IniFileDataIter Iter;

    if(groupname.size() > 0)
        key = groupname + "." + keyname;

    Iter = FileData.find(key);

    if(Iter != FileData.end())
        floatValue = strtof(Iter->second.c_str(),NULL);
    else {
        string defVal = floatTostring(defaultvalue);
        MSG_WARNING("[IniFile-WARING] "+key+" could not be found : default key used instead ("+defVal+")");
    }

    return floatValue;
}

// *************************************************************************************************************

string IniFile::GetErrorMsg(){
    return "";
}

