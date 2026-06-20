#ifndef TREE_BST_QUIZ_H
#define TREE_BST_QUIZ_H

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QList>

// ==========================================
// BST 퀴즈용 노드 구조체
// ==========================================
struct Quiz_Node {
    int key;
    Quiz_Node* left;
    Quiz_Node* right;

    // UI 전역 초기화를 위해 보존하는 가시성 플래그 (기본값 true)
    bool isRevealed;
    bool leftChecked;
    bool rightChecked;

    Quiz_Node(int _key) {
        key = _key;
        left = nullptr;
        right = nullptr;
        isRevealed = true;
        leftChecked = true;
        rightChecked = true;
    }
};

// ==========================================
// TREE_BST_QUIZ 메인 제어 클래스
// ==========================================
class TREE_BST_QUIZ : public QObject {
    Q_OBJECT

public:
    explicit TREE_BST_QUIZ(QMainWindow* _parent = nullptr);
    ~TREE_BST_QUIZ();

    // 퀴즈 샌드박스 위젯 초기 레이아웃 생성 및 반환
    QWidget* setting();

private slots:
    // 퀴즈 생성 및 초기화
    void startNewQuiz();
    // 선택된 데이터 검증 및 실제 BST 트리 재구성 수행
    void verifyAndExecute();

private:
    // 퀴즈 엔진 상태 정의 (대기 / 진행중)
    enum QuizState {
        PRE_START,
        PLAYING
    };

    // 트리 메모리 해제 관리 함수
    void clearTree(Quiz_Node* node);
    // 무작위 유니크 키 배치 및 트리 빌드
    void generateRandomTree();

    // 시각화 그래픽스 갱신 및 재조정
    void updateVisuals();
    // 재귀 트레이싱 노드 렌더러
    void drawNode(Quiz_Node* node, int level, double x_pos, double x_offset);

protected:
    // 캔버스 내부 클릭 이벤트를 다이렉트로 가로채서 처리하는 필터
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    // 상위 UI 컴포넌트 포인터 및 트리 관리 변수
    QMainWindow* parent;
    Quiz_Node* root;
    int size;
    Quiz_Node* currentFocusNode;

    // 퀴즈 상태 세부 변수
    int quizTargetKey;
    QuizState currentQuizState;

    // [핵심 교정] 클릭 순서 제어 및 토글 처리를 위한 단일 컨테이너 채택
    // index 0: userSelectedTarget (A)
    // index 1: userSelectedReplacement (B)
    QList<Quiz_Node*> userSelectedNodes;

    // UI 레이아웃 위젯 포인터
    QGraphicsScene* scene;
    QGraphicsView* view;
    QGroupBox* leftGroup;
    QVBoxLayout* leftLayout;

    QLabel* quizQuestionLabel;
    QLabel* statusOutput;

    QPushButton* startQuizButton;
    QPushButton* executeDeleteButton;

    // 그래프 노드 렌더링 규격 상수 정의
    const double NODE_WIDTH = 55.0;
    const double NODE_HEIGHT = 40.0;
    const double LEVEL_HEIGHT = 70.0;
};

#endif // TREE_BST_QUIZ_H