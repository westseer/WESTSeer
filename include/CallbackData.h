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
        static uint64_t getNumDataRead();
        static uint64_t getSizeRead();
        static uint64_t getNumDataWrite();
        static uint64_t getSizeWrite();
        static void updateWriteCount(uint64_t deltaNumData, uint64_t deltaSize);


    protected:

    private:
        int _n;
        int _i;
        static uint64_t _numDataRead;
        static uint64_t _numDataWrite;
        static uint64_t _sizeRead;
        static uint64_t _sizeWrite;
};

#endif // CALLBACKDATA_H
