#include "Matcher.h"
#include <StringProcessing.h>
#include <porter2_stemmer.h>

Matcher::Matcher()
{
    //ctor
}

Matcher::Matcher(Type type)
{
    _type = type;
}

Matcher::~Matcher()
{
    //dtor
}

AbstractMatcher *Matcher::insertToken(const std::string &token, bool last)
{
    std::string stem = normalize(token);
    Porter2Stemmer::stem(stem);
    auto tokenToSub = _subMatchers.find(stem);
    if (tokenToSub != _subMatchers.end())
    {
        if (last && tokenToSub->second->_type != TERM)
            tokenToSub->second->_type = TERM;
        return tokenToSub->second;
    }
    Matcher * sub = new Matcher(last ? TERM : PART);
    _subMatchers[stem] = sub;
    return sub;
}

AbstractMatcher *Matcher::matchToken(const std::string &token)
{
    std::string stem = normalize(token);
    Porter2Stemmer::stem(stem);
    auto tokenToSub = _subMatchers.find(stem);
    return tokenToSub != _subMatchers.end() ? tokenToSub->second : NULL;
}

void Matcher::load(std::string &strTerms)
{
    std::vector<std::string> terms = splitString(strTerms, ",");
    for (std::string &term: terms)
    {
        std::vector<std::string> tokens = splitString(term, " ");
        insertTerm(tokens, 0);
    }
}
