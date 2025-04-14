#include "List.h"
#include <iostream>

List::List(int size)
{
    this->data = new Node[size];
    this->pos = 0;
    this->start = 0;
}
List::~List()
{
    delete[] this->data;
}
int List::add(int value)
{
    Node *aux = start;
    Node *ant = 0;
    Node *node = &this->data[pos++];
    node->value = value;
    node->next = 0;
    if (start == 0)
    {
        this->start = node;
        return 0;
    }
    while (aux)
    {
        if (value == aux->value)
        {
            pos--;
            return 1;
        }
        if (value < aux->value)
        {
            if (ant == 0)
            {
                node->next = start;
                start = node;
            }
            else
            {
                ant->next = node;
                node->next = aux;
            }
            return 0;
        }
        ant = aux;
        aux = aux->next;
    }
    ant->next = node;
    return 0;
}

void List::print()
{
    Node *aux = this->start;
    while (aux)
    {
        std::cout << aux->value << "\n";
        aux = aux->next;
    }
}

int List::get(int pos)
{
    Node *aux = this->start;
    for (int i = 0; i < pos; i++)
    {
        aux = aux->next;
    }
    return aux->value;
}