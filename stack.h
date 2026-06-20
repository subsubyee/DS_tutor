#ifndef STACK_H
#define STACK_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

struct STACK {
    QString* list;
    int max;
    int len;
    QMainWindow* parent;

    QLabel** stackCells;
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
    QLabel* statusOutput;   // 마우스 클릭 조작 가이드 안내용 일반 상태창

    STACK(int _max, QMainWindow* _parent);
    ~STACK();
    QWidget* setting();
    void update();
    void push();
    void pop();
    void top();
    void is_empty();
    void _len();
};

#endif // STACK_H