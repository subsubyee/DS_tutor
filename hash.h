#ifndef HASH_H
#define HASH_H

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QVector>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class HASH : public QObject {
    Q_OBJECT

public:
    // 생성자 및 소멸자
    explicit HASH(QMainWindow* _parent = nullptr);
    ~HASH();

    // UI 레이아웃 생성 및 초기화 함수
    QWidget* setting();

private:
    // 수학적 조건 검증 및 크기 자동 결정 (PPT 알고리즘)
    bool is_prime(int n);
    bool is_4kplus3(int n);
    int calculate_optimal_size(int target_nmax);

    // 내부 상태 업데이트 및 데이터 시각화 제어
    void update();

private slots:
    // 코어 비즈니스 로직 슬롯 함수군
    void setupHashSize();  // STEP 1: 크기 정의 및 테이블 초기화
    void insertValue();    // STEP 2: 이중 해시 기반 원소 삽입
    void deleteValue();    // STEP 3: 소프트 딜리트(더미화) 원소 삭제

private:
    // 기본 제어 변수
    QMainWindow* parent;        // 부모 윈도우 포인터
    int nmax;                   // 목표 입력 개수 (Target Capacity)
    int max;                    // 해시 테이블의 실제 최적 크기 (M)
    int current_len;            // 현재 테이블에 적재된 유효 원소 개수
    QVector<QString> hashTable; // 실제 해시 데이터 저장 컨테이너 ("None", "Deleted", 또는 Key 값)

    // UI 컴포넌트 객체 포인터 (STACK 구조와 일치화)
    QScrollArea* scrollArea;    // 좌측 시각화 스크롤 영역
    QWidget* hashContainer;     // 슬롯들을 감싸는 컨테이너 위젯
    QGridLayout* hashGrid;      // 4열 배열 배치를 위한 그리드 레이아웃
    QLabel** hashCells;         // 동적 할당할 해시 슬롯 라벨 배열 포인터

    // 우측 대시보드 컨트롤 인터페이스 컴포넌트
    QLineEdit* sizeInput;       // 용량 설정 입력창
    QLineEdit* valueInput;      // 삽입 키 입력창
    QLineEdit* deleteInput;     // 삭제 키 입력창

    QPushButton* setSizeButton; // 초기화 실행 버튼
    QPushButton* insertButton;  // 삽입 실행 버튼
    QPushButton* deleteButton;  // 삭제 실행 버튼

    QLabel* statusOutput;       // 하단 통합 라이브 로그 콘솔 터미널 라벨
};

#endif // HASH_H