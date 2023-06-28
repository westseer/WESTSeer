#ifndef PUBLICATION_H
#define PUBLICATION_H
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <wx/string.h>
using namespace std;
using namespace nlohmann;
class Publication
{
public:
    Publication();
    Publication(const string& strJson);
    Publication(const json& jsonWork);
    Publication(const map<string,string>& work);
    Publication(const Publication& another);
    virtual ~Publication();
    void parse(const string& strJson);
    void init(const json& jsonWork);
    void init(const map<string,string>& work);
    void writeWoS(std::ofstream &streamOut, const map<uint64_t, Publication> &publications);
    inline uint64_t id()
    {
        return _id;
    }
    inline int year()
    {
        return _year;
    }
    inline const wxString& title()
    {
        return _title;

    }
    inline const wxString& abstract()
    {
        return _abstract;
    }
    inline const wxString& source()
    {
        return _source;
    }
    inline const wxString& language()
    {
        return _language;
    }
    inline const vector<wxString> &authors()
    {
        return _authors;
    }
    inline const vector<uint64_t> &refIds()
    {
        return _refIds;
    }
    inline int numRefs()
    {
        return _refIds.size();
    }
    inline uint64_t refId(int i)
    {
        return _refIds[i];
    }
    static uint64_t convertId(const std::string idString, const char chType);
    static string convertId(const uint64_t id, const char chType);

protected:

private:
    uint64_t _id;
    int _year;
    wxString _title;
    wxString _abstract;
    wxString _source;
    wxString _language;
    vector<wxString> _authors;
    vector<uint64_t> _refIds;
};

#endif // PUBLICATION_H
