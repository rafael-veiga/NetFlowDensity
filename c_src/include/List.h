#ifndef LIST_H
#define LIST_H

struct Node
{
    int value = 0;
    Node *next = 0;
};

class List
{
public:
    List(int size);
    ~List();
    int add(int value);
    void print();
    int get(int pos);

private:
    int pos;
    Node *data;
    Node *start;
};
#endif