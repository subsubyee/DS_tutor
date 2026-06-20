#ifndef DEQUE_H
#define DEQUE_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

struct DEQUE {
    QString* list;
    int str;
    int fin;
    int max;
    int size;
    QMainWindow* parent;

    QLabel** dequeCells;

    QLineEdit* afInput;
    QPushButton* afButton;

    QLineEdit* alInput;
    QPushButton* alButton;

    QLabel* dfOutput;
    QPushButton* dfButton;

    QLabel* dlOutput;
    QPushButton* dlButton;

    QLabel* fOutput;
    QPushButton* fButton;

    QLabel* lOutput;
    QPushButton* lButton;

    QLabel* isEmptyOutput;
    QPushButton* isEmptyButton;

    QLabel* lenOutput;
    QPushButton* lenButton;

    QLabel* statusOutput;

    DEQUE(int _max, QMainWindow* _parent);
    ~DEQUE();
    QWidget* setting();
    void update();
    void add_first();
    void add_last();
    void delete_first();
    void delete_last();
    void first();
    void last();
    void is_empty();
    void _len();
};

#endif // DEQUE_H