#include "LLMConfig.h"
#include "../WESTSeerApp.h"

LLMConfig::LLMConfig()
{
    //ctor
    wxFileConfig *config = WESTSeerApp::getFileConfig();
}

LLMConfig::~LLMConfig()
{
    //dtor
}
