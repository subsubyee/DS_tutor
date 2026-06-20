#include "radix_sort.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QRandomGenerator>
#include <QGroupBox>

RADIX_SORT::RADIX_SORT(QMainWindow* _parent)
    : QObject(_parent), parent(_parent), currentDigit(1), maxDigit(100), currentIdx(0),
    currentSubStep(DISTRIBUTE_TO_BUCKETS), activeBucketIdx(-1), isSortingComplete(false), currentSize(15) {

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &RADIX_SORT::handleStep);

    buckets.resize(10);
}

RADIX_SORT::~RADIX_SORT() {
    animationTimer->stop();
}

QWidget* RADIX_SORT::setting() {
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

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nDynamic Auto-Scaling Radix Sort Model", leftWidget);
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

    // 공용 스타일시트 정의
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

    // --- Card 1: Array Size Setup (데이터 개수 설정) ---
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

    // --- Card 2: Animation Speed Setup (진행속도 제어) ---
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

    connect(generateButton, &QPushButton::clicked, this, &RADIX_SORT::triggerGenerate);
    connect(playButton, &QPushButton::clicked, this, &RADIX_SORT::togglePlay);
    connect(speedSlider, &QSlider::valueChanged, this, &RADIX_SORT::updateSpeed);

    generateRandomData();
    updateVisuals();

    return containerWidget;
}

void RADIX_SORT::generateRandomData() {
    currentSize = sizeInput->value();
    arrayData.clear();
    for (int i = 0; i < currentSize; ++i) {
        arrayData.append(QRandomGenerator::global()->bounded(100, 1000));
    }
    resetSortState();
}

void RADIX_SORT::resetSortState() {
    animationTimer->stop();
    currentDigit = 1;
    currentIdx = 0;
    activeBucketIdx = -1;
    isSortingComplete = false;
    currentSubStep = DISTRIBUTE_TO_BUCKETS;

    for (int i = 0; i < 10; ++i) {
        buckets[i].clear();
    }
}

void RADIX_SORT::triggerGenerate() {
    generateRandomData();
    statusOutput->setText(QString(">> Generated %1 nodes (3-digit integers).\n>> Ready for Radix Sort. Starting with 1's digit.").arg(currentSize));
    updateVisuals();
}

