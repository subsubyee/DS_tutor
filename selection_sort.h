#ifndef SELECTION_SORT_H
#define SELECTION_SORT_H

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

class SELECTION_SORT : public QObject {
    Q_OBJECT

public:
    explicit SELECTION_SORT(QMainWindow* _parent = nullptr);
    ~SELECTION_SORT();

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
    QSpinBox* sizeInput;         // 데이터 개수 입력 바운드
    QSlider* speedSlider;        // 속도 조절 인터벌 슬라이더
    QLabel* speedLabel;          // 속도 레이블 수치 출력
    QLabel* statusOutput;        // 시스템 실시간 터미널 로그

    // 선택 정렬 알고리즘 상태 변수
    QVector<int> arrayData;
    int currentI;
    int currentJ;
    int minIndex;
    bool isSortingComplete;
    bool isSwapStep;
    int currentSize;             // 가변 제어 데이터 크기
};

#endif // SELECTION_SORT_H