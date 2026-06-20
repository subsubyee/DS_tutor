#include "insertion_sort.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QFont>
#include <QRandomGenerator>
#include <QGroupBox>

// ==========================================
// 시각화 전용 기하학 사양 기준점
// ==========================================
static const double BASE_X = -160.0;     // 좌측 기준선 시작점

// 완벽한 끼워넣기 알고리즘 전용 시각 상태 변수
static int visualCurrentI = -1;         // 현재 정렬 범위 기준점 포인터 (i)
static int visualCompareJ = -1;         // 정렬된 그룹 내에서 역순 비교 중인 노드 -> 파란색
static int visualInsertIdx = -1;        // 최종 '사이에 끼워넣는' 위치 -> 빨간색
static bool visualIsInsertPhase = false;// 현재 단계가 최종 삽입 순간인지 판별하는 플래그

INSERTION_SORT::INSERTION_SORT(QMainWindow* _parent)
    : QObject(_parent), parent(_parent), currentI(1), currentJ(0), currentKey(0),
    isSortingComplete(false), isComparingState(true), currentSize(15) {

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &INSERTION_SORT::handleStep);
}

INSERTION_SORT::~INSERTION_SORT() {
    animationTimer->stop();
}

QWidget* INSERTION_SORT::setting() {
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

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nDynamic Auto-Scaling Insertion Sort Model", leftWidget);
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

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nPartitioning Dashboard", rightContentWidget);
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

    connect(generateButton, &QPushButton::clicked, this, &INSERTION_SORT::triggerGenerate);
    connect(playButton, &QPushButton::clicked, this, &INSERTION_SORT::togglePlay);
    connect(speedSlider, &QSlider::valueChanged, this, &INSERTION_SORT::updateSpeed);

    generateRandomData();
    updateVisuals();

    return containerWidget;
}

void INSERTION_SORT::generateRandomData() {
    currentSize = sizeInput->value();
    arrayData.clear();
    for (int i = 0; i < currentSize; ++i) {
        arrayData.append(QRandomGenerator::global()->bounded(10, 101));
    }
    resetSortState();
}

void INSERTION_SORT::resetSortState() {
    animationTimer->stop();
    currentI = 1;
    currentJ = 0;
    currentKey = 0;
    isSortingComplete = false;
    isComparingState = true;

    visualCurrentI = -1;
    visualCompareJ = -1;
    visualInsertIdx = -1;
    visualIsInsertPhase = false;
}

void INSERTION_SORT::triggerGenerate() {
    generateRandomData();
    statusOutput->setText(QString(">> Array split ready. Total %1 elements initialized.\n>> Index 0 is Sorted Group, Remaining are Unsorted.").arg(currentSize));
    updateVisuals();
}

void INSERTION_SORT::togglePlay() {
    if (isSortingComplete) {
        statusOutput->setText(">> Sorting is complete. Reset with 'Generate Data'.");
        return;
    }
    if (animationTimer->isActive()) {
        animationTimer->stop();
        statusOutput->setText(statusOutput->text() + "\n>> Execution Paused.");
    } else {
        animationTimer->start(speedSlider->value());
        statusOutput->setText(statusOutput->text() + "\n>> Group partitioning active...");
    }
}

void INSERTION_SORT::updateSpeed(int value) {
    speedLabel->setText(QString("%1 ms").arg(value));
    if (animationTimer->isActive()) {
        animationTimer->start(value);
    }
}

void INSERTION_SORT::handleStep() {
    if (isSortingComplete) {
        animationTimer->stop();
        return;
    }

    int n = arrayData.size();

    if (currentI < n) {
        if (isComparingState) {
            currentKey = arrayData[currentI];
            currentJ = currentI - 1;

            visualCurrentI = currentI;
            visualCompareJ = currentJ;
            visualInsertIdx = -1;
            visualIsInsertPhase = false;

            statusOutput->setText(QString("[Pick Node] Value %1 from Unsorted Group.\nScanning proper spot inside Sorted Group...")
                                      .arg(currentKey));

            isComparingState = false;
            updateVisuals();
            return;
        }

        if (currentJ >= 0 && arrayData[currentJ] > currentKey) {
            statusOutput->setText(QString("[Shifting] Sorted Element %1 > Key %2.\nMoving node down to clear space.")
                                      .arg(arrayData[currentJ]).arg(currentKey));

            visualCompareJ = currentJ;
            visualIsInsertPhase = false;

            arrayData[currentJ + 1] = arrayData[currentJ];
            currentJ--;

            updateVisuals();
        }
        else {
            int targetPos = currentJ + 1;
            statusOutput->setText(QString("[Insert] Proper gap found at Index %1.\nInserting Key %2 into the Sorted Group.")
                                      .arg(targetPos).arg(currentKey));

            visualInsertIdx = targetPos;
            visualCompareJ = -1;
            visualIsInsertPhase = true;

            arrayData[targetPos] = currentKey;

            currentI++;
            isComparingState = true;

            updateVisuals();
        }
    } else {
        isSortingComplete = true;
        animationTimer->stop();
        statusOutput->setText(">> Success! Whole array consolidated into Sorted Group.");

        visualCurrentI = -1;
        visualCompareJ = -1;
        visualInsertIdx = -1;
        visualIsInsertPhase = false;

        updateVisuals();
    }
}

