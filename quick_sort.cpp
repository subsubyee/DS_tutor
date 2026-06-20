#include "quick_sort.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QRandomGenerator>
#include <QGroupBox>

// ==========================================
// 시각화 전용 기하학 사양 기준점
// ==========================================
static const double BASE_X = -160.0;     // 좌측 기준선 시작점

QUICK_SORT::QUICK_SORT(QMainWindow* _parent)
    : QObject(_parent), parent(_parent), currentLow(0), currentHigh(0),
    pivotIndex(0), pivotValue(0), leftPtr(0), rightPtr(0),
    currentSubStep(INIT_NEXT_TASK), isSortingComplete(false), currentSize(15) {

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &QUICK_SORT::handleStep);
}

QUICK_SORT::~QUICK_SORT() {
    animationTimer->stop();
}

QWidget* QUICK_SORT::setting() {
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet("QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }");

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION (스크롤 프리 자동 스케일링 캔버스)
    // ==========================================
    QWidget *leftWidget = new QWidget(containerWidget);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nDynamic Auto-Scaling Quick Sort Model", leftWidget);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 8px;");
    leftLayout->addWidget(visTitle);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, leftWidget);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QBrush(QColor("#11141A")));

    // 스크롤바 완전 비활성화 및 테두리 설정
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setStyleSheet("QGraphicsView { border: 1px solid #21262D; border-radius: 12px; background-color: #11141A; }");
    leftLayout->addWidget(view);

    // ==========================================
    // [Right Area] CONTROLS (Obsidian 테마 대시보드)
    // ==========================================
    QScrollArea *rightScrollArea = new QScrollArea(containerWidget);
    rightScrollArea->setWidgetResizable(true);
    rightScrollArea->setFixedWidth(450);
    rightScrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    QWidget *rightContentWidget = new QWidget();
    rightContentWidget->setObjectName("RightContent");
    rightContentWidget->setStyleSheet("QWidget#RightContent { background: transparent; }");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightContentWidget);
    rightLayout->setContentsMargins(0, 0, 8, 0);
    rightLayout->setSpacing(12);

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nMinimal Sorting Dashboard", rightContentWidget);
    controlsTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #8B949E; letter-spacing: 1px; margin-bottom: 4px;");
    rightLayout->addWidget(controlsTitle);

    // 스타일시트 셋업
    QString cardStyle   = "QWidget { background-color: #161B22; border: 1px solid #21262D; border-radius: 10px; }";
    QString titleStyle  = "color: #58A6FF; font-weight: bold; font-family: 'Consolas', monospace; font-size: 13px; background: transparent; border: none;";
    QString descStyle   = "color: #8B949E; font-size: 11px; background: transparent; border: none;";

    QString inputStyle  = "QSpinBox { background-color: #0D1117; border: 1px solid #30363D; border-radius: 6px; color: #E6EDF3; padding: 6px 10px; }"
                         "QSpinBox:focus { border: 1px solid #58A6FF; }"
                         "QSpinBox::up-button, QSpinBox::down-button { background: #161B22; border-left: 1px solid #30363D; width: 20px; }"
                         "QSpinBox::up-button:hover, QSpinBox::down-button:hover { background: #30363D; }";

    QString btnPrimary  = "QPushButton { background-color: #238636; color: #FFFFFF; border-radius: 6px; padding: 8px 16px; font-weight: bold; border: none; }"
                         "QPushButton:hover { background-color: #2EA043; }"
                         "QPushButton:pressed { background-color: #248039; }";

    QString btnAuto     = "QPushButton { background-color: #A371F7; color: #FFFFFF; border-radius: 6px; padding: 8px 16px; font-weight: bold; border: none; }"
                      "QPushButton:hover { background-color: #B48EFA; }"
                      "QPushButton:pressed { background-color: #8957E5; }";

    QString sliderStyle = "QSlider::groove:horizontal { height: 6px; background: #0D1117; border-radius: 3px; }"
                          "QSlider::sub-page:horizontal { background: #58A6FF; border-radius: 3px; }"
                          "QSlider::handle:horizontal { background: #E6EDF3; width: 14px; margin-top: -4px; margin-bottom: -4px; border-radius: 7px; }"
                          "QSlider::handle:horizontal:hover { background: #58A6FF; }";

    // --- Card 1: Array Size Setup (데이터 개수 입력란) ---
    QWidget *sizeCard = new QWidget(rightContentWidget); sizeCard->setAttribute(Qt::WA_StyledBackground, true); sizeCard->setStyleSheet(cardStyle);
    QVBoxLayout *sizeL = new QVBoxLayout(sizeCard); QHBoxLayout *sizeTop = new QHBoxLayout();
    QLabel *sTitle = new QLabel("Array Size", sizeCard); sTitle->setStyleSheet(titleStyle);
    QLabel *sDesc = new QLabel("Set dynamic nodes", sizeCard); sDesc->setStyleSheet(descStyle);
    sizeTop->addWidget(sTitle); sizeTop->addWidget(sDesc); sizeTop->addStretch();
    QHBoxLayout *sizeBot = new QHBoxLayout();
    sizeInput = new QSpinBox(sizeCard);
    sizeInput->setRange(1, 1000);
    sizeInput->setValue(15);
    sizeInput->setStyleSheet(inputStyle);
    generateButton = new QPushButton("Generate Data", sizeCard); generateButton->setStyleSheet(btnPrimary);
    sizeBot->addWidget(sizeInput, 1); sizeBot->addWidget(generateButton); sizeL->addLayout(sizeTop); sizeL->addLayout(sizeBot);
    rightLayout->addWidget(sizeCard);

    // --- Card 2: Animation Speed Setup (진행속도 스크롤 바) ---
    QWidget *speedCard = new QWidget(rightContentWidget); speedCard->setAttribute(Qt::WA_StyledBackground, true); speedCard->setStyleSheet(cardStyle);
    QVBoxLayout *speedL = new QVBoxLayout(speedCard); QHBoxLayout *speedTop = new QHBoxLayout();
    QLabel *spTitle = new QLabel("Interval Speed", speedCard); spTitle->setStyleSheet(titleStyle);
    QLabel *spDesc = new QLabel("Adjust refresh rate", speedCard); spDesc->setStyleSheet(descStyle);
    speedLabel = new QLabel("200 ms", speedCard); speedLabel->setStyleSheet("color: #56B6C2; font-size: 11px; font-weight: bold; font-family: monospace;");
    speedTop->addWidget(spTitle); speedTop->addWidget(spDesc); speedTop->addStretch(); speedTop->addWidget(speedLabel);
    QHBoxLayout *speedBot = new QHBoxLayout();
    speedSlider = new QSlider(Qt::Horizontal, speedCard); speedSlider->setRange(5, 1000); speedSlider->setValue(200); speedSlider->setStyleSheet(sliderStyle);
    speedBot->addWidget(speedSlider); speedL->addLayout(speedTop); speedL->addLayout(speedBot);
    rightLayout->addWidget(speedCard);

    // --- Card 3: Core Play Controls ---
    QWidget *playCard = new QWidget(rightContentWidget); playCard->setAttribute(Qt::WA_StyledBackground, true); playCard->setStyleSheet(cardStyle);
    QVBoxLayout *playL = new QVBoxLayout(playCard); QHBoxLayout *playTop = new QHBoxLayout();
    QLabel *plTitle = new QLabel("Engine Controller", playCard); plTitle->setStyleSheet(titleStyle);
    QLabel *plDesc = new QLabel("Run Sorting Algorithm", playCard); plDesc->setStyleSheet(descStyle);
    playTop->addWidget(plTitle); playTop->addWidget(plDesc); playTop->addStretch();
    QHBoxLayout *playBot = new QHBoxLayout();
    playButton = new QPushButton("Auto Play / Pause (Toggle)", playCard); playButton->setStyleSheet(btnAuto);
    playBot->addWidget(playButton, 1); playL->addLayout(playTop); playL->addLayout(playBot);
    rightLayout->addWidget(playCard);

    // --- Bottom: Operation Log Terminal ---
    QLabel *logTitle = new QLabel(">_ System Live Log", rightContentWidget);
    logTitle->setStyleSheet("color: #8B949E; font-size: 11px; font-family: monospace; font-weight: bold; margin-top: 6px;");
    rightLayout->addWidget(logTitle);

    statusOutput = new QLabel(">> System Ready. Click 'Generate Data' to load array.", rightContentWidget);
    statusOutput->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    statusOutput->setWordWrap(true);
    statusOutput->setStyleSheet(
        "background-color: #0D1117; border: 1px solid #21262D; border-radius: 8px; "
        "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 180px;"
        );
    rightLayout->addWidget(statusOutput);

    rightScrollArea->setWidget(rightContentWidget);

    mainLayout->addWidget(leftWidget, 1);
    mainLayout->addWidget(rightScrollArea);

    connect(generateButton, &QPushButton::clicked, this, &QUICK_SORT::triggerGenerate);
    connect(playButton, &QPushButton::clicked, this, &QUICK_SORT::togglePlay);
    connect(speedSlider, &QSlider::valueChanged, this, &QUICK_SORT::updateSpeed);

    generateRandomData();
    updateVisuals();

    return containerWidget;
}

