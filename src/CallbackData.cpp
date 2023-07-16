#include "CallbackData.h"
#include "AbstractTask.h"

uint64_t CallbackData::_numDataRead = 0;
uint64_t CallbackData::_numDataWrite = 0;
uint64_t CallbackData::_sizeRead = 0;
uint64_t CallbackData::_sizeWrite = 0;

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
        _sizeRead += (result[columnNames[i]] = argv[i] ? argv[i] : "NULL").size();
    }
    data->results.push_back(result);
    data->_i++;
    _numDataRead++;
    if (data->_n >0)
    {
        if (data->_i < data->_n)
            return 0;
        else
            return -1;
    }
    else
    {
        if (AbstractTask::cancelled())
            return -1;
        else
            return 0;
    }
}

uint64_t CallbackData::getNumDataRead()
{
    return _numDataRead;
}

uint64_t CallbackData::getSizeRead()
{
    return _sizeRead;
}

uint64_t CallbackData::getNumDataWrite()
{
    return _numDataWrite;
}

uint64_t CallbackData::getSizeWrite()
{
    return _sizeWrite;
}

void CallbackData::updateWriteCount(uint64_t deltaNumData, uint64_t deltaSize)
{
    _sizeWrite += deltaSize;
    _numDataWrite += deltaNumData;
}
