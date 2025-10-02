#pragma once
using namespace std;

class Order
{

};

class OrdersList
{
    public:
        bool operator == (const OrdersList& ordersList) {return true;} //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT
        friend ostream& operator << (ostream &output, const OrdersList& ordersList) {return output;} //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT
        void add(Order order) {}; //DECLARED FOR PLAYER FUNCTIONALITY, PLEASE IMPLEMENT
};