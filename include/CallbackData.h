#ifndef CALLBACKDATA_H
#define CALLBACKDATA_H
#include <string>
#include <vector>
#include <map>
using namespace std;

class CallbackData
{
    public:
        CallbackData(int n = 0);
        virtual ~CallbackData();
        vector<map<string,string>> results;
        static int sqliteCallback(void *pData, int argc, char**argv, char**columnNames);

    protected:

    private:
        int _n;
        int _i;
};

#endif // CALLBACKDATA_H
