#include "selection_sort.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QRandomGenerator>
#include <QGroupBox>

SELECTION_SORT::SELECTION_SORT(QMainWindow* _parent)
    : QObject(_parent), parent(_parent), currentI(0), currentJ(1), minIndex(0),
    isSortingComplete(false), isSwapStep(false), currentSize(15) {

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &SELECTION_SORT::handleStep);
}

SELECTION_SORT::~SELECTION_SORT() {
    animationTimer->stop();
}

QWidget* SELECTION_SORT::setting() {
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet("QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }");

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION (자동 뷰포트 스케일링 캔버스)
    // ==========================================
    QWidget *leftWidget = new QWidget(containerWidget);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nDynamic Auto-Scaling Selection Sort Model", leftWidget);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 8px;");
    leftLayout->addWidget(visTitle);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, leftWidget);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QBrush(QColor("#11141A")));

    // 내부 스크롤바 강제 억제 및 보더 처리
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

    // 공용 모던 컴포넌트 스타일시트 명세
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

    // --- Card 1: Array Size Setup (가변 데이터 개수 설정) ---
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

    // --- Card 2: Animation Speed Setup (진행속도 제어 슬라이더) ---
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

    connect(generateButton, &QPushButton::clicked, this, &SELECTION_SORT::triggerGenerate);
    connect(playButton, &QPushButton::clicked, this, &SELECTION_SORT::togglePlay);
    connect(speedSlider, &QSlider::valueChanged, this, &SELECTION_SORT::updateSpeed);

    generateRandomData();
    updateVisuals();

    return containerWidget;
}

void SELECTION_SORT::generateRandomData() {
    currentSize = sizeInput->value();
    arrayData.clear();
    for (int i = 0; i < currentSize; ++i) {
        arrayData.append(QRandomGenerator::global()->bounded(10, 101));
    }
    resetSortState();
}

void SELECTION_SORT::resetSortState() {
    animationTimer->stop();
    currentI = 0;
    currentJ = 1;
    minIndex = 0;
    isSortingComplete = false;
    isSwapStep = false;
}

void SELECTION_SORT::triggerGenerate() {
    generateRandomData();
    statusOutput->setText(QString(">> Generated new unsorted dataset. Total %1 elements initialized.\n>> Ready for auto selection sort.").arg(currentSize));
    updateVisuals();
}

