#ifndef QUEUE_H
#define QUEUE_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct QUEUE {
    QString* list;
    int str;
    int fin;
    int max;
    int size;

    QMainWindow* parent;

    Ui::MainWindow *ui;

    QLabel** queueCells;

    QLineEdit* enqueueInput;
    QLabel* dequeueOutput;
    QLabel* firstOutput;
    QLabel* isEmptyOutput;
    QLabel* lenOutput;
    QLabel* statusOutput;

    QPushButton* enqueueButton;
    QPushButton* dequeueButton;
    QPushButton* firstButton;
    QPushButton* isEmptyButton;
    QPushButton* lenButton;

    QUEUE(int _max, QMainWindow* _parent);
    ~QUEUE();
    QWidget* setting();
    void update();
    void enqueue();
    void dequeue();
    void first();
    void is_empty();
    void _len();
};
#endif // QUEUE_H