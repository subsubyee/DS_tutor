#ifndef NODE_H
#define NODE_H

#include <QString.h>

struct Node {
    QString data;
    Node* next;
};

#endif // NODE_H