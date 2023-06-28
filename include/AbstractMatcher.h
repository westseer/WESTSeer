#ifndef ABSTRACTMATCHER_H
#define ABSTRACTMATCHER_H
#include <string>
#include <vector>
#include <map>

class AbstractMatcher
{
public:
    enum Type
    {
        PUNCT,
        PART,
        STOP_WORD,
        TERM
    };
    Type _type;

    AbstractMatcher();
    virtual ~AbstractMatcher();
    virtual AbstractMatcher *insertToken(const std::string &token, bool last) = 0;
    virtual AbstractMatcher *matchToken(const std::string &token) = 0;
    std::vector<Type> match(const std::vector<std::string> &term, int i);
    void insertTerm(const std::vector<std::string> &term, int i);
    std::string getTerms();
    void clear();

protected:
    std::map<std::string, AbstractMatcher *> _subMatchers;
    void getTerms(std::string prefix, std::vector<std::string> &terms);

private:
};

#endif // ABSTRACTMATCHER_H