void SELECTION_SORT::togglePlay() {
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

void SELECTION_SORT::updateSpeed(int value) {
    speedLabel->setText(QString("%1 ms").arg(value));
    if (animationTimer->isActive()) {
        animationTimer->start(value);
    }
}

void SELECTION_SORT::handleStep() {
    if (isSortingComplete) {
        animationTimer->stop();
        return;
    }

    int n = arrayData.size();

    if (currentI < n - 1) {
        if (!isSwapStep) {
            if (currentJ < n) {
                statusOutput->setText(QString(">> [Comparing] Index %1 (Current Min Target) vs Index %2\n>> Values: [ %3 ] vs [ %4 ]")
                                          .arg(minIndex).arg(currentJ).arg(arrayData[minIndex]).arg(arrayData[currentJ]));

                if (arrayData[currentJ] < arrayData[minIndex]) {
                    statusOutput->setText(statusOutput->text() + QString("\n>> New Minimum Found! Index %1 (Value: %2) becomes Min Target.")
                                              .arg(currentJ).arg(arrayData[currentJ]));
                    minIndex = currentJ;
                }
                currentJ++;
            } else {
                statusOutput->setText(QString(">> Pass [ %1 ] Search Finished.\n>> Minimum value [ %2 ] found at Index %3. Preparing Swap with Index %4.")
                                          .arg(currentI + 1).arg(arrayData[minIndex]).arg(minIndex).arg(currentI));
                isSwapStep = true;
            }
        } else {
            if (currentI != minIndex) {
                int temp = arrayData[currentI];
                arrayData[currentI] = arrayData[minIndex];
                arrayData[minIndex] = temp;
                statusOutput->setText(QString(">> [Swapped] Minimum element relocated:\n>> Index %1 ↔ %2 accomplished.")
                                          .arg(currentI).arg(minIndex));
            } else {
                statusOutput->setText(QString(">> [No Swap Needed] Minimum element already at Index %1.").arg(currentI));
            }

            currentI++;
            currentJ = currentI + 1;
            minIndex = currentI;
            isSwapStep = false;
        }
    } else {
        isSortingComplete = true;
        animationTimer->stop();
        statusOutput->setText(QString(">> Success! The dynamic %1-node array is completely sorted via Selection Sort.").arg(currentSize));
    }

    updateVisuals();
}

// 명세 반영: 노드 개수(N)가 변경됨에 따라 뷰포트 내에 세로 컴포넌트를 정확히 분할하여 맞춤 배율 적용 (스크롤 스토퍼 빌드)
void SELECTION_SORT::updateVisuals() {
    scene->clear();
    if (arrayData.isEmpty()) return;

    int n = arrayData.size();
    const double BASE_X_COORD = -180.0;

    // 1. 현재 그래픽 뷰의 높이를 기준 삼아 유동적인 행 높이 및 가이드 마진 연산
    double viewHeight = qMax(static_cast<double>(view->height() - 60), 480.0);

    // totalUnits = (3.5 * n) + (1.0 * (n-1)) -> 노드 배율 비율 설정
    double totalUnits = (3.5 * n) + (1.0 * (n - 1));
    double unitLength = viewHeight / totalUnits;

    // 데이터가 늘어나도 경계선이 무너지지 않는 범위 보정 상한/하한값 식
    double dynamicBoxHeight = qMax(unitLength * 3.5, 4.0);
    double dynamicBoxSpacing = qMax(unitLength * 1.0, 1.0);

    double currentY = 20.0;
    QFont textFont("Consolas", 10, QFont::Bold);

    for (int idx = 0; idx < n; ++idx) {
        int val = arrayData[idx];

        // 10~100 사이 숫자를 가로 막대의 해상도 폭으로 부드럽게 사상
        double dynamicWidth = 60.0 + (val * 2.5);

        QPen pen(QColor("#21262D"), 1);
        QBrush brush(QColor("#161B22"));
        QColor textColor(QColor("#E6EDF3"));

        // 상태 의존적 레이아웃 피드백 컬러 필터링
        if (isSortingComplete) {
            brush = QBrush(QColor("#238636"));
            pen = QPen(QColor("#3FB950"), 1);
        } else if (idx < currentI) {
            brush = QBrush(QColor("#1F2937"));
            pen = QPen(QColor("#3FB950"), 1, Qt::DashLine);
            textColor = QColor("#8B949E");
        } else {
            pen = QPen(QColor("#D29922"), 1);

            if (!isSortingComplete) {
                // 탐색 최솟값 보관 노드 (Yellow)
                if (idx == minIndex) {
                    brush = QBrush(QColor("#D29922"));
                    pen = QPen(QColor("#F2B832"), 1);
                    textColor = QColor("#0D1117");
                }
                // 실시간 스캔 대조 추적 노드 (Blue)
                else if (!isSwapStep && idx == (currentJ - 1)) {
                    brush = QBrush(QColor("#0D47A1"));
                    pen = QPen(QColor("#58A6FF"), 1);
                }

                // 패스 완료 시점 물리 교환 정점 노드 (Crimson)
                if (isSwapStep && idx == currentI) {
                    brush = QBrush(QColor("#DA3633"));
                    pen = QPen(QColor("#F85149"), 1);
                }
            }
        }

        // 수평 정렬 막대 컴포넌트 렌더링
        QGraphicsPathItem *boxItem = new QGraphicsPathItem();
        QPainterPath path;
        double cornerRadius = qMin(5.0, dynamicBoxHeight / 3.0);
        path.addRoundedRect(BASE_X_COORD, currentY, dynamicWidth, dynamicBoxHeight, cornerRadius, cornerRadius);
        boxItem->setPath(path);
        boxItem->setPen(pen);
        boxItem->setBrush(brush);
        scene->addItem(boxItem);

        // 노드 가독성 텍스트 수치 매핑 (텍스트가 박스 높이보다 클 경우 축소 혹은 생략 자동 제어)
        if (dynamicBoxHeight >= 12.0) {
            QGraphicsTextItem *textItem = new QGraphicsTextItem(QString::number(val));
            textItem->setDefaultTextColor(textColor);
            textItem->setFont(textFont);

            double textH = textItem->boundingRect().height();
            textItem->setPos(BASE_X_COORD + 12, currentY + (dynamicBoxHeight - textH) / 2);
            scene->addItem(textItem);
        }

        // 마디 연결 수직 링크 점선 생성
        if (idx < n - 1) {
            scene->addLine(BASE_X_COORD, currentY + dynamicBoxHeight,
                           BASE_X_COORD, currentY + dynamicBoxHeight + dynamicBoxSpacing,
                           QPen(QColor("#30363D"), 1, Qt::DotLine));
        }

        currentY += dynamicBoxHeight + dynamicBoxSpacing;
    }

    // 화면 자동 바운드 줌 아웃 스케일 최적화
    QRectF bounds = scene->itemsBoundingRect();
    scene->setSceneRect(bounds.adjusted(-40, -20, 80, 40));
}