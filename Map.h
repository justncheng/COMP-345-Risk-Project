#pragma once
using namespace std;

class Territory
{
    public:
        bool operator != (const Territory& territory) {return true;} //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT
        friend ostream& operator << (ostream &output, const Territory& territory) {return output;} //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT
};