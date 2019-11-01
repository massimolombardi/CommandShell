#include <limits>
#include <fstream>
#include <iostream>
#include <strstream>
#include<readline/readline.h>
using namespace std;

#include "cConfigurationFile.h"


cConfigurationFile::cConfigurationFile(bool bDisableMsgOutput) {
    m_bDisableMsgOutput = bDisableMsgOutput;
    m_sFileName = DEFAULT_FILE_NAME;
}


cConfigurationFile::cConfigurationFile(const char* sFileName, bool bDisableMsgOutput) {
    m_bDisableMsgOutput = bDisableMsgOutput;
    m_sFileName = sFileName;
}


bool cConfigurationFile::loadConfigFile() {

    bool bWarning_FileInDosFormat = false;

    if(m_sFileName == "") {
        printMessage(ERROR_FILE_NOT_FOUND, ERROR_MSG);
        return false;
    }

    ifstream fin(m_sFileName.c_str());

    if(fin.bad() || fin.fail()) {
        printMessage(ERROR_READING_FILE, ERROR_MSG);
        cout << strerror(errno) << m_sFileName << endl;
        return false;
    }

    string nome;
    string valore;
    char line[DEFAULT_MAX_CONFIG_VALUE];

    while(!fin.eof()) {
        fin.getline(line, DEFAULT_MAX_CONFIG_VALUE);

        char *pPos = strchr(line, 0x0D);
        if(pPos != NULL) {
            *pPos = 0x00;
            bWarning_FileInDosFormat = true;
        }

        if(line[0] == '#' || strlen(line) == 0)
            continue;

        istrstream stringLine(line);
        stringLine >> nome >> valore;
        if(nome.at(0) == '#' || nome == "" || valore == "")
            continue;
        m_ConfigValues.insert(make_pair(nome, valore));
    }

    if(bWarning_FileInDosFormat == true) {
        string msg = ERROR_DOS_FORMAT_FILE + m_sFileName;
        printMessage(msg, ERROR_MSG);
    }

    fin.close();
    return true;
}


string cConfigurationFile::getStringValue(const char* key) {

    map<string, string>::iterator it;
    it = m_ConfigValues.find(string(key));

    if(it == m_ConfigValues.end())
        return DEFAULT_STRING_VALUE;
    else
        return it->second;
}


bool cConfigurationFile::getStringValue(const char* key, string* value) {

    map<string, string>::iterator it;
    it = m_ConfigValues.find(string(key));

    if(it == m_ConfigValues.end()) {
        return false;
    }
    else {
        *value = it->second;
        return true;
    }
}


int cConfigurationFile::getIntValue(const char* key) {

    char* end;
    long int tmpValue;

    int INT_MIN = numeric_limits<int>::min();
    int INT_MAX = numeric_limits<int>::max();
    int LONG_MIN = numeric_limits<long int>::min();
    int LONG_MAX = numeric_limits<long int>::max();

    string s = getStringValue(key);

    tmpValue = strtol(s.c_str(), &end, DEFAULT_BASE);

    if(end == s.c_str()) {
        printMessage(key, ERROR_NOT_DECIMAL_NUMBER, ERROR_MSG);
        return DEFAULT_INT_VALUE;
    }
    else if(*end != '\0') {
        printMessage(key, ERROR_OTHER_CHAR_AFTER_END, ERROR_MSG);
        return DEFAULT_INT_VALUE;
    }
    else if((tmpValue == LONG_MIN || tmpValue == LONG_MAX) && errno == ERANGE) {
        printMessage(key, ERROR_TOO_LONG_NUMBER, ERROR_MSG);
        return DEFAULT_INT_VALUE;
    }
    else if(tmpValue < INT_MIN || tmpValue > INT_MAX) {
        printMessage(key, ERROR_TOO_LONG_FOR_INT, ERROR_MSG);
        return DEFAULT_INT_VALUE;
    }
    else {
        return (int)tmpValue;
    }
}


