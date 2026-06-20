#ifndef TREE_AVL_DELETE_QUIZ_H
#define TREE_AVL_DELETE_QUIZ_H

#include <QObject>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QEvent>

// ==========================================
// 1. AVL 퀴즈 노드 구조체 정의
// ==========================================
struct Quiz_AVL_Del_Node {
    int key;
    int height;
    Quiz_AVL_Del_Node* left;
    Quiz_AVL_Del_Node* right;

    Quiz_AVL_Del_Node(int val) {
        key = val;
        height = 1;
        left = nullptr;
        right = nullptr;
    }
};

// ==========================================
// 2. 퀴즈 내부 상태 제어 열거형
// ==========================================
enum class DeleteQuizState {
    STEP1_PRE_START,      // 퀴즈 시작 대기 상태
    STEP2_NAVIGATING,     // 노드 직접 검색 및 타겟 선택 중인 상태
    STEP3_DELETE_READY,   // 올바른 타겟 노드가 선택되어 삭제 실행 대기 상태
    STEP4_REBALANCE,      // 구조적 삭제 후 불균형 노드(A->B->C) 회전 검증 상태
    STEP5_QUIZ_COMPLETED  // 모든 회전 검증이 정상 완료된 상태
};

// ==========================================
// 3. 내부 자동 연산 회전 단계 저장 구조체
// ==========================================
struct DeleteRotationStep {
    int aKey; // 조부모 피벗 노드 (Grandparent)
    int bKey; // 부모 노드 (Parent)
    int cKey; // 자식/손자 노드 (Child)
};

// ==========================================
// 4. AVL 삭제 퀴즈 메인 컨트롤 클래스
// ==========================================
class TREE_AVL_DELETE_QUIZ : public QObject {
    Q_OBJECT

public:
    explicit TREE_AVL_DELETE_QUIZ(QMainWindow* _parent = nullptr);
    ~TREE_AVL_DELETE_QUIZ();

    // 시스템 UI 레이아웃 빌더 (시뮬레이터 사양 통일)
    QWidget* setting();

protected:
    // 캔버스 내 마우스 다이렉트 픽킹 처리를 위한 이벤트 필터 오버라이드
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void startNewQuiz();          // 퀴즈 초기화 및 무작위 트리 디플로이
    void executeNodeDeletion();  // 타겟 노드 내부 물리 트리 구조적 삭제 단행
    void executeRotation();      // 유저가 선택한 3개의 노드 조합 회전 검증 및 반영

private:
    // AVL 트리 유틸리티 내부 논리 함수
    void clearTree(Quiz_AVL_Del_Node* node);
    void updateHeight(Quiz_AVL_Del_Node* n);
    int getBalance(Quiz_AVL_Del_Node* n);
    Quiz_AVL_Del_Node* findSuccessor(Quiz_AVL_Del_Node* node);
    Quiz_AVL_Del_Node* pureBSTInsert(Quiz_AVL_Del_Node* node, int key);

    // 기본 고정 회전 연산자
    Quiz_AVL_Del_Node* rotateRight(Quiz_AVL_Del_Node* y);
    Quiz_AVL_Del_Node* rotateLeft(Quiz_AVL_Del_Node* x);

    // 퀴즈 알고리즘 및 검증 파이프라인 함수
    void generateRandomBaseTreeWithRotationCheck(); // 삭제 시 반드시 회전이 발생하는 트리 무작위 추출 생성
    void calculateCorrectAnswers();                  // 내부 백그라운드 가상 시뮬레이션을 통한 정답 회전 경로 선행 계산
    void updateVisuals();                           // 뷰포트 그래픽스 씬 렌더링 갱신
    void drawNode(Quiz_AVL_Del_Node* node, int level, double x_pos, double x_offset); // 컴팩트 노드 렌더러

private:
    // Qt UI 인프라 구조 프레임워크 객체 포인터
    QMainWindow* parent;
    QGraphicsScene* scene;
    QGraphicsView* view;

    QLabel* quizQuestionLabel;      // 상단 미션 지시문 카드형 보드
    QLabel* statusOutput;           // 하단 터미널 라이브 액션 로그창
    QPushButton* insertActionButton; // 구조적 물리 삭제 버튼 (Step 2)
    QPushButton* rotateButton;       // 불균형 피벗 회전 실행 버튼 (Step 3)

    // 퀴즈 핵심 비즈니스 로직 제어 변수
    Quiz_AVL_Del_Node* avlRoot;           // 실제 런타임 퀴즈 AVL 트리 루트 노드
    Quiz_AVL_Del_Node* currentFocusNode;    // 사용자가 마우스로 직접 클릭하여 선택한 현재 포커스 노드
    int quizTargetDeleteKey;              // 시스템이 미션으로 지정한 삭제 대상 목표 Key값

    DeleteQuizState currentQuizState;     // 현재 퀴즈 세션 진행 상태 단계 지표
    int currentStepIndex;                 // 다단계 회전 요구 시 현재 검증 진행 중인 인덱스 번호

    // [중요] 기존 방향키 탐색용 historyStack(QStack)을 제거하고 동적 마우스 픽킹 바인딩 리스트 배열로 통합
    QList<Quiz_AVL_Del_Node*> userSelectedNodes; // 사용자가 회전(A->B->C)을 위해 순서대로 마우스 클릭한 노드 저장소
    QList<DeleteRotationStep> expectedSteps;     // 시뮬레이터가 계산해둔 순차별 정답 Imbalance 패스 정보 리스트
};

#endif // TREE_AVL_DELETE_QUIZ_H