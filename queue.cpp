#include "queue.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QScrollBar>

// ==========================================
// 1. QUEUE 구조체 생성자 & 소멸자
// ==========================================
QUEUE::QUEUE(int _max, QMainWindow* _parent){
    max = _max;
    parent = _parent;
}

QUEUE::~QUEUE() {
    delete[] list;
    delete[] queueCells;
}

// ==========================================
// 2. UI 레이아웃 세팅 (모던 다크 카드 뷰 & 인덱스 바닥 고정형)
// ==========================================
QWidget* QUEUE::setting(){
    list = new QString[max];
    str = 0;
    fin = 0;
    size = 0;
    queueCells = new QLabel*[max];

    // --- 메인 컨테이너 및 전역 배경 설정 (Modern Obsidian Dark Theme) ---
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet(
        "QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }"
        );

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION (고정형 큐 슬롯 레이아웃)
    // ==========================================
    QWidget *leftWidget = new QWidget(containerWidget);
    QVBoxLayout *leftVLayout = new QVBoxLayout(leftWidget);
    leftVLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nFixed Array Queue Slots", leftWidget);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 8px;");
    leftVLayout->addWidget(visTitle);

    // 큐 슬롯들이 넘칠 때를 대비한 부드러운 스크롤 영역
    QScrollArea *scrollArea = new QScrollArea(leftWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea { border: none; background-color: transparent; }"
        "QScrollBar:vertical { background: #161B22; width: 8px; margin-left: 2px; }"
        "QScrollBar::handle:vertical { background: #30363D; border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: #8B949E; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { background: none; height: 0px; }"
        );

    QWidget *queueContainer = new QWidget();
    queueContainer->setStyleSheet("background-color: #11141A; border-radius: 12px; border: 1px solid #21262D;");

    QVBoxLayout *queueLayout = new QVBoxLayout(queueContainer);
    queueLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    queueLayout->setContentsMargins(20, 20, 20, 20);
    queueLayout->setSpacing(10); // 슬롯 간 컴팩트한 간격 조정

    // [역순 배치] index 0번 슬롯이 바닥(가장 아래)에 보이도록 루프 설계
    for (int i = max - 1; i >= 0; --i) {
        queueCells[i] = new QLabel(queueContainer);
        queueCells[i]->setAlignment(Qt::AlignCenter);
        queueCells[i]->setFixedSize(180, 70); // 양식에 맞춘 180 x 70 해상도 고정
        queueLayout->addWidget(queueCells[i]);
    }

    scrollArea->setWidget(queueContainer);
    leftVLayout->addWidget(scrollArea, 1);

    // ==========================================
    // [Right Area] CONTROLS (그리드 카드 대시보드)
    // ==========================================
    QScrollArea *rightScrollArea = new QScrollArea(containerWidget);
    rightScrollArea->setWidgetResizable(true);
    rightScrollArea->setFixedWidth(540);
    rightScrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *rightContentWidget = new QWidget();
    rightContentWidget->setObjectName("RightContent");
    rightContentWidget->setStyleSheet("QWidget#RightContent { background: transparent; }");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightContentWidget);
    rightLayout->setContentsMargins(0, 0, 8, 0);
    rightLayout->setSpacing(12);

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nQueue Operations Dashboard", rightContentWidget);
    controlsTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #8B949E; letter-spacing: 1px; margin-bottom: 4px;");
    rightLayout->addWidget(controlsTitle);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(12);

    // --- 일관성 있는 Neon-Dark UI 스타일시트 정의 ---
    QString cardStyle   = "QWidget { background-color: #161B22; border: 1px solid #21262D; border-radius: 10px; }";
    QString titleStyle  = "color: #58A6FF; font-weight: bold; font-family: 'Consolas', monospace; font-size: 13px; background: transparent; border: none;";
    QString descStyle   = "color: #8B949E; font-size: 11px; background: transparent; border: none;";
    QString compStyle   = "color: #56B6C2; font-size: 11px; font-weight: bold; font-family: monospace; background: transparent; border: none;";
    QString inputStyle  = "QLineEdit { background-color: #0D1117; border: 1px solid #30363D; border-radius: 6px; color: #E6EDF3; padding: 6px 10px; selection-background-color: #238636; } "
                         "QLineEdit:focus { border: 1px solid #58A6FF; }";
    QString btnStyle    = "QPushButton { background-color: #238636; color: #FFFFFF; border-radius: 6px; padding: 6px 16px; font-weight: bold; border: none; }"
                       "QPushButton:hover { background-color: #2EA043; }"
                       "QPushButton:pressed { background-color: #248039; }";
    QString outputStyle = "QLabel { color: #EC5990; font-size: 12px; font-weight: bold; background-color: #0D1117; border: 1px solid #21262D; border-radius: 6px; padding: 6px 10px; }";

    // --- Card 0: enqueue ---
    QWidget *enqueueCard = new QWidget(rightContentWidget); enqueueCard->setAttribute(Qt::WA_StyledBackground, true); enqueueCard->setStyleSheet(cardStyle);
    QVBoxLayout *enqueueL = new QVBoxLayout(enqueueCard); QHBoxLayout *enqueueTop = new QHBoxLayout();
    QLabel *enqTitle = new QLabel("enqueue()", enqueueCard); enqTitle->setStyleSheet(titleStyle);
    QLabel *enqDesc = new QLabel("Insert element", enqueueCard); enqDesc->setStyleSheet(descStyle);
    QLabel *enqComp = new QLabel("O(1)", enqueueCard); enqComp->setStyleSheet(compStyle);
    enqueueTop->addWidget(enqTitle); enqueueTop->addWidget(enqDesc); enqueueTop->addStretch(); enqueueTop->addWidget(enqComp);
    QHBoxLayout *enqueueBot = new QHBoxLayout(); enqueueInput = new QLineEdit(enqueueCard); enqueueInput->setPlaceholderText("Enter value..."); enqueueInput->setStyleSheet(inputStyle);
    enqueueButton = new QPushButton("Enqueue", enqueueCard); enqueueButton->setStyleSheet(btnStyle);
    enqueueBot->addWidget(enqueueInput); enqueueBot->addWidget(enqueueButton); enqueueL->addLayout(enqueueTop); enqueueL->addLayout(enqueueBot);
    gridLayout->addWidget(enqueueCard, 0, 0);

    // --- Card 1: dequeue ---
    QWidget *dequeueCard = new QWidget(rightContentWidget); dequeueCard->setAttribute(Qt::WA_StyledBackground, true); dequeueCard->setStyleSheet(cardStyle);
    QVBoxLayout *dequeueL = new QVBoxLayout(dequeueCard); QHBoxLayout *dequeueTop = new QHBoxLayout();
    QLabel *deqTitle = new QLabel("dequeue()", dequeueCard); deqTitle->setStyleSheet(titleStyle);
    QLabel *deqDesc = new QLabel("Remove front", dequeueCard); deqDesc->setStyleSheet(descStyle);
    QLabel *deqComp = new QLabel("O(1)", dequeueCard); deqComp->setStyleSheet(compStyle);
    dequeueTop->addWidget(deqTitle); dequeueTop->addWidget(deqDesc); dequeueTop->addStretch(); dequeueTop->addWidget(deqComp);
    QHBoxLayout *dequeueBot = new QHBoxLayout(); dequeueOutput = new QLabel("Ready", dequeueCard); dequeueOutput->setStyleSheet(outputStyle);
    dequeueButton = new QPushButton("Dequeue", dequeueCard); dequeueButton->setStyleSheet(btnStyle);
    dequeueBot->addWidget(dequeueOutput, 1); dequeueBot->addWidget(dequeueButton); dequeueL->addLayout(dequeueTop); dequeueL->addLayout(dequeueBot);
    gridLayout->addWidget(dequeueCard, 0, 1);

    // --- Card 2: first ---
    QWidget *firstCard = new QWidget(rightContentWidget); firstCard->setAttribute(Qt::WA_StyledBackground, true); firstCard->setStyleSheet(cardStyle);
    QVBoxLayout *firstL = new QVBoxLayout(firstCard); QHBoxLayout *firstTop = new QHBoxLayout();
    QLabel *fTitle = new QLabel("first()", firstCard); fTitle->setStyleSheet(titleStyle);
    QLabel *fDesc = new QLabel("Peek front", firstCard); fDesc->setStyleSheet(descStyle);
    QLabel *fComp = new QLabel("O(1)", firstCard); fComp->setStyleSheet(compStyle);
    firstTop->addWidget(fTitle); firstTop->addWidget(fDesc); firstTop->addStretch(); firstTop->addWidget(fComp);
    QHBoxLayout *firstBot = new QHBoxLayout(); firstOutput = new QLabel("Ready", firstCard); firstOutput->setStyleSheet(outputStyle);
    firstButton = new QPushButton("First", firstCard); firstButton->setStyleSheet(btnStyle);
    firstBot->addWidget(firstOutput, 1); firstBot->addWidget(firstButton); firstL->addLayout(firstTop); firstL->addLayout(firstBot);
    gridLayout->addWidget(firstCard, 1, 0);

    // --- Card 3: is_empty ---
    QWidget *emptyCard = new QWidget(rightContentWidget); emptyCard->setAttribute(Qt::WA_StyledBackground, true); emptyCard->setStyleSheet(cardStyle);
    QVBoxLayout *emptyL = new QVBoxLayout(emptyCard); QHBoxLayout *emptyTop = new QHBoxLayout();
    QLabel *eTitle = new QLabel("is_empty()", emptyCard); eTitle->setStyleSheet(titleStyle);
    QLabel *eDesc = new QLabel("Check empty", emptyCard); eDesc->setStyleSheet(descStyle);
    QLabel *eComp = new QLabel("O(1)", emptyCard); eComp->setStyleSheet(compStyle);
    emptyTop->addWidget(eTitle); emptyTop->addWidget(eDesc); emptyTop->addStretch(); emptyTop->addWidget(eComp);
    QHBoxLayout *emptyBot = new QHBoxLayout(); isEmptyOutput = new QLabel("Ready", emptyCard); isEmptyOutput->setStyleSheet(outputStyle);
    isEmptyButton = new QPushButton("Check", emptyCard); isEmptyButton->setStyleSheet(btnStyle);
    emptyBot->addWidget(isEmptyOutput, 1); emptyBot->addWidget(isEmptyButton); emptyL->addLayout(emptyTop); emptyL->addLayout(emptyBot);
    gridLayout->addWidget(emptyCard, 1, 1);

    // --- Card 4: len ---
    QWidget *lenCard = new QWidget(rightContentWidget); lenCard->setAttribute(Qt::WA_StyledBackground, true); lenCard->setStyleSheet(cardStyle);
    QVBoxLayout *lenL = new QVBoxLayout(lenCard); QHBoxLayout *lenTop = new QHBoxLayout();
    QLabel *lTitle = new QLabel("len()", lenCard); lTitle->setStyleSheet(titleStyle);
    QLabel *lDesc = new QLabel("Get elements count", lenCard); lDesc->setStyleSheet(descStyle);
    QLabel *lComp = new QLabel("O(1)", lenCard); lComp->setStyleSheet(compStyle);
    lenTop->addWidget(lTitle); lenTop->addWidget(lDesc); lenTop->addStretch(); lenTop->addWidget(lComp);
    QHBoxLayout *lenBot = new QHBoxLayout(); lenOutput = new QLabel("Ready", lenCard); lenOutput->setStyleSheet(outputStyle);
    lenButton = new QPushButton("Size", lenCard); lenButton->setStyleSheet(btnStyle);
    lenBot->addWidget(lenOutput, 1); lenBot->addWidget(lenButton); lenL->addLayout(lenTop); lenL->addLayout(lenBot);
    gridLayout->addWidget(lenCard, 2, 0);

    rightLayout->addLayout(gridLayout);

    // --- Bottom: Operation Log Terminal ---
    QLabel *logTitle = new QLabel(">_ System Live Log", rightContentWidget);
    logTitle->setStyleSheet("color: #8B949E; font-size: 11px; font-family: monospace; font-weight: bold; margin-top: 6px;");
    rightLayout->addWidget(logTitle);

    statusOutput = new QLabel("System initialized. Ready for operations.", rightContentWidget);
    statusOutput->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    statusOutput->setStyleSheet(
        "background-color: #0D1117; border: 1px solid #21262D; border-radius: 8px; "
        "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
        );
    rightLayout->addWidget(statusOutput);

    rightScrollArea->setWidget(rightContentWidget);

    // ==========================================
    // 3. 레이아웃 조립 및 이벤트 시그널 연결
    // ==========================================
    mainLayout->addWidget(leftWidget, 1);
    mainLayout->addWidget(rightScrollArea);

    QObject::connect(enqueueButton, &QPushButton::clicked, [=]() { this->enqueue(); });
    QObject::connect(dequeueButton, &QPushButton::clicked, [=]() { this->dequeue(); });
    QObject::connect(firstButton, &QPushButton::clicked, [=]() { this->first(); });
    QObject::connect(isEmptyButton, &QPushButton::clicked, [=]() { this->is_empty(); });
    QObject::connect(lenButton, &QPushButton::clicked, [=]() { this->_len(); });

    this->update();

    return containerWidget;
}