void QUICK_SORT::generateRandomData() {
    currentSize = sizeInput->value();
    arrayData.clear();
    for (int i = 0; i < currentSize; ++i) {
        arrayData.append(QRandomGenerator::global()->bounded(10, 101));
    }
    resetSortState();
}

void QUICK_SORT::resetSortState() {
    animationTimer->stop();
    taskStack.clear();
    finalizedPivots.clear();
    isSortingComplete = false;
    currentSubStep = INIT_NEXT_TASK;

    if (!arrayData.isEmpty()) {
        taskStack.push({0, arrayData.size() - 1});
    }
}

void QUICK_SORT::triggerGenerate() {
    generateRandomData();
    statusOutput->setText(QString(">> Generated new unsorted dataset. Total %1 elements initialized.\n>> Ready for Quick Sort. Boundaries will accumulate as partitions split.").arg(currentSize));
    updateVisuals();
}

void QUICK_SORT::togglePlay() {
    if (isSortingComplete) {
        statusOutput->setText(">> Already fully sorted! Click 'Generate Data' to reset.");
        return;
    }
    if (animationTimer->isActive()) {
        animationTimer->stop();
        statusOutput->setText(statusOutput->text() + "\n>> Auto Playback Paused.");
    } else {
        animationTimer->start(speedSlider->value());
        statusOutput->setText(statusOutput->text() + "\n>> Auto Playback Active...");
    }
}