// 명세 반영: 노드 개수에 비례하여 세로 폭(가변 높이/마진) 실시간 가역 연산 처리 (스크롤바 완전 억제)
void INSERTION_SORT::updateVisuals() {
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
    int boundaryIndex = isSortingComplete ? n : currentI;

    for (int idx = 0; idx < n; ++idx) {
        int val = arrayData[idx];
        double dynamicWidth = 45.0 + (val * 2.4);

        QPen pen(QColor("#21262D"), 1);
        QBrush brush(QColor("#161B22"));

        if (isSortingComplete) {
            brush = QBrush(QColor("#238636"));
            pen = QPen(QColor("#3FB950"), 1);
        } else {
            if (visualIsInsertPhase && idx == visualInsertIdx) {
                brush = QBrush(QColor("#DA3633"));
                pen = QPen(QColor("#F85149"), 1);
            } else if (!visualIsInsertPhase && idx == visualCompareJ) {
                brush = QBrush(QColor("#0D47A1"));
                pen = QPen(QColor("#58A6FF"), 1);
            } else if (idx == visualCurrentI) {
                pen = QPen(QColor("#8B949E"), 1, Qt::DashLine);
            } else if (idx < boundaryIndex) {
                brush = QBrush(QColor("#1F2937"));
                pen = QPen(QColor("#21262D"), 1);
            } else {
                pen = QPen(QColor("#D29922"), 1);
            }
        }

        // 수평 막대 데이터 블록 렌더링 (숫자 텍스트 완전 제거 완료)
        QGraphicsPathItem *boxItem = new QGraphicsPathItem();
        QPainterPath path;
        double cornerRadius = qMin(5.0, dynamicBoxHeight / 3.0);
        path.addRoundedRect(BASE_X, currentY, dynamicWidth, dynamicBoxHeight, cornerRadius, cornerRadius);
        boxItem->setPath(path);
        boxItem->setPen(pen);
        boxItem->setBrush(brush);
        scene->addItem(boxItem);

        // 노드 간 수직 점선 축 (촘촘해지는 임계 구역에서는 노이즈 상쇄 생략)
        if (idx < n - 1 && (idx + 1) != boundaryIndex && dynamicSpacing > 3.0) {
            scene->addLine(BASE_X, currentY + dynamicBoxHeight, BASE_X, currentY + dynamicBoxHeight + dynamicSpacing, QPen(QColor("#30363D"), 1, Qt::DotLine));
        }

        // 정렬/미정렬 그룹 사이의 물리적 경계선 및 가이드 레이블 렌더링
        if (!isSortingComplete && (idx + 1) == boundaryIndex) {
            double dividerY = currentY + dynamicBoxHeight + (dynamicSpacing / 2.0);

            // 그룹 경계 수평선
            scene->addLine(BASE_X - 10, dividerY, BASE_X + 320, dividerY, QPen(QColor("#58A6FF"), 1.2, Qt::SolidLine));

            // 그룹 구별 인디케이터 라벨
            QGraphicsTextItem *sortedLabel = new QGraphicsTextItem("▲ SORTED GROUP");
            sortedLabel->setDefaultTextColor(QColor("#8B949E"));
            QFont lblFont = sortedLabel->font(); lblFont.setBold(true); lblFont.setPointSize(8);
            sortedLabel->setFont(lblFont);
            sortedLabel->setPos(BASE_X + 220, dividerY - 18);
            scene->addItem(sortedLabel);

            QGraphicsTextItem *unsortedLabel = new QGraphicsTextItem("▼ UNSORTED GROUP");
            unsortedLabel->setDefaultTextColor(QColor("#D29922"));
            unsortedLabel->setFont(lblFont);
            unsortedLabel->setPos(BASE_X + 220, dividerY + 2);
            scene->addItem(unsortedLabel);
        }

        currentY += dynamicBoxHeight + dynamicSpacing;
    }

    // 상하좌우 안전 여백 인셋 보정 및 화면 뷰포트 강제 고정
    scene->setSceneRect(BASE_X - 20, 0, 400, currentY + 10);
}