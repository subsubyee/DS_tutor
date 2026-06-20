#ifndef SHELL_SORT_H
#define SHELL_SORT_H

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

class SHELL_SORT : public QObject {
    Q_OBJECT

public:
    explicit SHELL_SORT(QMainWindow* _parent = nullptr);
    ~SHELL_SORT();

    QWidget* setting();

private slots:
    void triggerGenerate();
    void togglePlay();
    void handleStep();
    void updateSpeed(int value);

private:
    void generateRandomData();
    void calculateGaps();
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

    // 쉘 정렬 상태 변수
    QVector<int> arrayData;
    QVector<int> gapSequence;
    int gapIndex;
    int currentK;
    int currentI;
    int currentJ;
    int tempValue;

    bool isSortingComplete;
    bool isShiftStep;
    bool isInsertionDone;
    int currentSize;
};

#endif // SHELL_SORT_H