void QUICK_SORT::updateSpeed(int value) {
    speedLabel->setText(QString("%1 ms").arg(value));
    if (animationTimer->isActive()) {
        animationTimer->start(value);
    }
}

void QUICK_SORT::handleStep() {
    if (isSortingComplete) {
        animationTimer->stop();
        return;
    }

    switch (currentSubStep) {
    case INIT_NEXT_TASK: {
        if (taskStack.isEmpty()) {
            isSortingComplete = true;
            animationTimer->stop();
            statusOutput->setText(">> Success! All boundaries collapsed. The entire array is seamlessly unified and sorted.");
            updateVisuals();
            return;
        }

        QuickTask currentTask = taskStack.pop();
        currentLow = currentTask.low;
        currentHigh = currentTask.high;

        if (currentLow >= currentHigh) {
            if (currentLow == currentHigh) {
                finalizedPivots.insert(currentLow);
            }
            handleStep();
            return;
        }

        pivotIndex = currentHigh;
        pivotValue = arrayData[pivotIndex];
        leftPtr = currentLow;
        rightPtr = currentHigh - 1;

        statusOutput->setText(QString(">> [Active Partition] Sub-segment: Index [ %1 to %2 ]\n>> Pivot Selected at Index %3 (Value: %4)")
                                  .arg(currentLow).arg(currentHigh).arg(pivotIndex).arg(pivotValue));

        currentSubStep = SCAN_LEFT;
        break;
    }

    case SCAN_LEFT: {
        if (leftPtr <= rightPtr && arrayData[leftPtr] < pivotValue) {
            statusOutput->setText(QString(">> [Scanning l] Index %1 (Value: %2) < Pivot (%3).")
                                      .arg(leftPtr).arg(arrayData[leftPtr]).arg(pivotValue));
            leftPtr++;
        } else {
            statusOutput->setText(QString(">> [l Pointer Blocked] Found value >= Pivot at Index %1 (Value: %2)")
                                      .arg(leftPtr).arg(arrayData[leftPtr]));
            currentSubStep = SCAN_RIGHT;
        }
        break;
    }

    case SCAN_RIGHT: {
        if (rightPtr >= leftPtr && arrayData[rightPtr] > pivotValue) {
            statusOutput->setText(QString(">> [Scanning r] Index %1 (Value: %2) > Pivot (%3).")
                                      .arg(rightPtr).arg(arrayData[rightPtr]).arg(pivotValue));
            rightPtr--;
        } else {
            statusOutput->setText(QString(">> [r Pointer Blocked] Found value <= Pivot at Index %1 (Value: %2)")
                                      .arg(rightPtr).arg(arrayData[rightPtr]));

            if (leftPtr < rightPtr) {
                currentSubStep = SWAP_POINTERS;
            } else {
                currentSubStep = SWAP_PIVOT;
            }
        }
        break;
    }

    case SWAP_POINTERS: {
        int temp = arrayData[leftPtr];
        arrayData[leftPtr] = arrayData[rightPtr];
        arrayData[rightPtr] = temp;

        statusOutput->setText(QString(">> [Swapping Pointers] Swapped Index %1 ↔ Index %2")
                                  .arg(leftPtr).arg(rightPtr));

        leftPtr++;
        rightPtr--;
        currentSubStep = SCAN_LEFT;
        break;
    }

    case SWAP_PIVOT: {
        if (leftPtr != pivotIndex) {
            int temp = arrayData[leftPtr];
            arrayData[leftPtr] = arrayData[pivotIndex];
            arrayData[pivotIndex] = temp;
            statusOutput->setText(QString(">> [Pivot Swap] Repositioned Pivot to Center: Index %1 ↔ Index %2")
                                      .arg(leftPtr).arg(pivotIndex));
        } else {
            statusOutput->setText(QString(">> [Pivot in Place] Pivot naturally settled at Index %1.").arg(leftPtr));
        }

        int actualPivotPos = leftPtr;
        finalizedPivots.insert(actualPivotPos);

        taskStack.push({actualPivotPos + 1, currentHigh});
        taskStack.push({currentLow, actualPivotPos - 1});

        currentSubStep = INIT_NEXT_TASK;
        break;
    }
    }

    updateVisuals();
}

