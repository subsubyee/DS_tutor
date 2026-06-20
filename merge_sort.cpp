#include "merge_sort.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QRandomGenerator>
#include <QGroupBox>

// ==========================================
// 시각화 전용 기하학 사양 기준점
// ==========================================
static const double BASE_X = -160.0;     // 좌측 기준선 시작점

MERGE_SORT::MERGE_SORT(QMainWindow* _parent)
    : QObject(_parent), parent(_parent), currentSubArraySize(1), leftStart(0),
    mergeI(0), mergeJ(0), mergeK(0), isSortingComplete(false), isMergingState(false), currentSize(15) {

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &MERGE_SORT::handleStep);
}

MERGE_SORT::~MERGE_SORT() {
    animationTimer->stop();
}

QWidget* MERGE_SORT::setting() {
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

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nBoundary-Dissolving Merge Sort Model", leftWidget);
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

    // --- Card 1: Array Size Setup (데이터 개수 입력란 추가) ---
    QWidget *sizeCard = new QWidget(rightContentWidget); sizeCard->setAttribute(Qt::WA_StyledBackground, true); sizeCard->setStyleSheet(cardStyle);
    QVBoxLayout *sizeL = new QVBoxLayout(sizeCard); QHBoxLayout *sizeTop = new QHBoxLayout();
    QLabel *sTitle = new QLabel("Array Size", sizeCard); sTitle->setStyleSheet(titleStyle);
    QLabel *sDesc = new QLabel("Set dynamic nodes", sizeCard); sDesc->setStyleSheet(descStyle);
    sizeTop->addWidget(sTitle); sizeTop->addWidget(sDesc); sizeTop->addStretch();
    QHBoxLayout *sizeBot = new QHBoxLayout();
    sizeInput = new QSpinBox(sizeCard);
    sizeInput->setRange(1, 1000); // 병합 정렬 가독성을 위한 최적화 범위선
    sizeInput->setValue(12);
    sizeInput->setStyleSheet(inputStyle);
    generateButton = new QPushButton("Generate Data", sizeCard); generateButton->setStyleSheet(btnPrimary);
    sizeBot->addWidget(sizeInput, 1); sizeBot->addWidget(generateButton); sizeL->addLayout(sizeTop); sizeL->addLayout(sizeBot);
    rightLayout->addWidget(sizeCard);

    // --- Card 2: Animation Speed Setup (진행속도 스크롤 바 추가) ---
    QWidget *speedCard = new QWidget(rightContentWidget); speedCard->setAttribute(Qt::WA_StyledBackground, true); speedCard->setStyleSheet(cardStyle);
    QVBoxLayout *speedL = new QVBoxLayout(speedCard); QHBoxLayout *speedTop = new QHBoxLayout();
    QLabel *spTitle = new QLabel("Interval Speed", speedCard); spTitle->setStyleSheet(titleStyle);
    QLabel *spDesc = new QLabel("Adjust refresh rate", speedCard); spDesc->setStyleSheet(descStyle);
    speedLabel = new QLabel("450 ms", speedCard); speedLabel->setStyleSheet("color: #56B6C2; font-size: 11px; font-weight: bold; font-family: monospace;");
    speedTop->addWidget(spTitle); speedTop->addWidget(spDesc); speedTop->addStretch(); speedTop->addWidget(speedLabel);
    QHBoxLayout *speedBot = new QHBoxLayout();
    speedSlider = new QSlider(Qt::Horizontal, speedCard); speedSlider->setRange(10, 1500); speedSlider->setValue(450); speedSlider->setStyleSheet(sliderStyle);
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

    connect(generateButton, &QPushButton::clicked, this, &MERGE_SORT::triggerGenerate);
    connect(playButton, &QPushButton::clicked, this, &MERGE_SORT::togglePlay);
    connect(speedSlider, &QSlider::valueChanged, this, &MERGE_SORT::updateSpeed);

    generateRandomData();
    updateVisuals();

    return containerWidget;
}

