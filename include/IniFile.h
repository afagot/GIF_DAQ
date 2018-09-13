// *************************************************************************************************************
// *   IniFile
// *   Added & edited by
// *   Alexis Fagot
// *   from QC3 DAQ by
// *   Stefano Colafranceschi
// *   29/01/2015
// *************************************************************************************************************

#ifndef __INIFILE_H_
#define __INIFILE_H_

#include <string>
#include <map>

#include "utils.h"

using namespace std;

// *************************************************************************************************************

const int INI_OK                          = 0;

// File Errors
const int INI_ERROR_CANNOT_OPEN_READ_FILE = 10;

// Format Errors
const int INI_ERROR_WRONG_FORMAT          = 20;
const int INI_ERROR_WRONG_GROUP_FORMAT    = 21;
const int INI_ERROR_MISSING_VALUE         = 22;

// *************************************************************************************************************

typedef map< const string, string > IniFileData;
typedef IniFileData::iterator IniFileDataIter;

// *************************************************************************************************************

class IniFile{
    private:
        bool        CheckIfComment(string line);
        bool        CheckIfGroup(string line,string& group);
        bool        CheckIfToken(string line,string& key,string& value);
        string      FileName;
        IniFileData FileData;
        int         Error;

    public:
        IniFile();
        IniFile(string filename);
        virtual    ~IniFile();

        // Basic file operations
        void        SetFileName(string filename);
        int         Read();
        int         Write();
        IniFileData GetFileData();

        // Data readout methods
        Data32      addressType (string groupname, string keyname, Data32 defaultvalue);
        long        intType     (string groupname, string keyname, long defaultvalue);
        long long   longType    (string groupname, string keyname, long long defaultvalue );
        string      stringType  (string groupname, string keyname, string defaultvalue );
        float       floatType   (string groupname, string keyname, float defaultvalue );

        // Error methods
        string      GetErrorMsg();
};

#endif
