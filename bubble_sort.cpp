#include "bubble_sort.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QRandomGenerator>

// ==========================================
// 시각화 기하학 사양 (기존 상수 제거 후 기준선만 유지)
// ==========================================
static const double BASE_X = -160.0;     // 좌측 기준선 시작점

BUBBLE_SORT::BUBBLE_SORT(QMainWindow* _parent)
    : QObject(_parent), parent(_parent), currentI(0), currentJ(0),
    isSortingComplete(false), isSwapStep(false), currentSize(10) {

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &BUBBLE_SORT::handleStep);
}

BUBBLE_SORT::~BUBBLE_SORT() {
    animationTimer->stop();
}

QWidget* BUBBLE_SORT::setting() {
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet("QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }");

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION (스크롤 제거형 가변 스케일 캔버스)
    // ==========================================
    QWidget *leftWidget = new QWidget(containerWidget);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nDynamic Auto-Scaling Bubble Sort Model", leftWidget);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 8px;");
    leftLayout->addWidget(visTitle);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, leftWidget);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QBrush(QColor("#11141A")));

    // 스크롤바 완전 비활성화 (화면 크기에 딱 맞춤)
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setStyleSheet("QGraphicsView { border: 1px solid #21262D; border-radius: 12px; background-color: #11141A; }");
    leftLayout->addWidget(view);

    // ==========================================
    // [Right Area] CONTROLS
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

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nSorting Performance Dashboard", rightContentWidget);
    controlsTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #8B949E; letter-spacing: 1px; margin-bottom: 4px;");
    rightLayout->addWidget(controlsTitle);

    // 스타일시트 정의 (스택 테마 완벽 계승)
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

    // --- Card 1: Array Size Setup (Limit 비활성화) ---
    QWidget *sizeCard = new QWidget(rightContentWidget); sizeCard->setAttribute(Qt::WA_StyledBackground, true); sizeCard->setStyleSheet(cardStyle);
    QVBoxLayout *sizeL = new QVBoxLayout(sizeCard); QHBoxLayout *sizeTop = new QHBoxLayout();
    QLabel *sTitle = new QLabel("Array Size", sizeCard); sTitle->setStyleSheet(titleStyle);
    QLabel *sDesc = new QLabel("Set dynamic nodes", sizeCard); sDesc->setStyleSheet(descStyle);
    sizeTop->addWidget(sTitle); sizeTop->addWidget(sDesc); sizeTop->addStretch(); // 컴플렉시티 정보(Limit) 제거
    QHBoxLayout *sizeBot = new QHBoxLayout();
    sizeInput = new QSpinBox(sizeCard);
    sizeInput->setRange(1, 1000); // 스크롤이 없으므로 가시성 확보를 위해 300개 정도로 제한 완화
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

    connect(generateButton, &QPushButton::clicked, this, &BUBBLE_SORT::triggerGenerate);
    connect(playButton, &QPushButton::clicked, this, &BUBBLE_SORT::togglePlay);
    connect(speedSlider, &QSlider::valueChanged, this, &BUBBLE_SORT::updateSpeed);

    generateRandomData();
    updateVisuals();

    return containerWidget;
}

void BUBBLE_SORT::generateRandomData() {
    currentSize = sizeInput->value();
    arrayData.clear();
    for (int i = 0; i < currentSize; ++i) {
        arrayData.append(QRandomGenerator::global()->bounded(10, 101));
    }
    resetSortState();
}

void BUBBLE_SORT::resetSortState() {
    animationTimer->stop();
    currentI = 0;
    currentJ = 0;
    isSortingComplete = false;
    isSwapStep = false;
}

void BUBBLE_SORT::triggerGenerate() {
    generateRandomData();
    statusOutput->setText(QString(">> Generated new unsorted dataset. Total %1 elements initialized.\n>> Ready for auto bubble sort.").arg(currentSize));
    updateVisuals();
}

