#ifndef BUBBLE_SORT_H
#define BUBBLE_SORT_H

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

class BUBBLE_SORT : public QObject {
    Q_OBJECT

public:
    explicit BUBBLE_SORT(QMainWindow* _parent = nullptr);
    ~BUBBLE_SORT();

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
    QSpinBox* sizeInput;
    QSlider* speedSlider;
    QLabel* speedLabel;
    QLabel* statusOutput;

    // 버블 정렬 알고리즘 상태 변수
    QVector<int> arrayData;
    int currentI;
    int currentJ;
    bool isSortingComplete;
    bool isSwapStep;
    int currentSize;
};

#endif // BUBBLE_SORT_H