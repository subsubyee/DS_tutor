#ifndef INSERTION_SORT_H
#define INSERTION_SORT_H

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

class INSERTION_SORT : public QObject {
    Q_OBJECT

public:
    explicit INSERTION_SORT(QMainWindow* _parent = nullptr);
    ~INSERTION_SORT();

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

    // 삽입 정렬 알고리즘 상태 변수
    QVector<int> arrayData;
    int currentI;
    int currentJ;
    int currentKey;
    bool isSortingComplete;
    bool isComparingState;
    int currentSize;             // 현재 동적 배열 크기
};

#endif // INSERTION_SORT_H