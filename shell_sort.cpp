#include "shell_sort.h"
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

SHELL_SORT::SHELL_SORT(QMainWindow* _parent)
    : QObject(_parent), parent(_parent), gapIndex(0), currentK(1), currentI(0), currentJ(0), tempValue(0),
    isSortingComplete(false), isShiftStep(false), isInsertionDone(false), currentSize(15) {

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &SHELL_SORT::handleStep);
}

SHELL_SORT::~SHELL_SORT() {
    animationTimer->stop();
}

QWidget* SHELL_SORT::setting() {
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet("QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }");

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION (자동 스케일링 캔버스)
    // ==========================================
    QWidget *leftWidget = new QWidget(containerWidget);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nDynamic Auto-Scaling Shell Sort Model", leftWidget);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 8px;");
    leftLayout->addWidget(visTitle);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, leftWidget);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QBrush(QColor("#11141A")));

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setStyleSheet("QGraphicsView { border: 1px solid #21262D; border-radius: 12px; background-color: #11141A; }");
    leftLayout->addWidget(view);

    // ==========================================
    // [Right Area] CONTROLS (Dashboard)
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

    // --- Card 1: Array Size Setup ---
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

    // --- Card 2: Animation Speed Setup ---
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

    connect(generateButton, &QPushButton::clicked, this, &SHELL_SORT::triggerGenerate);
    connect(playButton, &QPushButton::clicked, this, &SHELL_SORT::togglePlay);
    connect(speedSlider, &QSlider::valueChanged, this, &SHELL_SORT::updateSpeed);

    generateRandomData();
    updateVisuals();

    return containerWidget;
}

void SHELL_SORT::calculateGaps() {
    gapSequence.clear();
    int n = arrayData.size();
    for (int d = n / 2; d > 0; d /= 2) {
        gapSequence.append(d);
    }
    if (gapSequence.isEmpty()) {
        gapSequence.append(1);
    }
}

void SHELL_SORT::generateRandomData() {
    currentSize = sizeInput->value();
    arrayData.clear();
    for (int i = 0; i < currentSize; ++i) {
        arrayData.append(QRandomGenerator::global()->bounded(10, 101));
    }
    calculateGaps();
    resetSortState();
}

void SHELL_SORT::resetSortState() {
    animationTimer->stop();
    gapIndex = 0;
    isSortingComplete = false;
    isShiftStep = false;
    isInsertionDone = false;

    if (!gapSequence.isEmpty()) {
        currentK = gapSequence[gapIndex];
        currentI = currentK;
        currentJ = currentI;
        if (currentI < arrayData.size()) {
            tempValue = arrayData[currentI];
        }
    }
}

void SHELL_SORT::triggerGenerate() {
    generateRandomData();
    QStringList gapStrList;
    for(int g : gapSequence) gapStrList << QString::number(g);
    statusOutput->setText(QString(">> Generated %1 elements.\n>> Defined Gap Sequence: [ %2 ]\n>> Ready for Shell Sort.")
                              .arg(currentSize).arg(gapStrList.join(", ")));
    updateVisuals();
}

