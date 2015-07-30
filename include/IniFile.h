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

#include "CAENVMEtypes.h"

using namespace std;

// *************************************************************************************************************

const int INI_OK                            = 0;

// File Errors
const int INI_ERROR_CANNOT_OPEN_READ_FILE   = 10;

// Format Errors
const int INI_ERROR_WRONG_FORMAT            = 20;
const int INI_ERROR_WRONG_GROUP_FORMAT      = 21;
const int INI_ERROR_MISSING_VALUE           = 22;

// *************************************************************************************************************

typedef  map< const string, string > IniFileData;
typedef  IniFileData::iterator IniFileDataIter;

// *************************************************************************************************************


class IniFile{
    private:
        bool            CheckIfComment(string line);
        bool            CheckIfGroup(string line,string& group);
        bool            CheckIfToken(string line,string& key,string& value);
        string          FileName;
        IniFileData 	FileData;
        int             Error;

    public:
        IniFile();
        IniFile(string filename);
        virtual         ~IniFile();

        // Basic file operations
        void            SetFileName(const string filename);
        int             Read();
        int             Write();

        // Data readout methods
        Data32          addressType(const string groupname, const string keyname, const Data32 defaultvalue);
        long            intType(const string groupname,const string keyname,const long defaultvalue);
        long long       longType( const string groupname, const string keyname, const long long defaultvalue );
        string          stringType( const string groupname, const string keyname, const string defaultvalue );
        float           floatType( const string groupname, const string keyname, const float defaultvalue );

        // Error methods
        string          GetErrorMsg();
};

#endif