void BUBBLE_SORT::togglePlay() {
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

void BUBBLE_SORT::updateSpeed(int value) {
    speedLabel->setText(QString("%1 ms").arg(value));
    if (animationTimer->isActive()) {
        animationTimer->start(value);
    }
}

void BUBBLE_SORT::handleStep() {
    if (isSortingComplete) {
        animationTimer->stop();
        return;
    }

    int n = arrayData.size();
    int limitJ = n - currentI - 1;

    if (currentI < n - 1) {
        if (currentJ < limitJ) {
            int val1 = arrayData[currentJ];
            int val2 = arrayData[currentJ + 1];

            if (!isSwapStep) {
                statusOutput->setText(QString(">> [Comparing] Index %1 and %2:\n>> Values: [ %3 ] vs [ %4 ]")
                                          .arg(currentJ).arg(currentJ + 1).arg(val1).arg(val2));

                if (val1 > val2) {
                    statusOutput->setText(statusOutput->text() + QString("\n>> Condition Met (%1 > %2). Preparing Swap.").arg(val1).arg(val2));
                    isSwapStep = true;
                } else {
                    currentJ++;
                }
            } else {
                int temp = arrayData[currentJ];
                arrayData[currentJ] = arrayData[currentJ + 1];
                arrayData[currentJ + 1] = temp;

                statusOutput->setText(QString(">> [Swapped] Elements changed positions:\n>> Index %1 ↔ %2 accomplished.")
                                          .arg(currentJ).arg(currentJ + 1));
                isSwapStep = false;
                currentJ++;
            }
        } else {
            int lockedVal = arrayData[n - currentI - 1];
            statusOutput->setText(QString(">> Pass [ %1 ] finished. Settle element at the bottom.").arg(currentI + 1));
            currentJ = 0;
            currentI++;
            isSwapStep = false;
        }
    } else {
        isSortingComplete = true;
        animationTimer->stop();
        statusOutput->setText(QString(">> Success! The dynamic %1-node array is completely sorted.").arg(currentSize));
    }

    updateVisuals();
}

// 명세 반영: 노드 개수에 맞추어 세로 크기(가변 높이/마진)를 역산하여 캔버스에 박제
void BUBBLE_SORT::updateVisuals() {
    scene->clear();
    if (arrayData.isEmpty()) return;

    int n = arrayData.size();

    // 1. 현재 뷰포트 영역의 가용 세로 길이를 가상 계산 (기본 최소 높이 500 보장)
    double viewHeight = qMax(static_cast<double>(view->height() - 40), 500.0);

    // 2. 전체 노드 개수(n)에 맞춰 균등 배분 비율 계산
    // 전체 공간 = (BOX_HEIGHT * n) + (BOX_SPACING * (n - 1))
    // 여기서는 최적 배치 비율인 [박스 높이 : 여백 = 4 : 1] 관계식 적용
    double totalUnits = (4.0 * n) + (1.0 * (n - 1));
    double unitLength = viewHeight / totalUnits;

    double dynamicBoxHeight = qMax(unitLength * 4.0, 2.0); // 최소 높이 2px 보장
    double dynamicSpacing   = qMax(unitLength * 1.0, 0.5); // 최소 마진 0.5px 보장

    double currentY = 10.0;

    for (int idx = 0; idx < n; ++idx) {
        int val = arrayData[idx];
        double dynamicWidth = 45.0 + (val * 2.4);

        QPen pen(QColor("#21262D"), 1);
        QBrush brush(QColor("#161B22"));

        if (isSortingComplete) {
            brush = QBrush(QColor("#238636"));
            pen = QPen(QColor("#3FB950"), 1);
        } else if (idx >= n - currentI) {
            brush = QBrush(QColor("#1F2937"));
            pen = QPen(QColor("#3FB950"), 1, Qt::DashLine);
        } else if (idx == currentJ || idx == currentJ + 1) {
            if (isSwapStep && idx == currentJ) {
                brush = QBrush(QColor("#DA3633"));
                pen = QPen(QColor("#F85149"), 1);
            } else {
                brush = QBrush(QColor("#0D47A1"));
                pen = QPen(QColor("#58A6FF"), 1);
            }
        } else {
            pen = QPen(QColor("#D29922"), 1);
        }

        // 수평 막대 데이터 블록 드로잉 (노드의 텍스트 레이블 제거 완료)
        QGraphicsPathItem *boxItem = new QGraphicsPathItem();
        QPainterPath path;
        // 노드가 극도로 많아질 때 깨짐 방지를 위해 코너 라운딩 반경도 가변 스케일 적용
        double cornerRadius = qMin(5.0, dynamicBoxHeight / 3.0);
        path.addRoundedRect(BASE_X, currentY, dynamicWidth, dynamicBoxHeight, cornerRadius, cornerRadius);
        boxItem->setPath(path);
        boxItem->setPen(pen);
        boxItem->setBrush(brush);
        scene->addItem(boxItem);

        // 노드 간 수직 점선 연결선 (노드가 너무 많아 촘촘할 때는 노이즈 방지를 위해 스킵)
        if (idx < n - 1 && dynamicSpacing > 3.0) {
            scene->addLine(BASE_X, currentY + dynamicBoxHeight, BASE_X, currentY + dynamicBoxHeight + dynamicSpacing, QPen(QColor("#30363D"), 1, Qt::DotLine));
        }

        currentY += dynamicBoxHeight + dynamicSpacing;
    }

    // 뷰포트 내부에 가변 패딩 공간 정밀 안착 및 고정
    scene->setSceneRect(BASE_X - 20, 0, 400, currentY + 10);
}