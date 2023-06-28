#include "StopWordMatcher.h"
#include <StopWords.h>
#include <StringProcessing.h>
#include <ctype.h>

StopWordMatcher::StopWordMatcher()
{
    _type = PART;

    //ctor
    for (char c = '!'; c < '0'; c++)
    {
        std::string s;
        s += c;
        insertToken(s, true);
    }
    for (char c = ':'; c < 'A'; c++)
    {
        std::string s;
        s += c;
        insertToken(s, true);
    }
    for (char c = '['; c < 'a'; c++)
    {
        std::string s;
        s += c;
        insertToken(s, true);
    }
    for (char c = '{'; c <= '~'; c++)
    {
        std::string s;
        s += c;
        insertToken(s, true);
    }

    for (const char* stopword: SmartStopList)
    {
        std::string s(stopword);
        std::vector<std::string> tokens = tokenize(s);
        if (tokens.size() > 0)
            insertTerm(tokens, 0);
    }
}

StopWordMatcher::StopWordMatcher(Type type)
{
    _type = type;
}

StopWordMatcher::~StopWordMatcher()
{
    //dtor
}

AbstractMatcher *StopWordMatcher::insertToken(const std::string &token, bool last)
{
    std::string normalToken = normalize(token);
    auto tokenToSub = _subMatchers.find(normalToken);
    if (tokenToSub != _subMatchers.end())
        return tokenToSub->second;
    Type type;
    if (last)
    {
        if (isalpha(token[0]))
            type = STOP_WORD;
        else
            type = PUNCT;
    }
    else
        type = PART;
    StopWordMatcher * sub = new StopWordMatcher(type);
    _subMatchers[normalToken] = sub;
    return sub;
}

AbstractMatcher *StopWordMatcher::matchToken(const std::string &token)
{
    std::string normalToken = normalize(token);
    auto tokenToSub = _subMatchers.find(normalToken);
    return tokenToSub != _subMatchers.end() ? tokenToSub->second : NULL;
}
