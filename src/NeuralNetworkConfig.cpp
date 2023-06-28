#include "NeuralNetworkConfig.h"
#include "../WESTSeerApp.h"

NeuralNetworkConfig::NeuralNetworkConfig()
{
    //ctor
    wxFileConfig *config = WESTSeerApp::getFileConfig();

}

NeuralNetworkConfig::~NeuralNetworkConfig()
{
    //dtor
}
