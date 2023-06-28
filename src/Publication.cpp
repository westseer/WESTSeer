#include "Publication.h"
#include "StringProcessing.h"
#include <sstream>
#include <map>
#include <cassert>

uint64_t Publication::convertId(const std::string idString, const char chType)
{
    vector<string> idFields = splitString(idString, "/", false);
    if (idFields.size() == 0)
    {
        return 0;
    }
    string s = idFields.back();
    if (s.size() < 2)
    {
        return 0;
    }
    if (s[0] != chType)
    {
        return 0;
    }
    s = s.substr(1);
    uint64_t id = 0;
    try
    {
        id = stoull(s);
    }
    catch (...)
    {
    }
    return id;
}

string Publication::convertId(const uint64_t id, const char chType)
{
    std::stringstream ss;
    std::string c;
    c += chType;
    ss << "https://openalex.org/" << c << id;
    return ss.str();
}

Publication::Publication()
{
    _id = 0;
    _year = 0;
}

Publication::Publication(const string& strJson)
{
    parse(strJson);
}

Publication::Publication(const json& jsonWork)
{
    init(jsonWork);
}

Publication::Publication(const map<string,string>& work)
{
    init(work);
}

Publication::Publication(const Publication& another)
{
    _id = another._id;
    _year = another._year;
    _title = another._title;
    _abstract = another._abstract;
    _source = another._source;
    _language = another._language;
    for (auto author: another._authors)
    {
        _authors.push_back(author);
    }
    for (auto refId: another._refIds)
    {
        _refIds.push_back(refId);
    }
}

Publication::~Publication()
{
    //dtor
}

void Publication::init(const json& jsonWork)
{
    // get id
    auto jsonId = jsonWork.find("id");
    if (jsonId != jsonWork.end()
            && jsonId.value().type() != json::value_t::null)
    {
        string strId = jsonId.value();
        _id = convertId(strId, 'W');
    }
    else
    {
        _id = 0;
    }

    // get year
    auto jsonYear = jsonWork.find("publication_year");
    if (jsonYear != jsonWork.end()
            && jsonYear.value().type() != json::value_t::null)
    {
        if (jsonYear.value().type() == json::value_t::number_integer
                || jsonYear.value().type() == json::value_t::number_unsigned)
        {
            _year = jsonYear.value();
        }
        else if (jsonYear.value().type() == json::value_t::string)
        {
            string s = jsonYear.value();
            _year = atoi(s.c_str());
        }
    }
    else
    {
        _year = 0;
    }

    // get title
    auto jsonTitle = jsonWork.find("title");
    if (jsonTitle != jsonWork.end()
            && jsonTitle.value().type() != json::value_t::null)
    {
        _title = wxStr(jsonTitle.value());
    }

    // get abstract
    auto jsonAbstract = jsonWork.find("abstract_inverted_index");
    if (jsonAbstract != jsonWork.end()
            && jsonAbstract.value().type() != json::value_t::null)
    {
        const map<string,vector<int>> index = jsonAbstract.value();

        int maxIndex = 0;
        for (auto iter = index.begin(); iter != index.end(); iter++)
        {
            for (int i: iter->second)
            {
                if (i > maxIndex)
                {
                    maxIndex = i;
                }
            }
        }

        vector<string> abstractTokens;
        abstractTokens.resize(maxIndex + 1);
        for (auto iter = index.begin(); iter != index.end(); iter++)
        {
            string s = iter->first;
            for (int i: iter->second)
            {
                assert(i >= 0 && i <= maxIndex);
                abstractTokens[i] = s;
            }
        }

        stringstream ss;
        for (size_t i = 0; i < abstractTokens.size(); i++)
        {
            if (i >0)
                ss << " ";
            ss << abstractTokens[i];
        }
        _abstract = wxStr(ss.str());
    }

    // get source
    auto jsonLo = jsonWork.find("primary_location");
    if (jsonLo != jsonWork.end()
            && jsonLo.value().type() != json::value_t::null)
    {
        auto jsonSo = jsonLo.value().find("source");
        if (jsonSo != jsonLo.value().end()
                && jsonSo.value().type() != json::value_t::null)
        {
            auto jsonDN = jsonSo.value().find("display_name");
            if (jsonDN != jsonSo.value().end()
                    && jsonDN.value().type() == json::value_t::string)
            {
                _source = wxStr(jsonDN.value());
            }
        }
    }

    // get language
    auto jsonLa = jsonWork.find("language");
    if (jsonLa != jsonWork.end()
            && jsonLa.value().type() != json::value_t::null)
    {
        _language = wxStr(jsonLa.value());
    }

    // get authors
    _authors.clear();
    auto jsonAus = jsonWork.find("authorships");
    if (jsonAus != jsonWork.end()
            && jsonAus.value().type() != json::value_t::null)
    {
        for (auto &authorship: jsonAus.value())
        {
            const auto au = authorship.find("author");
            if (au != authorship.end()
                    && au.value().type() != json::value_t::null)
            {
                auto &author = au.value();
                const auto dn = author.find("display_name");
                if (dn != author.end() && dn.value().type() != json::value_t::null)
                {
                    string authorName = dn.value();
                    if (authorName.find(",") != string::npos)
                    {
                        vector<string> fields = splitString(authorName, ",");
                        authorName = "";
                        for (int i = fields.size() - 1; i >= 0; i--)
                        {
                            if (authorName.size() > 0)
                                authorName += " ";
                            authorName += fields[i];
                        }
                    }
                    _authors.push_back(wxStr(authorName));
                }
            }
        }
    }

    // get reference ids
    _refIds.clear();
    auto jsonRW = jsonWork.find("referenced_works");
    if (jsonRW != jsonWork.end() &&
            jsonRW.value().type() != json::value_t::null)
    {
        for (string strRefId: jsonRW.value())
        {
            uint64_t refId = convertId(strRefId, 'W');
            _refIds.push_back(refId);
        }
    }
}