// 명세 반영: 노드 개수에 비례하여 세로 폭(가변 높이/마진) 실시간 가역 연산 처리 (스크롤바 완전 억제 및 텍스트 제거)
void QUICK_SORT::updateVisuals() {
    scene->clear();
    if (arrayData.isEmpty()) return;

    int n = arrayData.size();

    // 1. 현재 뷰포트 가용 세로 높이 기반 분할 비율 계산 (최소 높이 500 고정선 확보)
    double viewHeight = qMax(static_cast<double>(view->height() - 40), 500.0);
    double totalUnits = (4.0 * n) + (1.0 * (n - 1));
    double unitLength = viewHeight / totalUnits;

    double dynamicBoxHeight = qMax(unitLength * 4.0, 2.0); // 최소 높이 2px
    double dynamicSpacing   = qMax(unitLength * 1.0, 0.5); // 최소 마진 0.5px

    double currentY = 10.0;
    bool hasActiveTask = !isSortingComplete && (currentSubStep != INIT_NEXT_TASK) && (currentLow <= currentHigh);

    for (int idx = 0; idx < n; ++idx) {
        int val = arrayData[idx];
        double dynamicWidth = 45.0 + (val * 2.4);

        // 기본 스타일 (금색 테두리)
        QPen pen(QColor("#D29922"), 1);
        QBrush brush(QColor("#161B22"));

        if (isSortingComplete) {
            brush = QBrush(QColor("#238636"));
            pen = QPen(QColor("#3FB950"), 1);
        } else {
            if (hasActiveTask && idx >= currentLow && idx <= currentHigh) {
                if (idx == pivotIndex) {
                    brush = QBrush(QColor("#D29922"));
                    pen = QPen(QColor("#F2B832"), 1);
                }
                else if (idx == leftPtr || idx == rightPtr) {
                    brush = QBrush(QColor("#0D47A1"));
                    pen = QPen(QColor("#58A6FF"), 1);
                }
                else {
                    brush = QBrush(QColor("#1F2937"));
                    pen = QPen(QColor("#D29922"), 1, Qt::DashLine);
                }
            }
            else if (finalizedPivots.contains(idx)) {
                brush = QBrush(QColor("#0E2A1B"));
                pen = QPen(QColor("#2ea043"), 1, Qt::SolidLine);
            }
        }

        // 수평 막대 데이터 블록 드로잉 (숫자 텍스트 제거 완료)
        QGraphicsPathItem *boxItem = new QGraphicsPathItem();
        QPainterPath path;
        double cornerRadius = qMin(5.0, dynamicBoxHeight / 3.0);
        path.addRoundedRect(BASE_X, currentY, dynamicWidth, dynamicBoxHeight, cornerRadius, cornerRadius);
        boxItem->setPath(path);
        boxItem->setPen(pen);
        boxItem->setBrush(brush);
        scene->addItem(boxItem);

        // [누적식 경계선 메커니즘] 원소 사이의 선 분기 렌더링
        if (idx < n - 1) {
            double dividerY = currentY + dynamicBoxHeight + (dynamicSpacing / 2.0);
            bool drawSolidBoundary = false;

            if (!isSortingComplete) {
                if (finalizedPivots.contains(idx) || finalizedPivots.contains(idx + 1)) {
                    drawSolidBoundary = true;
                }
                if (hasActiveTask) {
                    if ((idx + 1) == currentLow || (idx + 1) == (currentHigh + 1)) {
                        drawSolidBoundary = true;
                    }
                }
            }

            // 노드가 너무 많아 촘촘할 때 경계선이 겹쳐 보이는 노이즈 방지 조건 추가
            if (drawSolidBoundary && dynamicSpacing > 2.0) {
                QColor boundaryColor = (hasActiveTask && ((idx + 1) == currentLow || (idx + 1) == (currentHigh + 1)))
                ? QColor("#58A6FF") : QColor("#30363D");
                scene->addLine(BASE_X - 15, dividerY, BASE_X + 340, dividerY, QPen(boundaryColor, 1.2, Qt::SolidLine));
            } else if (dynamicSpacing > 3.0) {
                scene->addLine(BASE_X, currentY + dynamicBoxHeight, BASE_X, currentY + dynamicBoxHeight + dynamicSpacing, QPen(QColor("#21262D"), 1, Qt::DotLine));
            }
        }

        currentY += dynamicBoxHeight + dynamicSpacing;
    }

    // 상하좌우 안전 여백 인셋 보정 및 화면 뷰포트 강제 고정
    scene->setSceneRect(BASE_X - 20, 0, 400, currentY + 10);
}