#ifndef MERGE_SORT_H
#define MERGE_SORT_H

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

class MERGE_SORT : public QObject {
    Q_OBJECT

public:
    explicit MERGE_SORT(QMainWindow* _parent = nullptr);
    ~MERGE_SORT();

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

    // UI 제어 컴포넌트 (데이터 개수 및 속도 조절 추가)
    QPushButton* generateButton;
    QPushButton* playButton;
    QSpinBox* sizeInput;
    QSlider* speedSlider;
    QLabel* speedLabel;
    QLabel* statusOutput;

    // 병합 정렬 상태 변수
    QVector<int> arrayData;
    QVector<int> tempArray;
    int currentSubArraySize;
    int leftStart;
    int mergeI;
    int mergeJ;
    int mergeK;

    bool isSortingComplete;
    bool isMergingState;
    int currentSize;
};

#endif // MERGE_SORT_H