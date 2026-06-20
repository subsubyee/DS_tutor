#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "stack.h"
#include "queue.h"
#include "deque.h"
#include "linked_stack.h"
#include "circular_linked.h"
#include "doubly_linked.h"
#include "tree_bst.h"
#include "tree_bst_quiz.h"
#include "tree_avl.h"
#include "tree_avl_insert_quiz.h"
#include "tree_avl_delete_quiz.h"
#include "hash.h"
#include "tree_sort.h"
#include "bubble_sort.h"
#include "insertion_sort.h"
#include "selection_sort.h"
#include "shell_sort.h"
#include "merge_sort.h"
#include "quick_sort.h"
#include "radix_sort.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;
    STACK* stack;
    QUEUE* queue;
    DEQUE* deque;
    LINKED_STACK* linked_stack;
    CIRCULAR_LINKED* circular_linked;
    DOUBLY_LINKED* doubly_linked;
    TREE_BST* tree_bst;
    TREE_BST_QUIZ* tree_bst_quiz;
    TREE_AVL* tree_avl;
    TREE_AVL_INSERT_QUIZ* tree_avl_insert_quiz;
    TREE_AVL_DELETE_QUIZ* tree_avl_delete_quiz;
    HASH* hash;
    TREE_SORT* tree_sort;
    BUBBLE_SORT* bubble_sort;
    INSERTION_SORT* insertion_sort;
    SELECTION_SORT* selection_sort;
    SHELL_SORT* shell_sort;
    MERGE_SORT* merge_sort;
    QUICK_SORT* quick_sort;
    RADIX_SORT* radix_sort;

    void updateSidebarHighlight(QPushButton* selectedButton);
    QList<QPushButton*> sidebarButtons; // 모든 사이드바 버튼을 관리할 리스트
};
#endif // MAINWINDOW_H