bool cConfigurationFile::getIntValue(const char* key, int* value) {

    char* end;
    long int tmpValue;

    int INT_MIN = numeric_limits<int>::min();
    int INT_MAX = numeric_limits<int>::max();
    int LONG_MIN = numeric_limits<long int>::min();
    int LONG_MAX = numeric_limits<long int>::max();

    string s = getStringValue(key);

    tmpValue = strtol(s.c_str(), &end, DEFAULT_BASE);

    if(end == s.c_str()) {
        printMessage(key, ERROR_NOT_DECIMAL_NUMBER, ERROR_MSG);
        return false;
    }
    else if(*end != '\0') {
        printMessage(key, ERROR_OTHER_CHAR_AFTER_END, ERROR_MSG);
        return false;
    }
    else if((tmpValue == LONG_MIN || tmpValue == LONG_MAX) && errno == ERANGE) {
        printMessage(key, ERROR_TOO_LONG_NUMBER, ERROR_MSG);
        return false;
    }
    else if(tmpValue < INT_MIN || tmpValue > INT_MAX) {
        printMessage(key, ERROR_TOO_LONG_FOR_INT, ERROR_MSG);
        return false;
    }
    else {
        *value = (int)tmpValue;
        return true;
    }
}


bool cConfigurationFile::getBoolValue(const char* key) {

    string s = getStringValue(key);

    if(s == "true" || s == "TRUE" || s == "True" || s == "1")
        return true;
    else if(s == "false" || s == "FALSE" || s == "False" || s == "0")
        return false;
    else {
        printMessage(key, ERROR_NOT_VALID_VALUE, ERROR_MSG);
        return DEFAULT_BOOL_VALUE;
    }
}


bool cConfigurationFile::getBoolValue(const char* key, bool* value) {

    string s = getStringValue(key);

    if(s == "true" || s == "TRUE" || s == "True" || s == "1") {
        *value = true;
        return true;
    }
    else if(s == "false" || s == "FALSE" || s == "False" || s == "0") {
        *value = false;
        return true;
    }
    else {
        printMessage(key, ERROR_NOT_VALID_VALUE, ERROR_MSG);
        return false;
    }
}


unsigned int cConfigurationFile::getUnsignedIntValue(const char* key) {

    char* end;
    long unsigned int tmpValue;

    unsigned int UINT_MIN = numeric_limits<unsigned int>::min();
    unsigned int UINT_MAX = numeric_limits<unsigned int>::max();
    unsigned int ULONG_MIN = numeric_limits<unsigned long int>::min();
    unsigned int ULONG_MAX = numeric_limits<unsigned long int>::max();

    string s = getStringValue(key);

    tmpValue = strtoul(s.c_str(), &end, DEFAULT_BASE);

    if(end == s.c_str()) {
        printMessage(key, ERROR_NOT_DECIMAL_NUMBER, ERROR_MSG);
        return DEFAULT_UNSIGNEDINT_VALUE;
    }
    else if(*end != '\0') {
        printMessage(key, ERROR_OTHER_CHAR_AFTER_END, ERROR_MSG);
        return DEFAULT_UNSIGNEDINT_VALUE;
    }
    else if((tmpValue == ULONG_MIN || tmpValue == ULONG_MAX) && errno == ERANGE) {
        printMessage(key, ERROR_TOO_LONG_NUMBER, ERROR_MSG);
        return DEFAULT_UNSIGNEDINT_VALUE;
    }
    else if(tmpValue < UINT_MIN || tmpValue > UINT_MAX) {
        printMessage(key, ERROR_TOO_LONG_FOR_UINT, ERROR_MSG);
        return DEFAULT_UNSIGNEDINT_VALUE;
    }
    else {
        return (unsigned int)tmpValue;
    }
}


bool cConfigurationFile::getUnsignedIntValue(const char* key, unsigned int* value) {

    char* end;
    long unsigned int tmpValue;

    unsigned int UINT_MIN = numeric_limits<unsigned int>::min();
    unsigned int UINT_MAX = numeric_limits<unsigned int>::max();
    unsigned int ULONG_MIN = numeric_limits<long int>::min();
    unsigned int ULONG_MAX = numeric_limits<long int>::max();

    string s = getStringValue(key);

    tmpValue = strtol(s.c_str(), &end, DEFAULT_BASE);

    if(end == s.c_str()) {
        printMessage(key, ERROR_NOT_DECIMAL_NUMBER, ERROR_MSG);
        return false;
    }
    else if(*end != '\0') {
        printMessage(key, ERROR_OTHER_CHAR_AFTER_END, ERROR_MSG);
        return false;
    }
    else if((tmpValue == ULONG_MIN || tmpValue == ULONG_MAX) && errno == ERANGE) {
        printMessage(key, ERROR_TOO_LONG_NUMBER, ERROR_MSG);
        return false;
    }
    else if(tmpValue < UINT_MIN || tmpValue > UINT_MAX) {
        printMessage(key, ERROR_TOO_LONG_FOR_UINT, ERROR_MSG);
        return false;
    }
    else {
        *value = (unsigned int)tmpValue;
        return true;
    }
}


