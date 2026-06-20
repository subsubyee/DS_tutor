#ifndef QUICK_SORT_H
#define QUICK_SORT_H

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QSlider>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QVector>
#include <QStack>
#include <QSet>

struct QuickTask {
    int low;
    int high;
};

enum QuickSubStep {
    INIT_NEXT_TASK,
    SCAN_LEFT,
    SCAN_RIGHT,
    SWAP_POINTERS,
    SWAP_PIVOT
};

class QUICK_SORT : public QObject {
    Q_OBJECT

public:
    explicit QUICK_SORT(QMainWindow* _parent = nullptr);
    ~QUICK_SORT();

    QWidget* setting();

private slots:
    void triggerGenerate();
    void togglePlay();
    void handleStep();
    void updateSpeed(int value);

private:
    void generateRandomData();
    void resetSortState();
    void updateVisuals();

    QMainWindow* parent;
    QGraphicsScene* scene;
    QGraphicsView* view;
    QTimer* animationTimer;

    // UI 제어 컴포넌트
    QPushButton* generateButton;
    QPushButton* playButton;
    QSpinBox* sizeInput;         // 데이터 개수 입력란
    QSlider* speedSlider;        // 속도 조절 슬라이더
    QLabel* speedLabel;          // 현재 속도 표시 라벨
    QLabel* statusOutput;        // 시스템 라이브 로그 터미널

    // 퀵 정렬 알고리즘 상태 변수
    QVector<int> arrayData;
    QStack<QuickTask> taskStack;
    QSet<int> finalizedPivots;

    int currentLow;
    int currentHigh;
    int pivotIndex;
    int pivotValue;
    int leftPtr;
    int rightPtr;

    QuickSubStep currentSubStep;
    bool isSortingComplete;
    int currentSize;             // 현재 동적 배열 크기
};

#endif // QUICK_SORT_H