void SHELL_SORT::togglePlay() {
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

void SHELL_SORT::updateSpeed(int value) {
    speedLabel->setText(QString("%1 ms").arg(value));
    if (animationTimer->isActive()) {
        animationTimer->start(value);
    }
}

void SHELL_SORT::handleStep() {
    if (isSortingComplete) {
        animationTimer->stop();
        return;
    }

    int n = arrayData.size();

    if (gapIndex < gapSequence.size()) {
        currentK = gapSequence[gapIndex];

        if (currentI < n) {
            if (!isInsertionDone) {
                if (currentJ >= currentK && arrayData[currentJ - currentK] > tempValue) {
                    if (!isShiftStep) {
                        statusOutput->setText(QString(">> [Comparing / K=%1]\n>> Target Temp [ %2 ] vs Index %3 [ %4 ]\n>> Condition met, preparing Shift.")
                                                  .arg(currentK).arg(tempValue).arg(currentJ - currentK).arg(arrayData[currentJ - currentK]));
                        isShiftStep = true;
                    } else {
                        arrayData[currentJ] = arrayData[currentJ - currentK];
                        statusOutput->setText(QString(">> [Shifted] Index %1 value copied to Index %2.")
                                                  .arg(currentJ - currentK).arg(currentJ));
                        currentJ -= currentK;
                        isShiftStep = false;
                    }
                } else {
                    arrayData[currentJ] = tempValue;
                    statusOutput->setText(QString(">> [Inserted] Temp Value [ %1 ] safely placed at Index %2.")
                                              .arg(tempValue).arg(currentJ));
                    isInsertionDone = true;
                }
            } else {
                currentI++;
                if (currentI < n) {
                    currentJ = currentI;
                    tempValue = arrayData[currentI];
                }
                isInsertionDone = false;
                isShiftStep = false;
            }
        } else {
            statusOutput->setText(QString(">> [Pass Complete] Elements sorted partially under Gap K = %1.").arg(currentK));
            gapIndex++;
            if (gapIndex < gapSequence.size()) {
                currentK = gapSequence[gapIndex];
                currentI = currentK;
                currentJ = currentI;
                tempValue = arrayData[currentI];
            }
        }
    } else {
        isSortingComplete = true;
        animationTimer->stop();
        statusOutput->setText(QString(">> Success! Shell Sort finalized. The dynamic %1-node array is completely sorted.").arg(currentSize));
    }

    updateVisuals();
}

// =========================================================================
// [핵심 변경구간] 쉘 정렬의 K-간격(묶음별) 경계선 시각화 메커니즘 최적화
// =========================================================================
void SHELL_SORT::updateVisuals() {
    scene->clear();
    if (arrayData.isEmpty()) return;

    int n = arrayData.size();

    // 1. 뷰포트 가용 높이 기반 실시간 박스 스케일링 (스크롤바 완벽 억제)
    double viewHeight = qMax(static_cast<double>(view->height() - 40), 500.0);
    double totalUnits = (4.0 * n) + (1.0 * (n - 1));
    double unitLength = viewHeight / totalUnits;

    double dynamicBoxHeight = qMax(unitLength * 4.0, 2.0);
    double dynamicSpacing   = qMax(unitLength * 1.0, 0.5);

    double currentY = 10.0;

    for (int idx = 0; idx < n; ++idx) {
        int val = arrayData[idx];
        double dynamicWidth = 45.0 + (val * 2.4);

        // 기본 상태: 정렬 대기 노드는 골드 테두리(#D29922) 유지
        QPen pen(QColor("#D29922"), 1);
        QBrush brush(QColor("#161B22"));

        if (isSortingComplete) {
            brush = QBrush(QColor("#238636"));
            pen = QPen(QColor("#3FB950"), 1);
        } else {
            // 현재 타겟 인덱스 (I)와 동일한 묶음(Subgroup)에 속한 인덱스인지 판별 (mod K 연산)
            // 즉, (idx % K) == (currentI % K) 이면 동일 묶음(그룹)에 속함
            bool isSameSubgroup = (!isSortingComplete && (idx % currentK == currentI % currentK));

            if (isSameSubgroup) {
                if (idx == currentI && !isInsertionDone) {
                    brush = QBrush(QColor("#D29922")); // 삽입 타겟 (Yellow)
                    pen = QPen(QColor("#F2B832"), 1);
                }
                else if (idx == (currentJ - currentK) && currentJ >= currentK && !isInsertionDone) {
                    brush = QBrush(QColor("#0D47A1")); // 비교/시프트 대상 (Blue)
                    pen = QPen(QColor("#58A6FF"), 1);
                }
                else {
                    // 동일 묶음에 포함되나 현재 연산엔 쉬고 있는 노드 (흐린 군청색 톤)
                    brush = QBrush(QColor("#1F2937"));
                    pen = QPen(QColor("#58A6FF"), 1, Qt::DashLine);
                }
            }
            // 다른 묶음에 속한 일반 대기 노드는 기본 골드 테두리 적용
        }

        // 수평 데이터 막대 그리기
        QGraphicsPathItem *boxItem = new QGraphicsPathItem();
        QPainterPath path;
        double cornerRadius = qMin(5.0, dynamicBoxHeight / 3.0);
        path.addRoundedRect(BASE_X, currentY, dynamicWidth, dynamicBoxHeight, cornerRadius, cornerRadius);
        boxItem->setPath(path);
        boxItem->setPen(pen);
        boxItem->setBrush(brush);
        scene->addItem(boxItem);

        // [누적식 묶음별 경계선 메커니즘]
        if (idx < n - 1) {
            double dividerY = currentY + dynamicBoxHeight + (dynamicSpacing / 2.0);
            bool drawSolidBoundary = false;

            if (!isSortingComplete) {
                // 원소 사이의 인덱스에서 묶음(Subgroup)의 순환 주기가 끝나는 경계선 검출
                // (idx % currentK)가 (currentK - 1)인 지점은 해당 K묶음 그룹의 논리적 단락 경계선이 됩니다.
                if ((idx % currentK) == (currentK - 1)) {
                    drawSolidBoundary = true;
                }
            }

            // 노드가 너무 촘촘해 겹치는 노이즈를 방지하기 위한 마진 확보 검사
            if (drawSolidBoundary && dynamicSpacing > 2.0) {
                // 현재 연산 중인 묶음 바로 아래 경계선은 밝은 블루(#58A6FF)로 강조, 일반 묶음 분리선은 회색(#30363D) 처리
                QColor boundaryColor = ((idx % currentK) == (currentI % currentK)) ? QColor("#58A6FF") : QColor("#30363D");
                scene->addLine(BASE_X - 15, dividerY, BASE_X + 340, dividerY, QPen(boundaryColor, 1.2, Qt::SolidLine));
            } else if (dynamicSpacing > 3.0) {
                // 묶음 내부 원소 사이는 촘촘한 도트 가이드 링크선 유지
                scene->addLine(BASE_X, currentY + dynamicBoxHeight, BASE_X, currentY + dynamicBoxHeight + dynamicSpacing, QPen(QColor("#21262D"), 1, Qt::DotLine));
            }
        }

        currentY += dynamicBoxHeight + dynamicSpacing;
    }

    // 뷰포트 영역 강제 고정 및 인셋 마진 보정
    scene->setSceneRect(BASE_X - 20, 0, 400, currentY + 10);
}