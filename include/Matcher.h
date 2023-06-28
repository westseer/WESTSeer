#ifndef MATCHER_H
#define MATCHER_H
#include <AbstractMatcher.h>

class Matcher: public AbstractMatcher
{
    public:
        Matcher();
        Matcher(Type type);
        virtual ~Matcher();
        virtual AbstractMatcher *insertToken(const std::string &token, bool last);
        virtual AbstractMatcher *matchToken(const std::string &token);
        void load(std::string &strTerms);

    protected:

    private:
};

#endif // MATCHER_H