// ==========================================
// 4. 시각화 셀 스타일 드로잉 제어 (LINKED_STACK 크기 및 디자인 통일)
// ==========================================
void QUEUE::update(){
    for (int i = 0; i < max; ++i) {
        queueCells[i]->setTextFormat(Qt::RichText);
        queueCells[i]->setFixedSize(180, 70); // 모든 셀의 크기를 180 x 70으로 고정

        // 포인터 텍스트 포맷 생성
        QString pointerStr = "";
        if (str == fin && i == str) {
            pointerStr = "<br><span style='font-size: 10pt; color: #FF7B72;'><b>(Front, Rear)</b></span>";
        } else if (i == str) {
            pointerStr = "<br><span style='font-size: 10pt; color: #FF7B72;'><b>(Front)</b></span>";
        } else if (i == fin) {
            pointerStr = "<br><span style='font-size: 10pt; color: #58A6FF;'><b>(Rear)</b></span>";
        }

        // Active node condition
        if (size > 0 && ((str < fin && i >= str && i < fin) || (str >= fin && (i >= str || i < fin)))) {
            // [통일] 데이터가 존재하는 노드
            queueCells[i]->setText(
                QString("Data (Idx: %1)<br><span style='font-size: 16pt;'>%2</span>%3")
                    .arg(i)
                    .arg(list[i])
                    .arg(pointerStr)
                );

            queueCells[i]->setStyleSheet(
                "QLabel {"
                "   border-radius: 8px;"
                "   background-color: #D29922;"
                "   color: #0D1117;"
                "   font-weight: bold;"
                "}"
                );
        }
        else {
            // [통일] 비어 있는 슬롯
            queueCells[i]->setText(
                QString("[ %1 ]<br><span style='font-size: 10pt;'>Empty</span>%2")
                    .arg(i)
                    .arg(pointerStr)
                );

            queueCells[i]->setStyleSheet(
                "QLabel {"
                "   border: 2px dashed #30363D;"
                "   border-radius: 8px;"
                "   background-color: transparent;"
                "   color: #8B949E;"
                "   font-weight: bold;"
                "}"
                );
        }
    }
}