float cConfigurationFile::getFloatValue(const char* key) {

    char* end;
    float tmpValue;

    float FLOAT_MIN = numeric_limits<float>::lowest();
    float FLOAT_MAX = numeric_limits<float>::max();
    long double LONG_FLOAT_MIN = numeric_limits<long double>::min();
    long double LONG_FLOAT_MAX = numeric_limits<long double>::max();

    string s = getStringValue(key);

    tmpValue = strtof(s.c_str(), &end);

    if(end == s.c_str()) {
        printMessage(key, ERROR_NOT_DECIMAL_NUMBER, ERROR_MSG);
        return DEFAULT_FLOAT_VALUE;
    }
    else if(*end != '\0') {
        printMessage(key, ERROR_OTHER_CHAR_AFTER_END, ERROR_MSG);
        return DEFAULT_FLOAT_VALUE;
    }
    else if((tmpValue == LONG_FLOAT_MIN || tmpValue == LONG_FLOAT_MAX) && errno == ERANGE) {
        printMessage(key, ERROR_TOO_LONG_NUMBER, ERROR_MSG);
        return DEFAULT_FLOAT_VALUE;
    }
    else if(tmpValue < FLOAT_MIN || tmpValue > FLOAT_MAX) {
        printMessage(key, ERROR_TOO_LONG_FOR_FLOAT, ERROR_MSG);
        return DEFAULT_FLOAT_VALUE;
    }
    else {
        return (float)tmpValue;
    }
}


bool cConfigurationFile::getFloatValue(const char* key, float* value) {

    char* end;
    double tmpValue;

    float FLOAT_MIN = numeric_limits<float>::lowest();
    float FLOAT_MAX = numeric_limits<float>::max();
    long double LONG_FLOAT_MIN = numeric_limits<long double>::min();
    long double LONG_FLOAT_MAX = numeric_limits<long double>::max();

    string s = getStringValue(key);

    tmpValue = strtod(s.c_str(), &end);

    if(end == s.c_str()) {
        printMessage(key, ERROR_NOT_DECIMAL_NUMBER, ERROR_MSG);
        return false;
    }
    else if(*end != '\0') {
        printMessage(key, ERROR_OTHER_CHAR_AFTER_END, ERROR_MSG);
        return false;
    }
    else if((tmpValue == LONG_FLOAT_MIN || tmpValue == LONG_FLOAT_MAX) && errno == ERANGE) {
        printMessage(key, ERROR_TOO_LONG_NUMBER, ERROR_MSG);
        return false;
    }
    else if(tmpValue < FLOAT_MIN || tmpValue > FLOAT_MAX) {
        printMessage(key, ERROR_TOO_LONG_FOR_FLOAT, ERROR_MSG);
        return false;
    }
    else {
        *value = (float)tmpValue;
        return true;
    }
}


void cConfigurationFile::clearConfigValues() {
    m_ConfigValues.clear();
}


void cConfigurationFile::dumpConfiguration() {

    cout << MSG_DUMP_CFG << endl;

    map<string, string>::iterator it;

    for(it = m_ConfigValues.begin(); it != m_ConfigValues.end(); it++) {
        cout << "\t" << it->first << ": " << it->second << endl;
    }
}


void cConfigurationFile::printMessage(string msg, OUTPUT_MSG_TYPE type) {

    if(type == OK_MSG) {
        cout << msg << endl;
        return;
    }

    if(type == ERROR_MSG) {
        cout << msg << endl;
        return;
    }

    return;
}


void cConfigurationFile::printMessage(const char* key, string msg, OUTPUT_MSG_TYPE type) {

    if(type == OK_MSG) {
        cout << key << ": " << msg << endl;
        return;
    }

    if(type == ERROR_MSG) {
        cout << key << ": " << msg << endl;
        return;
    }

    return;
}
