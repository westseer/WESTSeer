#ifndef STRINGPROCESSING_H_INCLUDED
#define STRINGPROCESSING_H_INCLUDED

#include <string>
#include <vector>
#include <wx/string.h>
using namespace std;

void removeCharsFromString(string &str, const char* charsToRemove);

void replaceString(string& str, const string& oldStr, const string& newStr);

wchar_t *allocateWStr(const string& str);

wxString wxStr(const string& str);

string& trim(string& s, const char* t = " \t\n\r\f\v");

vector<string> splitString(string text, string delimiter, bool needTrimming = true);

vector<wxString> splitWString(string text, string delimiter, bool needTrimming = true);

string normalize(const string& s);

vector<string> tokenize(const string &phrase);

string getVectorStr(const vector<double> &v);

string getVectorStr(const vector<int> &v);

std::vector<double> getDoubleVector(const string &strV);

std::vector<int> getIntVector(const string &strV);

string simplifyTopic(const string &strTopic);

vector<vector<string>> stemHLKWs(string text);

#endif // STRINGPROCESSING_H_INCLUDED
