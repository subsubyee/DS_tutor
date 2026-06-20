#ifndef LINKED_STACK_H
#define LINKED_STACK_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include "node.h"

struct LINKED_STACK {
    Node* head;
    int size;
    QMainWindow* parent;

    // 동적 화면 제어를 위한 컴포넌트 보관
    QGroupBox *leftGroup;
    QVBoxLayout *leftLayout;

    QLineEdit* pushInput;
    QPushButton* pushButton;
    QLabel* popOutput;
    QPushButton* popButton;
    QLabel* topOutput;
    QPushButton* topButton;
    QLabel* isEmptyOutput;
    QPushButton* isEmptyButton;
    QLabel* lenOutput;
    QPushButton* lenButton;
    QLabel* statusOutput;

    // 생성자에서 _max 인자를 완전히 없앴습니다.
    LINKED_STACK(QMainWindow* _parent);
    ~LINKED_STACK();
    QWidget* setting();
    void update();
    void push();
    void pop();
    void top();
    void is_empty();
    void _len();
};

#endif // LINKED_STACK_H