void RADIX_SORT::togglePlay() {
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

void RADIX_SORT::updateSpeed(int value) {
    speedLabel->setText(QString("%1 ms").arg(value));
    if (animationTimer->isActive()) {
        animationTimer->start(value);
    }
}

void RADIX_SORT::handleStep() {
    if (isSortingComplete) {
        animationTimer->stop();
        return;
    }

    int n = arrayData.size();

    switch (currentSubStep) {
    case DISTRIBUTE_TO_BUCKETS: {
        if (currentIdx < n) {
            int value = arrayData[currentIdx];
            int bucketNumber = (value / currentDigit) % 10;

            buckets[bucketNumber].append(value);
            activeBucketIdx = bucketNumber;

            statusOutput->setText(QString(">> [Distributing / Digit: %1]\n>> Array Index %2 (Value: %3) -> Placed into Bucket [ %4 ]")
                                      .arg(currentDigit).arg(currentIdx).arg(value).arg(bucketNumber));
            currentIdx++;
        } else {
            currentSubStep = COLLECT_FROM_BUCKETS;
            currentIdx = 0;
            activeBucketIdx = 0;
            handleStep();
        }
        break;
    }

    case COLLECT_FROM_BUCKETS: {
        while (activeBucketIdx < 10 && buckets[activeBucketIdx].isEmpty()) {
            activeBucketIdx++;
        }

        if (activeBucketIdx < 10) {
            int value = buckets[activeBucketIdx].takeFirst();
            arrayData[currentIdx] = value;

            statusOutput->setText(QString(">> [Collecting / Digit: %1]\n>> Extracted %2 from Bucket [ %3 ] -> Restored to Array Index %4")
                                      .arg(currentDigit).arg(value).arg(activeBucketIdx).arg(currentIdx));
            currentIdx++;
        } else {
            currentSubStep = CHECK_NEXT_DIGIT;
            handleStep();
        }
        break;
    }

    case CHECK_NEXT_DIGIT: {
        activeBucketIdx = -1;
        if (currentDigit < maxDigit) {
            currentDigit *= 10;
            currentIdx = 0;
            currentSubStep = DISTRIBUTE_TO_BUCKETS;
            statusOutput->setText(QString(">> Pass complete for previous digit.\n>> Moving up to next digit level: [ %1's Place ]").arg(currentDigit));
        } else {
            isSortingComplete = true;
            animationTimer->stop();
            statusOutput->setText(">> Success! Radix Sort finalized. All digits processed cleanly.");
        }
        break;
    }
    }

    updateVisuals();
}

void RADIX_SORT::updateVisuals() {
    scene->clear();
    if (arrayData.isEmpty()) return;

    int n = arrayData.size();

    // 1. 현재 가용 뷰포트 높이 기반 스케일링 계산 (최소 고정 높이 500 내에서 압축 비율 산정)
    double viewHeight = qMax(static_cast<double>(view->height() - 60), 500.0);

    // 메인 배열 배치용 동적 스펙 산출
    double totalUnits = (4.0 * n) + (1.0 * (n - 1));
    double unitLength = viewHeight / totalUnits;
    double dynamicNodeHeight = qMax(unitLength * 4.0, 2.0);
    double dynamicLineSpacing = qMax(unitLength * 1.0, 0.5);

    // 버킷 10개 고정 기둥용 동적 스펙 산출
    double bucketUnit = viewHeight / 59.0; // (4.5 * 10) + (1.0 * 9) = 54 + 5 = 59
    double dynamicBucketHeight = qMax(bucketUnit * 4.5, 30.0);
    double dynamicBucketSpacing = qMax(bucketUnit * 1.0, 4.0);

    // 기하학적 수평 컬럼 좌표 배치선
    const double START_X = -320.0;        // 1열 메인 데이터 배열 컬럼
    const double BUCKET_LABEL_X = -10.0;  // 2열 버킷 번호 인덱스 컬럼
    const double BUCKET_DATA_X = 40.0;    // 3열 버킷 내부 전개 링크 데이터 컬럼

    // -------------------------------------------------------------
    // 1열: 원본 / 현재 진행 중인 가변 메인 배열 수평 막대 렌더링
    // -------------------------------------------------------------
    double arrayY = 20.0;

    // 배열 전체 테두리 박스 감싸기
    double totalArrayHeight = (dynamicNodeHeight * n) + (dynamicLineSpacing * (n - 1));
    scene->addRect(START_X - 10, arrayY - 10, 160 + 20, totalArrayHeight + 20, QPen(QColor("#21262D"), 1));

    for (int idx = 0; idx < n; ++idx) {
        int val = arrayData[idx];

        // 원본 3자리 데이터 값을 가시성 좋게 가로 막대 너비로 치환하는 스케일식
        double dynamicWidth = 45.0 + ((val - 100) * 0.13);

        QPen currentPen(QColor("#D29922"), 1);
        QBrush currentBrush(QColor("#161B22"));

        if (isSortingComplete) {
            currentBrush = QBrush(QColor("#238636"));
            currentPen = QPen(QColor("#3FB950"), 1);
        } else if (currentSubStep == DISTRIBUTE_TO_BUCKETS && idx == currentIdx) {
            currentBrush = QBrush(QColor("#D29922"));
            currentPen = QPen(QColor("#F2B832"), 1);
        } else if (currentSubStep == COLLECT_FROM_BUCKETS && idx < currentIdx) {
            currentBrush = QBrush(QColor("#0D47A1"));
            currentPen = QPen(QColor("#58A6FF"), 1);
        }

        QGraphicsPathItem *boxItem = new QGraphicsPathItem();
        QPainterPath path;
        double cornerRadius = qMin(4.0, dynamicNodeHeight / 3.0);
        path.addRoundedRect(START_X, arrayY, dynamicWidth, dynamicNodeHeight, cornerRadius, cornerRadius);
        boxItem->setPath(path);
        boxItem->setPen(currentPen);
        boxItem->setBrush(currentBrush);
        scene->addItem(boxItem);

        arrayY += dynamicNodeHeight + dynamicLineSpacing;
    }

    // -------------------------------------------------------------
    // 2열 & 3열: 0~9번 버킷 인덱스 기둥 및 링크 데이터 수평 전개
    // -------------------------------------------------------------
    double bucketY = 20.0;
    double totalBucketHeight = (dynamicBucketHeight * 10) + (dynamicBucketSpacing * 9);

    // 버킷 전체 구조선 가이드 박스
    scene->addRect(BUCKET_LABEL_X - 5, bucketY - 5, 40, totalBucketHeight + 10, QPen(QColor("#21262D"), 1));

    QFont font("Consolas", 10, QFont::Bold);

    for (int b = 0; b < 10; ++b) {
        QGraphicsTextItem *lblItem = new QGraphicsTextItem(QString::number(b));
        lblItem->setFont(font);

        if (b == activeBucketIdx) {
            lblItem->setDefaultTextColor(QColor("#58A6FF"));
        } else {
            lblItem->setDefaultTextColor(QColor("#CDD6F4"));
        }

        double lblW = lblItem->boundingRect().width();
        double lblH = lblItem->boundingRect().height();
        lblItem->setPos(BUCKET_LABEL_X + (30.0 - lblW) / 2.0, bucketY + (dynamicBucketHeight - lblH) / 2.0);
        scene->addItem(lblItem);

        // 3열: 버킷 리스트 내부에 링크된 요소들을 가로로 연달아 텍스트 노드로 표시
        double itemXOffset = BUCKET_DATA_X;
        for (int i = 0; i < buckets[b].size(); ++i) {
            int bucketValue = buckets[b][i];

            QGraphicsTextItem *valItem = new QGraphicsTextItem(QString::number(bucketValue));
            valItem->setFont(font);

            if (b == activeBucketIdx && currentSubStep == DISTRIBUTE_TO_BUCKETS && i == buckets[b].size() - 1) {
                valItem->setDefaultTextColor(QColor("#F2B832"));
            } else {
                valItem->setDefaultTextColor(QColor("#E6EDF3"));
            }

            double valH = valItem->boundingRect().height();
            valItem->setPos(itemXOffset, bucketY + (dynamicBucketHeight - valH) / 2.0);
            scene->addItem(valItem);

            itemXOffset += 55.0;
        }

        if (b < 9) {
            scene->addLine(BUCKET_LABEL_X - 5, bucketY + dynamicBucketHeight, BUCKET_LABEL_X + 35, bucketY + dynamicBucketHeight, QPen(QColor("#21262D"), 1));
        }

        bucketY += dynamicBucketHeight + dynamicBucketSpacing;
    }

    // -------------------------------------------------------------
    // 상단 타겟 자릿수 라이브 안내 인디케이터
    // -------------------------------------------------------------
    QGraphicsTextItem *digitLabel = new QGraphicsTextItem(QString("[ Current Target Digit: %1's Place ]").arg(currentDigit));
    QFont labelFont("Segoe UI", 12, QFont::Bold);
    digitLabel->setFont(labelFont);
    digitLabel->setDefaultTextColor(QColor("#58A6FF"));
    digitLabel->setPos(START_X - 10, -25);
    scene->addItem(digitLabel);

    QRectF bounds = scene->itemsBoundingRect();
    scene->setSceneRect(bounds.adjusted(-40, -30, 60, 30));
}