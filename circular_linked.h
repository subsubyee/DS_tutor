#ifndef CIRCULAR_LINKED_H
#define CIRCULAR_LINKED_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include "node.h"

struct CIRCULAR_LINKED {
    Node* tail;          // 큐의 가장 마지막 노드를 가리키는 포인터 (tail->next는 head)
    int size;            // 현재 큐의 노드 개수
    QMainWindow* parent; // 부모 윈도우 포인터

    QWidget *stackContainer;

    // UI 컴포넌트 변수
    QGroupBox *leftGroup;
    QVBoxLayout *leftLayout;

    QLineEdit* enqueueInput;
    QPushButton* enqueueButton;

    QLabel* dequeueOutput;
    QPushButton* dequeueButton;

    QLabel* firstOutput;
    QPushButton* firstButton;

    QLabel* isEmptyOutput;
    QPushButton* isEmptyButton;

    QLabel* lenOutput;
    QPushButton* lenButton;

    QLabel* rotateOutput;
    QPushButton* rotateButton;

    QLabel* statusOutput;   // 마우스 클릭 조작 가이드 안내용 일반 상태창

    // 멤버 함수 선언
    CIRCULAR_LINKED(QMainWindow* _parent);
    ~CIRCULAR_LINKED();
    QWidget* setting();
    void update();
    void enqueue();
    void dequeue();
    void first();
    void is_empty();
    void _len();
    void rotate();
};

#endif // CIRCULAR_LINKED_H