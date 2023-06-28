#include "CallbackData.h"

CallbackData::CallbackData(int n)
{
    //ctor
    _n = n;
    _i = 0;
}

CallbackData::~CallbackData()
{
    //dtor
}

int CallbackData::sqliteCallback(void *pData, int argc, char**argv, char**columnNames)
{
    CallbackData *data = (CallbackData *)pData;
    map<string,string> result;
    for (int i = 0; i < argc; i++)
    {
        result[columnNames[i]] = argv[i] ? argv[i] : "NULL";
    }
    data->results.push_back(result);
    data->_i++;
    if (data->_n >0)
    {
        if (data->_i < data->_n)
            return 0;
        else
            return -1;
    }
    else
    {
        return 0;
    }
}
