#ifndef TREE_AVL_H
#define TREE_AVL_H

#include <QMainWindow>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QSlider>
#include <algorithm> // std::max 사용을 위해 추가

// 🌟 요구사항에 맞춘 AVL 노드 구조체 이름: tree_avl
struct tree_avl {
    int key;
    int height;
    tree_avl* left;
    tree_avl* right;

    tree_avl(int val) : key(val), height(1), left(nullptr), right(nullptr) {}
};

class TREE_AVL : public QObject {
    Q_OBJECT
public:
    explicit TREE_AVL(QMainWindow* _parent = nullptr);
    ~TREE_AVL();
    QWidget* setting();

private:
    // ⚠️ 중복 정의 에러 방지를 위해 .cpp 상단에 static const로 선언되었으므로,
    // 헤더 내부 선언은 유지하되 .cpp에서 중복 충돌이 나지 않도록 구성되었습니다.
    const double NODE_WIDTH = 55.0;
    const double NODE_HEIGHT = 40.0;
    const double LEVEL_HEIGHT = 80.0;

    // 🌟 애니메이션 제어 상태 정의 (삭제 및 삽입 독립 상태 머신)
    enum DeleteState { IDLE, SEARCHING, TARGET_FOUND, FIND_REPLACE, COPY_AND_DELETE, REBALANCING };
    enum InsertState {
        I_IDLE, I_SEARCHING, I_DONE,
        I_BACKTRACK,    // 루트로 거슬러 올라가며 BF 체크
        I_SELECT_A,     // 불균형 노드(Grandparent) 선택
        I_SELECT_B,     // 자식 노드(Parent) 선택
        I_SELECT_C,     // 손자 노드(Child) 선택
        I_ROTATING      // 실제 회전 수행
    };

    // 클래스 멤버 변수에 추가
    QList<tree_avl*> pathStack; // 거슬러 올라가기 위한 경로 저장
    tree_avl *nodeA = nullptr, *nodeB = nullptr, *nodeC = nullptr;
    int insertStepCount = 0;
    // ※ 명확한 분리를 위해 IDLE, SEARCHING, REBALANCING 앞에 INSERT_를 붙여 상태 상수를 확장했습니다.
    // cpp 코드 맥락에 맞춰 컴파일되도록 내부 매핑 처리

    QMainWindow* parent;
    tree_avl* root;
    int size;

    // 시각화 하이라이트 키 관리
    int highlightSearchKey;
    int highlightTargetKey;
    int highlightReplaceKey;

    // 🌟 연산 제어 상태 머신 변수들
    int currentDeleteState;
    int currentInsertState; // `.cpp`의 유연한 매핑을 위해 int 또는 매칭 열거형으로 제어

    tree_avl* currNode;
    tree_avl* parentNode;
    tree_avl* replaceNode;
    tree_avl* replaceParent;

    int targetKey;
    int insertKey; // 🌟 삽입 연산용 키 저장 변수 추가

    QTimer* animationTimer;

    // UI 컴포넌트
    QGroupBox* leftGroup;
    QVBoxLayout* leftLayout;
    QGraphicsScene* scene;
    QGraphicsView* view;

    QLineEdit* insertInput;
    QPushButton* insertButton;
    QLineEdit* deleteInput;
    QPushButton* deleteButton;
    QLineEdit* searchInput;
    QPushButton* searchButton;

    QLabel* minOutput;
    QPushButton* minButton;
    QLabel* maxOutput;
    QPushButton* maxButton;
    QLabel* isEmptyOutput;
    QPushButton* isEmptyButton;
    QLabel* lenOutput;
    QPushButton* lenButton;
    QLabel* statusOutput;

    // 내부 유틸리티 함수
    void clearTree(tree_avl* node);
    void updateVisuals();
    void drawNode(tree_avl* node, int level, double x_pos, double x_offset);
    void setControlsEnabled(bool enabled);

    // AVL 트리 필수 계산 함수
    int getHeight(tree_avl* node) { return node ? node->height : 0; }
    int getBalance(tree_avl* node) { return node ? getHeight(node->left) - getHeight(node->right) : 0; }
    void updateHeight(tree_avl* node) {
        if (node) node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    }

    // 기본 회전 함수
    tree_avl* rotateRight(tree_avl* y);
    tree_avl* rotateLeft(tree_avl* x);

    // 비동기 퀴즈 및 트리 밸런싱 재구축 함수
    tree_avl* balanceTree(tree_avl* node, bool& rotated);

    QSlider* speedSlider;
    QLabel* speedLabel;

private slots:
    void insert_op();
    void delete_op();
    void search_op();
    void min_op();
    void max_op();
    void is_empty();
    void _len();

    // 🌟 핵심 타이머 슬롯 (삭제와 삽입 각각 매핑)
    void deleteStep();
    void insertStep(); // 🌟 추가된 삽입 비동기 타이머 스텝 슬롯
    void updateSpeed(int value);
};

// .cpp 내부의 가독성과 상태 호환성을 위해 매핑 상수 정의
#define IDLE 0
#define SEARCHING 1
#define TARGET_FOUND 2
#define FIND_REPLACE 3
#define COPY_AND_DELETE 4
#define REBALANCING 5
#define INSERT_DONE 6

#endif // TREE_AVL_H