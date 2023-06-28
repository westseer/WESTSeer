#ifndef EMAILVALIDATOR_H
#define EMAILVALIDATOR_H
#include <string>

class EmailValidator
{
    public:
        EmailValidator(std::string email);
        virtual ~EmailValidator();
        inline bool valid()
        {
            return _valid;
        }
    protected:

    private:
        bool _valid;
};

#endif // EMAILVALIDATOR_H
