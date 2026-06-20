#ifndef RADIX_SORT_H
#define RADIX_SORT_H

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

enum RadixSubStep {
    DISTRIBUTE_TO_BUCKETS,
    COLLECT_FROM_BUCKETS,
    CHECK_NEXT_DIGIT
};

class RADIX_SORT : public QObject {
    Q_OBJECT

public:
    explicit RADIX_SORT(QMainWindow* _parent = nullptr);
    ~RADIX_SORT();

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

    // 기수 정렬 알고리즘 상태 변수
    QVector<int> arrayData;
    QVector<QVector<int>> buckets;

    int currentDigit;
    int maxDigit;
    int currentIdx;
    int activeBucketIdx;
    bool isSortingComplete;
    int currentSize;             // 현재 동적 데이터 크기

    RadixSubStep currentSubStep;
};

#endif // RADIX_SORT_H