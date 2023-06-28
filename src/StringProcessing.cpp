#include <StringProcessing.h>
#include <algorithm>
#include <cstring>
#include <regex>
#include <map>

void removeCharsFromString(string &str, const char* charsToRemove)
{
    for (unsigned int i = 0; i < strlen(charsToRemove); ++i)
    {
        str.erase( remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
    }
}

void replaceString(string& str, const string& oldStr, const string& newStr)
{
  string::size_type pos = 0u;
  while((pos = str.find(oldStr, pos)) != string::npos){
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}

wchar_t *allocateWStr(const string& str)
{
    std::string t = str;
    replaceString(t, "'", "`");
    replaceString(t, "\n", " ");
    std::wstring s(t.begin(), t.end());
    return _wcsdup(s.c_str());
}

wxString wxStr(const string& str)
{
    std::string t = str;
    replaceString(t, "'", "`");
    replaceString(t, "\n", " ");
    wxString s(t);
    return s;
}

// trim from end of string (right)
inline string& rtrim(string& s, const char* t)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline string& ltrim(string& s, const char* t)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
string& trim(string& s, const char* t)
{
    return ltrim(rtrim(s, t), t);
}

// split string according to a specific delimiter, with optional trimming
vector<string> splitString(string text, string delimiter, bool needTrimming)
{
    if (delimiter.size() == 1)
    {
        char c = delimiter[0];
        int n = (int) text.size();
        int i0 = 0;
        vector<string> elems;
        for (int i1 = 0; i1 < n; i1++)
        {
            if (text[i1] == c)
            {
                int len = i1 - i0;
                if (len > 0)
                {
                    string elem = text.substr(i0, len);
                    string& trimmedElem = trim(elem);
                    if (trimmedElem.size() > 0)
                    {
                        elems.push_back(trimmedElem);
                    }
                }
                i0 = i1 + 1;
            }
        }
        if (i0 < n)
        {
            int len = n - i0;
            string elem = text.substr(i0, len);
            string& trimmedElem = trim(elem);
            if (trimmedElem.size() > 0)
            {
                elems.push_back(trimmedElem);
            }
        }
        return elems;
    }

    regex reg(delimiter);
    vector<string> untrimmedElems(
        sregex_token_iterator(text.begin(), text.end(), reg, -1),
        sregex_token_iterator()
    );
    if (needTrimming)
    {
        vector<string> elems;
        for (string elem : untrimmedElems)
        {
            string& trimmedElem = trim(elem);
            if (trimmedElem.size() > 0)
            {
                elems.push_back(trimmedElem);
            }
        }
        return elems;
    }
    else
    {
        return untrimmedElems;
    }
}

vector<wxString> splitWString(string text, string delimiter, bool needTrimming)
{
    vector<string> result1 = splitString(text, delimiter, needTrimming);
    vector<wxString> result2;
    for (auto r1: result1)
    {
        result2.push_back(wxStr(r1));
    }
    return result2;
}

size_t utf8_len(char src)
{
	const size_t lookup[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 4};
	uint8_t highbits = static_cast<uint8_t>(src) >> 4;
	return lookup[highbits];
}

string stripAccents(const string &inputString)
{
	string resultString;
	map<string, char> accentMap = {{"À", 'A'},{"Á", 'A'},
		{"Â", 'A'},{"Ã", 'A'},{"Ä", 'A'},{"Å", 'A'},{"à", 'a'},{"á", 'a'},
		{"â", 'a'},{"ã", 'a'},{"ä", 'a'},{"å", 'a'},{"È", 'E'},{"É", 'E'},
		{"Ê", 'E'},{"Ë", 'E'},{"è", 'e'},{"é", 'e'},{"ê", 'e'},{"ë", 'e'},
		{"Ì", 'I'},{"Í", 'I'},{"Î", 'I'},{"Ï", 'I'},{"ì", 'i'},{"í", 'i'},
		{"î", 'i'},{"ï", 'i'},{"Ò", 'O'},{"Ó", 'O'},{"Ô", 'O'},{"Õ", 'O'},
		{"Ö", 'O'},{"ò", 'o'},{"ó", 'o'},{"ô", 'o'},{"õ", 'o'},{"ö", 'o'},
		{"Ù", 'U'},{"Ú", 'U'},{"Û", 'U'},{"Ü", 'U'},{"ù", 'u'},{"ú", 'u'},
		{"û", 'u'},{"ü", 'u'},{"Ý", 'Y'},{"ý", 'y'},{"Ç", 'C'},{"ç", 'c'},
		{"Ñ", 'N'},{"ñ", 'n'},{"-", ' '},
	};

	for (size_t i = 0; i < inputString.length();)
	{
		int len = utf8_len(inputString[i]);
		string curChar = inputString.substr(i, len);
		auto iter = accentMap.find(curChar);
		if (iter != accentMap.end())
		{
			resultString += iter->second;
		}
		else
		{
			resultString += curChar;
		}
		i += len;
	}

	return resultString;
}

string toLower(const string& s)
{
	string result = s;
	transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

string normalize(const string& s)
{
	return toLower(stripAccents(s));
}

vector<string> tokenize(const string &phrase)
{
	string r = R"([[:punct:]]|[[:alpha:]]+|[[:digit:]]+)";
	regex re(r);
	smatch m;

	vector<string> tokens;
	string str = phrase;
	while (regex_search(str, m, re))
	{
		for (string x : m)
		{
		    bool isNumber = true;
		    for (size_t i = 0; i <x.size(); i++)
            {
                if (!isdigit(x[i]))
                {
                    isNumber = false;
                    break;
                }
            }
		    if (isNumber)
                tokens.push_back("#");
            else
                tokens.push_back(x);
		}
		str = m.suffix();
	}
	return tokens;
}

string getVectorStr(const vector<double> &v)
{
    std::stringstream ss;
    for (size_t i = 0; i < v.size(); i++)
    {
        if (i > 0)
            ss << ",";
        ss << v[i];
    }
    return ss.str();
}

std::vector<double> getDoubleVector(const string &strV)
{
    std::vector<std::string> strs = splitString(strV, ",");
    std::vector<double> result;
    for (std::string s: strs)
    {
        result.push_back(atof(s.c_str()));
    }
    return result;
}

string getVectorStr(const vector<int> &v)
{
    std::stringstream ss;
    for (size_t i = 0; i < v.size(); i++)
    {
        if (i > 0)
            ss << ",";
        ss << v[i];
    }
    return ss.str();
}

std::vector<int> getIntVector(const string &strV)
{
    std::vector<std::string> strs = splitString(strV, ",");
    std::vector<int> result;
    for (std::string s: strs)
    {
        result.push_back(atoi(s.c_str()));
    }
    return result;
}
