#ifndef TREE_BST_H
#define TREE_BST_H

#include <QObject>
#include <QMainWindow>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QSlider>
#include <QTimer> // 🌟 추가: WebAssembly 표준 비동기 타이머

struct BST_Node {
    int key;
    BST_Node* left;
    BST_Node* right;
    BST_Node(int k) : key(k), left(nullptr), right(nullptr) {}
};

class TREE_BST : public QObject {
    Q_OBJECT
public:
    explicit TREE_BST(QMainWindow* _parent = nullptr);
    ~TREE_BST();
    QWidget* setting();

private:
    const double NODE_WIDTH = 55.0;
    const double NODE_HEIGHT = 40.0;
    const double LEVEL_HEIGHT = 80.0;

    void clearTree(BST_Node* node);
    void updateVisuals();
    void drawNode(BST_Node* node, int level, double x_pos, double x_offset);

    // 🌟 WebAssembly 친화형 타이머 기반 상태 머신 함수들
    void delete_op();
    void deleteStep();
    void setControlsEnabled(bool enabled); // 애니메이션 중 오작동 방지 잠금장치

    void insert_op();
    void search_op();
    void min_op();
    void max_op();
    void is_empty();
    void _len();

    void updateSpeed(int value);

    QMainWindow* parent;
    BST_Node* root;
    int size;

    QGraphicsScene *scene;
    QGraphicsView *view;

    int highlightSearchKey;
    int highlightTargetKey;
    int highlightReplaceKey;

    // 🌟 애니메이션 단계를 기억할 상태 변수 정의
    enum DeleteState { IDLE, SEARCHING, TARGET_FOUND, FIND_REPLACE, COPY_AND_DELETE };
    DeleteState currentDeleteState;
    QTimer *animationTimer;
    int targetKey;
    BST_Node *currNode;
    BST_Node *parentNode;
    BST_Node *replaceNode;
    BST_Node *replaceParent;

    QGroupBox *leftGroup;
    QVBoxLayout *leftLayout;
    QLineEdit *insertInput;   QPushButton *insertButton;
    QLineEdit *deleteInput;   QPushButton *deleteButton;
    QLineEdit *searchInput;   QPushButton *searchButton;
    QLabel *minOutput;        QPushButton *minButton;
    QLabel *maxOutput;        QPushButton *maxButton;
    QLabel *isEmptyOutput;    QPushButton *isEmptyButton;
    QLabel *lenOutput;        QPushButton *lenButton;
    QLabel *statusOutput;

    QSlider* speedSlider;
    QLabel* speedLabel;
};

#endif // TREE_BST_H