#ifndef TREE_AVL_INSERT_QUIZ_H
#define TREE_AVL_INSERT_QUIZ_H

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QList>
#include <QStack>

// AVL 퀴즈용 노드 구조체
struct Quiz_AVL_Ins_Node {
    int key;
    int height;
    Quiz_AVL_Ins_Node* left;
    Quiz_AVL_Ins_Node* right;

    // 더미 노드 판별 및 정답 검증용 변수
    bool isVirtual;
    Quiz_AVL_Ins_Node* virtual_parent;

    Quiz_AVL_Ins_Node(int val, bool isVirt = false, Quiz_AVL_Ins_Node* p = nullptr)
        : key(val), height(1), left(nullptr), right(nullptr),
        isVirtual(isVirt), virtual_parent(p) {}
};

struct RotationStep {
    int aKey;
    int bKey;
    int cKey;
};

class TREE_AVL_INSERT_QUIZ : public QObject {
    Q_OBJECT

public:
    explicit TREE_AVL_INSERT_QUIZ(QMainWindow* _parent = nullptr);
    virtual ~TREE_AVL_INSERT_QUIZ();

    QWidget* setting();

private slots:
    void startNewQuiz();
    void executeRotation();

private:
    void generateRandomBaseTree();
    void calculateCorrectAnswers();
    void clearTree(Quiz_AVL_Ins_Node* node);
    void updateHeight(Quiz_AVL_Ins_Node* n);
    int getBalance(Quiz_AVL_Ins_Node* n);

    // 회색 빈 노드를 트리에 붙이거나 해제하는 헬퍼 함수
    void attachVirtualNodes(Quiz_AVL_Ins_Node* node);
    void detachVirtualNodes(Quiz_AVL_Ins_Node* node);

    void updateVisuals();
    void drawNode(Quiz_AVL_Ins_Node* node, int level, double x_pos, double x_offset);

    bool eventFilter(QObject* obj, QEvent* event) override;

    Quiz_AVL_Ins_Node* pureBSTInsert(Quiz_AVL_Ins_Node* node, int key);
    Quiz_AVL_Ins_Node* rotateRight(Quiz_AVL_Ins_Node* y);
    Quiz_AVL_Ins_Node* rotateLeft(Quiz_AVL_Ins_Node* x);

private:
    enum class QuizState {
        STEP1_PRE_START,
        STEP2_CLICK_EMPTY_NODE,
        STEP4_REBALANCE,
        STEP5_QUIZ_COMPLETED
    };

    // [복원] 원래 원본 코드의 크기 그대로 지정
    static constexpr double NODE_WIDTH = 55.0;
    static constexpr double NODE_HEIGHT = 35.0;
    static constexpr double LEVEL_HEIGHT = 65.0;

    QMainWindow* parent;
    QGraphicsScene* scene;
    QGraphicsView* view;

    QLabel* quizQuestionLabel;
    QLabel* statusOutput;

    QPushButton* startQuizButton;
    QPushButton* rotateButton;

    Quiz_AVL_Ins_Node* avlRoot;
    Quiz_AVL_Ins_Node* currentFocusNode;

    int quizTargetInsertKey;
    int currentStepIndex;
    bool isActualInserted;

    QuizState currentQuizState;
    QList<RotationStep> expectedSteps;
    QList<Quiz_AVL_Ins_Node*> userSelectedNodes;
};

#endif // TREE_AVL_INSERT_QUIZ_H