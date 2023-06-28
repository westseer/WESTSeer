#ifndef GENERALCONFIG_H
#define GENERALCONFIG_H
#include <string>

class GeneralConfig
{
    public:
        GeneralConfig();
        virtual ~GeneralConfig();
        const std::string getEmail()
        {
            return _email;
        }
        const std::string getDatabase()
        {
            return _database;
        }
        const int getBiterms()
        {
            return _biterms;
        }
        const double getTfirdf()
        {
            return _tfirdf;
        }
        const int getCitations()
        {
            return _citations;
        }
        void setEmail(std::string value);
        void setDatabase(std::string value);
        void setBiterms(int value);
        void setTfirdf(double value);
        void setCitations(int value);
        const std::string getLogFile();

    protected:

    private:
        std::string _email;
        std::string _database;
        int _obYears;
        int _biterms;
        double _tfirdf;
        int _citations;
};

#endif // GENERALCONFIG_H
