#include "EmailValidator.h"

bool isChar(char c)
{
    return ((c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z'));
}

// Function to check the character
// is an digit or not
bool isDigit(const char c)
{
    return (c >= '0' && c <= '9');
}

EmailValidator::EmailValidator(std::string email)
{
    //ctor
    // Check the first character
    // is an alphabet or not
    if (!isChar(email[0]))
    {

        // If it's not an alphabet
        // email id is not valid
        _valid = false;
        return;
    }

    // Variable to store position
    // of At and Dot
    int At = -1, Dot = -1;

    // Traverse over the email id
    // string to find position of
    // Dot and At
    for (size_t i = 0;
            i < email.length(); i++)
    {

        // If the character is '@'
        if (email[i] == '@')
        {

            At = i;
        }

        // If character is '.'
        else if (email[i] == '.')
        {

            Dot = i;
        }
    }

    // If At or Dot is not present
    if (At == -1 || Dot == -1)
    {

        _valid = false;
        return;
    }

    // If Dot is present before At
    if (At > Dot)
    {
        _valid = false;
        return;
    };

    // If Dot is present at the end
    _valid = !(Dot >= (int)(email.length() - 1));
}

EmailValidator::~EmailValidator()
{
    //dtor
}
