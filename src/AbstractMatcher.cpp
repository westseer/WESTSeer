#include "AbstractMatcher.h"
#include <sstream>

AbstractMatcher::AbstractMatcher()
{
    //ctor
    _type = PART;
}

AbstractMatcher::~AbstractMatcher()
{
    //dtor
    clear();
}

std::vector<AbstractMatcher::Type> AbstractMatcher::match(const std::vector<std::string> &term, int i)
{
    std::vector<Type> result;
    if (i < 0 || i >= (int) term.size())
        return result;

    AbstractMatcher *next = this->matchToken(term[i]);
    while (next != NULL)
    {
        result.push_back(next->_type);
        int nextI = i + result.size();
        if (nextI < (int) term.size())
            next = next->matchToken(term[nextI]);
        else
            next = NULL;
    }
    return result;
}

void AbstractMatcher::insertTerm(const std::vector<std::string> &term, int i)
{
    if (i < 0)
        return;
    if (i == (int) term.size() - 1)
    {
        insertToken(term[i], true);
    }
    else if (i < (int) term.size())
    {
        AbstractMatcher *sub = matchToken(term[i]);
        if (sub == NULL)
            sub = insertToken(term[i], false);
        sub->insertTerm(term, i+1);
    }
}

void AbstractMatcher::getTerms(std::string prefix, std::vector<std::string> &terms)
{
    if (_type == TERM && prefix.size() > 0)
    {
        terms.push_back(prefix);
    }
    for (auto partToSub: _subMatchers)
    {
        if (prefix.size() > 0)
        {
            partToSub.second->getTerms(prefix + " " + partToSub.first, terms);
        }
        else
        {
            partToSub.second->getTerms(partToSub.first, terms);
        }
    }
}

std::string AbstractMatcher::getTerms()
{
    std::stringstream ss;
    std::vector<std::string> terms;
    getTerms("", terms);
    for (size_t i = 0; i < terms.size(); i++)
    {
        if (i > 0)
            ss << ",";
        ss << terms[i];
    }
    return ss.str();
}

void AbstractMatcher::clear()
{
    for (auto tokenToSub: _subMatchers)
    {
        delete tokenToSub.second;
    }
    _subMatchers.clear();
}