void Publication::init(const map<string,string>& work)
{
    auto keyToId = work.find("id");
    if (keyToId != work.end())
    {
        _id = stoull(keyToId->second);
    }

    auto keyToYear = work.find("year");
    if (keyToYear != work.end())
    {
        _year = atoi(keyToYear->second.c_str());
    }

    auto keyToTitle = work.find("title");
    if (keyToTitle != work.end())
    {
        _title = allocateWStr(keyToTitle->second);
    }

    auto keyToAbstract = work.find("abstract");
    if (keyToAbstract != work.end())
    {
        _abstract = allocateWStr(keyToAbstract->second);
    }

    auto keyToSource = work.find("source");
    if (keyToSource != work.end())
    {
        _source = allocateWStr(keyToSource->second);
    }

    auto keyToLanguage = work.find("language");
    if (keyToLanguage != work.end())
    {
        _language = allocateWStr(keyToLanguage->second);
    }

    auto keyToAuthors = work.find("authors");
    if (keyToAuthors != work.end())
    {
        _authors = splitWString(keyToAuthors->second, ",");
    }

    auto keyToRefIds = work.find("ref_ids");
    if (keyToRefIds != work.end())
    {
        vector<string> refIdStrs = splitString(keyToRefIds->second, ",");
        for(string refIdStr: refIdStrs)
        {
            _refIds.push_back(stoull(refIdStr));
        }
    }
}

void Publication::parse(const string& strJson)
{
    auto jsonWork = json::parse(strJson);
    init(jsonWork);
}

void Publication::writeWoS(std::ofstream &streamOut, const map<uint64_t, Publication> &publications)
{
    streamOut << "PT J" <<endl;

    if (_authors.size() == 0)
    {
        streamOut << "AU unknown author" << endl;
    }
    else
    {
        for (size_t i = 0; i < _authors.size(); i++)
        {
            streamOut << (i == 0? "AU ":"   ") << _authors[i] << endl;
        }
    }

    streamOut << "TI " << _title << endl;
    streamOut << "SO " << _source << endl;
    streamOut << "DT Article" << endl;
    std::string ab = _abstract.ToStdString();
    removeCharsFromString(ab, "\r\n");
    streamOut << "AB " << ab << endl;

    int iCR = 0;
    for (size_t i = 0; i < _refIds.size(); i++)
    {
        auto ridToPub = publications.find(_refIds[i]);
        if (ridToPub == publications.end() || ridToPub->second._id != _refIds[i])
            continue;
        const Publication pubCR = ridToPub->second;
        streamOut << (iCR == 0 ? "CR ":"   ")
                  << (pubCR._authors.size() == 0 ? "no author":pubCR._authors[0])
                  << ", " << pubCR._year << ", "
                  << pubCR._title << endl;
        iCR++;
    }

    streamOut << "PY " << _year << endl;
    streamOut << "ER" << endl;
    streamOut << endl;
}