// ==========================================
// 5. 비즈니스 로직 함수군 (터미널 문장 규칙 완벽 통일)
// ==========================================
void QUEUE::enqueue(){
    QString text = enqueueInput->text().trimmed();
    if (text.isEmpty()){
        statusOutput->setText(">> Error: Cannot enqueue an empty string.");
        return;
    }

    if (size == max){
        statusOutput->setText(">> Error: Queue Overflow! Dynamic limit reached (" + QString::number(max) + ").");
        return;
    }

    list[fin] = text;
    fin = (fin + 1) % max;
    size++;
    enqueueInput->clear();
    statusOutput->setText(">> Success: Enqueued value '" + text + "' into Slot [" + QString::number((fin - 1 + max) % max) + "].");
    update();
}

void QUEUE::dequeue(){
    if (size == 0) {
        statusOutput->setText(">> Error: Queue Underflow! No items left to dequeue.");
        return;
    }

    QString val = list[str];
    str = (str + 1) % max;
    size--;
    dequeueOutput->setText(val);
    statusOutput->setText(">> Success: Dequeued element '" + val + "' from Slot [" + QString::number((str - 1 + max) % max) + "].");
    update();
}

void QUEUE::first(){
    if (size == 0) {
        statusOutput->setText(">> Warning: Queue is empty. First is undefined.");
        firstOutput->setText("Empty");
        return;
    }
    firstOutput->setText(list[str]);
    statusOutput->setText(">> Peek: Current front value is '" + list[str] + "' at Slot [" + QString::number(str) + "].");
}

void QUEUE::is_empty(){
    if (size == 0) {
        isEmptyOutput->setText("True");
        statusOutput->setText(">> Inspection: is_empty() -> TRUE (Queue is currently empty).");
    } else {
        isEmptyOutput->setText("False");
        statusOutput->setText(">> Inspection: is_empty() -> FALSE (Queue has " + QString::number(size) + " item(s)).");
    }
}

void QUEUE::_len(){
    lenOutput->setText(QString::number(size) + " / " + QString::number(max));
    statusOutput->setText(">> Inspection: len() -> Queue utilizes " + QString::number(size) + " out of " + QString::number(max) + " slots.");
}