void MERGE_SORT::generateRandomData() {
    currentSize = sizeInput->value();
    arrayData.clear();
    for (int i = 0; i < currentSize; ++i) {
        arrayData.append(QRandomGenerator::global()->bounded(10, 101));
    }
    resetSortState();
}

void MERGE_SORT::resetSortState() {
    animationTimer->stop();
    currentSubArraySize = 1;
    leftStart = 0;
    isSortingComplete = false;
    isMergingState = false;
    tempArray = arrayData;
}

void MERGE_SORT::triggerGenerate() {
    generateRandomData();
    statusOutput->setText(QString(">> Generated new unsorted dataset. Total %1 elements initialized.\n>> Initial boundaries drawn for single elements.").arg(currentSize));
    updateVisuals();
}

void MERGE_SORT::togglePlay() {
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

void MERGE_SORT::updateSpeed(int value) {
    speedLabel->setText(QString("%1 ms").arg(value));
    if (animationTimer->isActive()) {
        animationTimer->start(value);
    }
}

void MERGE_SORT::handleStep() {
    if (isSortingComplete) {
        animationTimer->stop();
        return;
    }

    int n = arrayData.size();

    if (!isMergingState) {
        if (currentSubArraySize < n) {
            if (leftStart < n) {
                int mid = qMin(leftStart + currentSubArraySize, n);
                int rightEnd = qMin(leftStart + 2 * currentSubArraySize, n);

                if (mid < rightEnd) {
                    mergeI = leftStart;
                    mergeJ = mid;
                    mergeK = leftStart;
                    isMergingState = true;

                    statusOutput->setText(QString(">> [Dissolving Boundary] Subarray Size: %1\n>> Merging Section: Index [ %2 ~ %3 ] with [ %4 ~ %5 ]")
                                              .arg(currentSubArraySize).arg(leftStart).arg(mid - 1).arg(mid).arg(rightEnd - 1));
                } else {
                    leftStart += 2 * currentSubArraySize;
                }
            } else {
                currentSubArraySize *= 2;
                leftStart = 0;
            }
        } else {
            isSortingComplete = true;
            animationTimer->stop();
            statusOutput->setText(QString(">> Success! All boundaries collapsed. The dynamic %1-node array is fully unified.").arg(currentSize));
        }
    }

    if (isMergingState) {
        int mid = qMin(leftStart + currentSubArraySize, n);
        int rightEnd = qMin(leftStart + 2 * currentSubArraySize, n);

        if (mergeI < mid && mergeJ < rightEnd) {
            if (arrayData[mergeI] <= arrayData[mergeJ]) {
                tempArray[mergeK] = arrayData[mergeI];
                statusOutput->setText(QString(">> [S1 picked] Value %1 moved to Temp position %2.")
                                          .arg(arrayData[mergeI]).arg(mergeK));
                mergeI++;
            } else {
                tempArray[mergeK] = arrayData[mergeJ];
                statusOutput->setText(QString(">> [S2 picked] Value %1 moved to Temp position %2.")
                                          .arg(arrayData[mergeJ]).arg(mergeK));
                mergeJ++;
            }
            mergeK++;
        }
        else if (mergeJ < rightEnd) {
            tempArray[mergeK] = arrayData[mergeJ];
            mergeJ++;
            mergeK++;
        }
        else if (mergeI < mid) {
            tempArray[mergeK] = arrayData[mergeI];
            mergeI++;
            mergeK++;
        }

        if (mergeK >= rightEnd) {
            for (int i = leftStart; i < rightEnd; i++) {
                arrayData[i] = tempArray[i];
            }
            leftStart += 2 * currentSubArraySize;
            isMergingState = false;
        }
    }

    updateVisuals();
}

// =========================================================================
// [자동 스케일링 대응 변경] 동적 노드 수 비례 가변 높이 및 여백 연산 처리
// =========================================================================
void MERGE_SORT::updateVisuals() {
    scene->clear();
    if (arrayData.isEmpty()) return;

    int n = arrayData.size();

    // 1. 현재 뷰포트 크기에 맞춰 원소 간 격자와 높이를 가역 연산 (스크롤바 완벽 제어)
    double viewHeight = qMax(static_cast<double>(view->height() - 40), 500.0);
    double totalUnits = (4.0 * n) + (1.0 * (n - 1));
    double unitLength = viewHeight / totalUnits;

    double dynamicBoxHeight = qMax(unitLength * 4.0, 2.0); // 가변 노드 세로 높이
    double dynamicSpacing   = qMax(unitLength * 1.0, 0.5); // 가변 노드 간 마진

    double currentY = 10.0;

    int currentRangeStart = leftStart;
    int currentRangeEnd = qMin(leftStart + 2 * currentSubArraySize, n);
    int currentMid = qMin(leftStart + currentSubArraySize, n);

    for (int idx = 0; idx < n; ++idx) {
        int val = arrayData[idx];
        double dynamicWidth = 45.0 + (val * 2.4);

        QPen pen(QColor("#D29922"), 1);
        QBrush brush(QColor("#161B22"));
        QColor textColor(QColor("#E6EDF3"));

        if (isSortingComplete) {
            brush = QBrush(QColor("#238636"));
            pen = QPen(QColor("#3FB950"), 1);
        } else if (isMergingState) {
            if ((idx == mergeI && mergeI < currentMid) || (idx == mergeJ && mergeJ < currentRangeEnd)) {
                brush = QBrush(QColor("#D29922"));
                pen = QPen(QColor("#F2B832"), 1);
                textColor = QColor("#0D1117");
            }
            else if (idx >= currentRangeStart && idx < currentRangeEnd) {
                brush = QBrush(QColor("#0D47A1"));
                pen = QPen(QColor("#58A6FF"), 1);
            }
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

        // 노드 내부 숫자 텍스트 출력 (폰트 크기도 노드 높이에 유동적 비례 처리)
        if (dynamicBoxHeight > 10.0) {
            QGraphicsTextItem *textItem = new QGraphicsTextItem(QString::number(val));
            textItem->setDefaultTextColor(textColor);
            QFont font = textItem->font();
            font.setBold(true);
            font.setPointSize(qMax(7, qMin(10, static_cast<int>(dynamicBoxHeight / 3.0))));
            textItem->setFont(font);

            double textH = textItem->boundingRect().height();
            textItem->setPos(BASE_X + 12, currentY + (dynamicBoxHeight - textH) / 2);
            scene->addItem(textItem);
        }

        // [경계선 허물기 연산부] 기존의 고유 논리선 분기를 가변 마진 기반으로 연계
        if (idx < n - 1) {
            int nextIdx = idx + 1;
            bool drawBoundary = false;

            if (!isSortingComplete) {
                if (isMergingState && nextIdx >= currentRangeStart && nextIdx <= currentRangeEnd) {
                    drawBoundary = false;
                } else {
                    int step = (nextIdx < currentRangeStart) ? (2 * currentSubArraySize) : currentSubArraySize;
                    if (nextIdx % step == 0) {
                        drawBoundary = true;
                    }
                }
            }

            double dividerY = currentY + dynamicBoxHeight + (dynamicSpacing / 2.0);

            if (drawBoundary && dynamicSpacing > 2.0) {
                QColor boundaryColor = (nextIdx >= currentRangeStart && nextIdx < currentRangeEnd) ? QColor("#58A6FF") : QColor("#30363D");
                scene->addLine(BASE_X - 15, dividerY, BASE_X + 340, dividerY, QPen(boundaryColor, 1.2, Qt::SolidLine));
            } else if (dynamicSpacing > 3.0) {
                scene->addLine(BASE_X, currentY + dynamicBoxHeight, BASE_X, currentY + dynamicBoxHeight + dynamicSpacing, QPen(QColor("#444C56"), 1, Qt::DotLine));
            }
        }

        currentY += dynamicBoxHeight + dynamicSpacing;
    }

    scene->setSceneRect(BASE_X - 20, 0, 400, currentY + 10);
}