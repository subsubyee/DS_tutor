#ifndef DOUBLY_LINKED_H
#define DOUBLY_LINKED_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QEvent>
#include <QList>

// ==========================================
// 1. 이중 연결 리스트 노드 구조체 선언
// ==========================================
struct Double_Node {
    QString data;
    Double_Node* prev;
    Double_Node* next;
};

// ==========================================
// 2. DOUBLY_LINKED 클래스 정의
// ==========================================
// Qt의 이벤트 시스템(Event Filter) 및 시그널-슬롯을 사용하기 위해 QObject를 상속받습니다.
class DOUBLY_LINKED : public QObject {
    Q_OBJECT
public:
    // 핵심 링크드 리스트 포인터 및 상태 변수
    Double_Node* header;
    Double_Node* trailer;
    int size;
    QMainWindow* parent;

    // [왼쪽 영역] Deque 스타일의 동적 수직 적층 레이아웃용 변수
    QGroupBox* leftGroup;
    QVBoxLayout* leftLayout;
    QList<QLabel*> listCells;       // 화면에 그려지는 블록(QLabel)들의 리스트
    QList<Double_Node*> cellNodes;  // 각 블록과 실제 노드 메모리 주소를 1:1로 매핑하는 리스트

    // [오른쪽 영역] 제어용 입력창 컴포넌트 (Deque 스타일)
    QLineEdit* ifInput;   // insert_first 입력창
    QLineEdit* ilInput;   // insert_last 입력창
    QLineEdit* ibInput;   // insert_between 입력창

    // [오른쪽 영역] 제어용 출력창 컴포넌트 (Deque 스타일, 패널/썬큰 스타일 적용 대상)
    QLabel* dfOutput;       // delete_first 결과창
    QLabel* dlOutput;       // delete_last 결과창
    QLabel* fOutput;        // first 결과창
    QLabel* lOutput;        // last 결과창
    QLabel* isEmptyOutput;  // is_empty 결과창
    QLabel* lenOutput;      // len 결과창
    QLabel* statusOutput;   // 마우스 클릭 조작 가이드 안내용 일반 상태창

    // [오른쪽 영역] 함수 트리거용 버튼 컴포넌트
    QPushButton* ifButton;
    QPushButton* ilButton;
    QPushButton* ibButton;
    QPushButton* dfButton;
    QPushButton* dlButton;
    QPushButton* dnButton;
    QPushButton* fButton;
    QPushButton* lButton;
    QPushButton* isEmptyButton;
    QPushButton* lenButton;

    // 특정 타겟 노드 클릭 제어를 위한 대기 상태 정의
    enum State { IDLE, WAITING_INSERT_CLICK, WAITING_DELETE_CLICK };
    State currentState;

    // 생성자 및 소멸자
    DOUBLY_LINKED(QMainWindow* _parent);
    ~DOUBLY_LINKED();
    QWidget* setting();

    // UI 및 레이아웃을 노드 개수에 맞게 동적으로 다시 그리는 핵심 함수
    void update();

    // 왼쪽의 특정 노드 블록을 클릭했을 때 비즈니스 로직을 가로채 분기 처리하는 내부 핸들러
    void handleBlockClick(Double_Node* targetNode);

    // ==========================================
    // 3. 8가지 핵심 기능 및 복잡도 매칭 함수 명세
    // ==========================================
    void insert_first();    // O(1)
    void insert_last();     // O(1)
    void insert_between();  // O(1) - 대기 상태 전환 후 클릭 트리거
    void delete_first();    // O(1)
    void delete_last();     // O(1)
    void delete_node();     // O(1) - 대기 상태 전환 후 클릭 트리거
    void first();           // O(1)
    void last();            // O(1)
    void is_empty();        // O(1)
    void _len();            // O(1)

protected:
    // 각 QLabel(노드 블록)의 마우스 누름 이벤트를 안전하게 가로채기 위한 Qt 이벤트 필터
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // DOUBLY_LINKED_H