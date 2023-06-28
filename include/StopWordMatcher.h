#ifndef STOPWORDMATCHER_H
#define STOPWORDMATCHER_H
#include <AbstractMatcher.h>

class StopWordMatcher : public AbstractMatcher
{
    public:
        StopWordMatcher();
        StopWordMatcher(Type type);
        virtual ~StopWordMatcher();
        virtual AbstractMatcher *insertToken(const std::string &token, bool last);
        virtual AbstractMatcher *matchToken(const std::string &token);

    protected:

    private:
};

#endif // STOPWORDMATCHER_H
