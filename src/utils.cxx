#ifndef UTILS_CXX
#define UTILS_CXX

#include "../include/utils.h"
#include "../include/MsgSvc.h"
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;

// ****************************************************************************************************

string shortTostring(short value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************

string intTostring(int value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************

string UintTostring(Uint value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************

string longTostring(long value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************

string longlongTostring(long long value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************

string floatTostring(float value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************

string GetRunStatus(){
    ifstream statusfile(__runstatuspath.c_str());
    string runStatus;

    if(statusfile){
        statusfile >> runStatus;
        statusfile.close();
        return runStatus;
    } else {
        MSG_ERROR("[DAQ-ERROR] Error opening the run status file.Please check "+__runstatuspath);
        runStatus = "READ_ERR";
        return runStatus;
    }
}

// ****************************************************************************************************

int SetRunStatus(string& runStatus){
    ofstream statusfile(__runstatuspath.c_str(), ios::out);

    if(statusfile){
        statusfile << runStatus;
        statusfile.close();
        return WR_OK;
    } else {
        MSG_ERROR("[DAQ-ERROR] Error opening the run status file.Please check "+__runstatuspath);
        runStatus = "WRITE_ERR";
        return WR_ERR;
    }
}

// ****************************************************************************************************

int CtrlRunStatus(string& runStatus){
    if(runStatus == "START")
        return START;
    else if(runStatus == "NEXT")
        return NEXT;
    else if(runStatus == "RUNNING")
        return RUNNING;
    else if(runStatus == "STOP")
        return STOP;
    else if(runStatus == "KILL")
        return FATAL;
    else if(runStatus == "RD_ERR")
        return RD_ERR;
    else if(runStatus == "WR_ERR")
        return WR_ERR;
    else{
        MSG_ERROR("[DAQ-ERROR] Run status is unknown ("+runStatus+")");
        runStatus = "STATUS_ERR";
        return STATUS_ERR;
    }
}

// ****************************************************************************************************

int GetTimeStamp(){
    stringstream stream;

    //Get time information
    time_t t = time(0);
    struct tm *Time = localtime(&t);
    int Y = Time->tm_year + 1900;
    int M = Time->tm_mon + 1;
    int D = Time->tm_mday;
    int h = Time->tm_hour;
    int m = Time->tm_min;
    int s = Time->tm_sec;

    //Set the time stamp
    int timestamp;

    stream << setfill('0') << setw(4) << Y
           << setfill('0') << setw(2) << M
           << setfill('0') << setw(2) << D
           << setfill('0') << setw(2) << h
           << setfill('0') << setw(2) << m
           << setfill('0') << setw(2) << s;

    stream >> timestamp;
    stream.clear();

    return timestamp;
}

// ****************************************************************************************************

string GetLogTimeStamp(){
    stringstream stream;

    //Get time information
    time_t t = time(0);
    struct tm *Time = localtime(&t);
    int Y = Time->tm_year + 1900;
    int M = Time->tm_mon + 1;
    int D = Time->tm_mday;
    int h = Time->tm_hour;
    int m = Time->tm_min;
    int s = Time->tm_sec;

    //Set the Date
    string Date;

    stream << setfill('0') << setw(4) << Y << "-"
           << setfill('0') << setw(2) << M << "-"
           << setfill('0') << setw(2) << D << "."
           << setfill('0') << setw(2) << h << ":"
           << setfill('0') << setw(2) << m << ":"
           << setfill('0') << setw(2) << s << ".";

    stream >> Date;
    stream.clear();

    return Date;
}

#endif // UTILS_CXX
