#ifndef TREE_SORT_H
#define TREE_SORT_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QRandomGenerator>
#include <QSet>
#include <QList>
#include <QVector>
#include <QGraphicsPathItem>

// 노드 구조체 정의 (제공해주신 Quiz_Node 변수 규격 일치)
struct tree_node {
    int key;
    tree_node *left, *right;
    bool isRevealed;
    bool leftChecked;
    bool rightChecked;
    tree_node(int k) : key(k), left(nullptr), right(nullptr), isRevealed(false), leftChecked(false), rightChecked(false) {}
};

class TREE_SORT;

// 마우스 클릭 이벤트를 감지하기 위한 커스텀 그래픽스 아이템
class TreeNodeItem : public QGraphicsPathItem {
private:
    int key;
    TREE_SORT* sortManager;
public:
    TreeNodeItem(int k, TREE_SORT* manager, QGraphicsItem* parent = nullptr);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};

class TREE_SORT : public QObject {
    Q_OBJECT
public:
    explicit TREE_SORT(QMainWindow* _parent = nullptr);
    ~TREE_SORT();
    QWidget* setting();

    // 노드 클릭 시 호출되는 퀴즈 정렬 검증 로직
    void handleNodeClick(int clickedKey);

private:
    tree_node* root;
    int size;
    QMainWindow* parent;

    // 제공해주신 알고리즘 바인딩용 변수
    tree_node* currentFocusNode;
    int quizTargetKey;

    // Binary Tree Sort(LNR) 검증용 상태 변수
    QVector<int> correctOrder;       // 올바른 중위 순회 정렬 순서 배열
    QSet<int> correctClickedNodes;   // 이미 올바르게 맞춘 노드 집합
    int lastWrongClickedKey;         // 최근 틀린 노드 키 (빨간색 하이라이트용)
    int currentSortIndex;            // 현재 클릭해야 하는 정렬 순서 인덱스

    // UI 컴포넌트
    QGraphicsScene *scene;
    QGraphicsView *view;
    QLabel *statusOutput;
    QPushButton *generateButton;
    QPushButton *resetButton;

    // 내부 로직 함수
    void generateRandomTree();       // 요청하신 랜덤 트리 생성 알고리즘
    void calculateInOrder(tree_node* node, QVector<int>& order);
    void updateVisuals();
    void drawNode(tree_node* node, int level, double x_pos, double x_offset);
    void clearTree(tree_node* node);

private slots:
    void startQuiz();
    void resetQuiz();
};

#endif