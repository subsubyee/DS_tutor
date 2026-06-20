#include "circular_linked.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QScrollBar>
#include <cmath>

// ==========================================
// 1. CIRCULAR_LINKED 구조체 생성자 & 소멸자
// ==========================================
CIRCULAR_LINKED::CIRCULAR_LINKED(QMainWindow* _parent){
    parent = _parent;
}

CIRCULAR_LINKED::~CIRCULAR_LINKED() {
    if (tail != nullptr) {
        Node* head = tail->next;
        tail->next = nullptr;

        Node* current = head;
        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }
}

// ==========================================
// 2. UI 레이아웃 세팅 (모던 다크 카드 뷰 & 원형 링 노드 시각화)
// ==========================================
QWidget* CIRCULAR_LINKED::setting(){
    tail = nullptr;
    size = 0;

    // --- 메인 컨테이너 및 전역 배경 설정 (Modern Obsidian Dark Theme) ---
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet(
        "QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }"
        );

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION (상단 프록시 스크롤바 레이아웃 반영)
    // ==========================================
    QWidget *leftWidget = new QWidget(containerWidget);
    QVBoxLayout *leftVLayout = new QVBoxLayout(leftWidget);
    leftVLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nCircular Linked Queue Topology", leftWidget);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 8px;");
    leftVLayout->addWidget(visTitle);

    // 상단 프록시 가로 스크롤바 생성 및 모던 디자인 정의
    QScrollBar *topScrollBar = new QScrollBar(Qt::Horizontal, leftWidget);
    topScrollBar->setStyleSheet(
        "QScrollBar:horizontal { background: #161B22; height: 8px; border-radius: 4px; margin-bottom: 6px; }"
        "QScrollBar::handle:horizontal { background: #30363D; border-radius: 4px; min-width: 20px; }"
        "QScrollBar::handle:horizontal:hover { background: #8B949E; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { background: none; width: 0px; }"
        );

    // 본체 스크롤 영역 생성
    QScrollArea *scrollArea = new QScrollArea(leftWidget);
    scrollArea->setWidgetResizable(false);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea { border: none; background-color: transparent; }"
        "QScrollBar:vertical { background: #161B22; width: 8px; }"
        "QScrollBar::handle:vertical { background: #30363D; border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: #8B949E; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { background: none; }"
        );

    // 상단 바 - 본체 가로 스크롤바 데이터 양방향 동기화 연동
    QScrollBar *nativeHBar = scrollArea->horizontalScrollBar();
    QObject::connect(nativeHBar, &QScrollBar::rangeChanged, [=](int min, int max) {
        topScrollBar->setRange(min, max);
        topScrollBar->setVisible(max > min);
    });
    QObject::connect(topScrollBar, &QScrollBar::valueChanged, nativeHBar, &QScrollBar::setValue);
    QObject::connect(nativeHBar, &QScrollBar::valueChanged, topScrollBar, &QScrollBar::setValue);

    stackContainer = new QWidget();
    stackContainer->setStyleSheet("background-color: #11141A; border-radius: 12px; border: 1px solid #21262D;");

    scrollArea->setWidget(stackContainer);

    leftVLayout->addWidget(topScrollBar);
    leftVLayout->addWidget(scrollArea, 1);

    // ==========================================
    // [Right Area] CONTROLS (그리드 카드 대시보드 - 우측 짤림 방지 스크롤 포함)
    // ==========================================
    QScrollArea *rightScrollArea = new QScrollArea(containerWidget);
    rightScrollArea->setWidgetResizable(true);
    rightScrollArea->setFixedWidth(540); // 스택/큐/덱과 패널 폭 통일
    rightScrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *rightContentWidget = new QWidget();
    rightContentWidget->setObjectName("RightContent");
    rightContentWidget->setStyleSheet("QWidget#RightContent { background: transparent; }");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightContentWidget);
    rightLayout->setContentsMargins(0, 0, 8, 0);
    rightLayout->setSpacing(12);

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nCircular Queue Operations Dashboard", rightContentWidget);
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

    // --- Card 0: enqueue (0행 0열) ---
    QWidget *enqueueCard = new QWidget(rightContentWidget); enqueueCard->setAttribute(Qt::WA_StyledBackground, true); enqueueCard->setStyleSheet(cardStyle);
    QVBoxLayout *enqLayout = new QVBoxLayout(enqueueCard); QHBoxLayout *enqTop = new QHBoxLayout();
    QLabel *enqT = new QLabel("enqueue()", enqueueCard); enqT->setStyleSheet(titleStyle);
    QLabel *enqD = new QLabel("Insert at tail", enqueueCard); enqD->setStyleSheet(descStyle);
    QLabel *enqC = new QLabel("O(1)", enqueueCard); enqC->setStyleSheet(compStyle);
    enqTop->addWidget(enqT); enqTop->addWidget(enqD); enqTop->addStretch(); enqTop->addWidget(enqC);
    QHBoxLayout *enqBot = new QHBoxLayout(); enqueueInput = new QLineEdit(enqueueCard); enqueueInput->setPlaceholderText("Enter value..."); enqueueInput->setStyleSheet(inputStyle);
    enqueueButton = new QPushButton("Enq", enqueueCard); enqueueButton->setStyleSheet(btnStyle);
    enqBot->addWidget(enqueueInput); enqBot->addWidget(enqueueButton); enqLayout->addLayout(enqTop); enqLayout->addLayout(enqBot);
    gridLayout->addWidget(enqueueCard, 0, 0);

    // --- Card 1: dequeue (0행 1열) ---
    QWidget *dequeueCard = new QWidget(rightContentWidget); dequeueCard->setAttribute(Qt::WA_StyledBackground, true); dequeueCard->setStyleSheet(cardStyle);
    QVBoxLayout *deqLayout = new QVBoxLayout(dequeueCard); QHBoxLayout *deqTop = new QHBoxLayout();
    QLabel *deqT = new QLabel("dequeue()", dequeueCard); deqT->setStyleSheet(titleStyle);
    QLabel *deqD = new QLabel("Remove from head", dequeueCard); deqD->setStyleSheet(descStyle);
    QLabel *deqC = new QLabel("O(1)", dequeueCard); deqC->setStyleSheet(compStyle);
    deqTop->addWidget(deqT); deqTop->addWidget(deqD); deqTop->addStretch(); deqTop->addWidget(deqC);
    QHBoxLayout *deqBot = new QHBoxLayout(); dequeueOutput = new QLabel("Ready", dequeueCard); dequeueOutput->setStyleSheet(outputStyle);
    dequeueButton = new QPushButton("Deq", dequeueCard); dequeueButton->setStyleSheet(btnStyle);
    deqBot->addWidget(dequeueOutput, 1); deqBot->addWidget(dequeueButton); deqLayout->addLayout(deqTop); deqLayout->addLayout(deqBot);
    gridLayout->addWidget(dequeueCard, 0, 1);

    // --- Card 2: first (1행 0열) ---
    QWidget *firstCard = new QWidget(rightContentWidget); firstCard->setAttribute(Qt::WA_StyledBackground, true); firstCard->setStyleSheet(cardStyle);
    QVBoxLayout *firstLayout = new QVBoxLayout(firstCard); QHBoxLayout *firstTop = new QHBoxLayout();
    QLabel *fT = new QLabel("first()", firstCard); fT->setStyleSheet(titleStyle);
    QLabel *fD = new QLabel("Peek front element", firstCard); fD->setStyleSheet(descStyle);
    QLabel *fC = new QLabel("O(1)", firstCard); fC->setStyleSheet(compStyle);
    firstTop->addWidget(fT); firstTop->addWidget(fD); firstTop->addStretch(); firstTop->addWidget(fC);
    QHBoxLayout *firstBot = new QHBoxLayout(); firstOutput = new QLabel("Ready", firstCard); firstOutput->setStyleSheet(outputStyle);
    firstButton = new QPushButton("First", firstCard); firstButton->setStyleSheet(btnStyle);
    firstBot->addWidget(firstOutput, 1); firstBot->addWidget(firstButton); firstLayout->addLayout(firstTop); firstLayout->addLayout(firstBot);
    gridLayout->addWidget(firstCard, 1, 0);

    // --- Card 3: rotate (1행 1열) ---
    QWidget *rotateCard = new QWidget(rightContentWidget); rotateCard->setAttribute(Qt::WA_StyledBackground, true); rotateCard->setStyleSheet(cardStyle);
    QVBoxLayout *rotLayout = new QVBoxLayout(rotateCard); QHBoxLayout *rotTop = new QHBoxLayout();
    QLabel *rT = new QLabel("rotate()", rotateCard); rT->setStyleSheet(titleStyle);
    QLabel *rD = new QLabel("Move head to tail", rotateCard); rD->setStyleSheet(descStyle);
    QLabel *rC = new QLabel("O(1)", rotateCard); rC->setStyleSheet(compStyle);
    rotTop->addWidget(rT); rotTop->addWidget(rD); rotTop->addStretch(); rotTop->addWidget(rC);
    QHBoxLayout *rotBot = new QHBoxLayout(); rotateOutput = new QLabel("Ready", rotateCard); rotateOutput->setStyleSheet(outputStyle);
    rotateButton = new QPushButton("Rotate", rotateCard); rotateButton->setStyleSheet(btnStyle);
    rotBot->addWidget(rotateOutput, 1); rotBot->addWidget(rotateButton); rotLayout->addLayout(rotTop); rotLayout->addLayout(rotBot);
    gridLayout->addWidget(rotateCard, 1, 1);

    // --- Card 4: is_empty (2행 0열) ---
    QWidget *isEmptyCard = new QWidget(rightContentWidget); isEmptyCard->setAttribute(Qt::WA_StyledBackground, true); isEmptyCard->setStyleSheet(cardStyle);
    QVBoxLayout *isEmptyLayout = new QVBoxLayout(isEmptyCard); QHBoxLayout *isEmptyTop = new QHBoxLayout();
    QLabel *ieT = new QLabel("is_empty()", isEmptyCard); ieT->setStyleSheet(titleStyle);
    QLabel *ieD = new QLabel("Check emptiness", isEmptyCard); ieD->setStyleSheet(descStyle);
    QLabel *ieC = new QLabel("O(1)", isEmptyCard); ieC->setStyleSheet(compStyle);
    isEmptyTop->addWidget(ieT); isEmptyTop->addWidget(ieD); isEmptyTop->addStretch(); isEmptyTop->addWidget(ieC);
    QHBoxLayout *isEmptyBot = new QHBoxLayout(); isEmptyOutput = new QLabel("Ready", isEmptyCard); isEmptyOutput->setStyleSheet(outputStyle);
    isEmptyButton = new QPushButton("Check", isEmptyCard); isEmptyButton->setStyleSheet(btnStyle);
    isEmptyBot->addWidget(isEmptyOutput, 1); isEmptyBot->addWidget(isEmptyButton); isEmptyLayout->addLayout(isEmptyTop); isEmptyLayout->addLayout(isEmptyBot);
    gridLayout->addWidget(isEmptyCard, 2, 0);

    // --- Card 5: len (2행 1열) ---
    QWidget *lenCard = new QWidget(rightContentWidget); lenCard->setAttribute(Qt::WA_StyledBackground, true); lenCard->setStyleSheet(cardStyle);
    QVBoxLayout *lenLayout = new QVBoxLayout(lenCard); QHBoxLayout *lenTop = new QHBoxLayout();
    QLabel *lenT = new QLabel("len()", lenCard); lenT->setStyleSheet(titleStyle);
    QLabel *lenD = new QLabel("Get current size", lenCard); lenD->setStyleSheet(descStyle);
    QLabel *lenC = new QLabel("O(1)", lenCard); lenC->setStyleSheet(compStyle);
    lenTop->addWidget(lenT); lenTop->addWidget(lenD); lenTop->addStretch(); lenTop->addWidget(lenC);
    QHBoxLayout *lenBot = new QHBoxLayout(); lenOutput = new QLabel("Ready", lenCard); lenOutput->setStyleSheet(outputStyle);
    lenButton = new QPushButton("Size", lenCard); lenButton->setStyleSheet(btnStyle);
    lenBot->addWidget(lenOutput, 1); lenBot->addWidget(lenButton); lenLayout->addLayout(lenTop); lenLayout->addLayout(lenBot);
    gridLayout->addWidget(lenCard, 2, 1);

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
    // 3. 레이아웃 최종 조립 및 이벤트 연결
    // ==========================================
    mainLayout->addWidget(leftWidget, 1);
    mainLayout->addWidget(rightScrollArea);

    QObject::connect(enqueueButton, &QPushButton::clicked, [=]() { this->enqueue(); });
    QObject::connect(dequeueButton, &QPushButton::clicked, [=]() { this->dequeue(); });
    QObject::connect(firstButton, &QPushButton::clicked, [=]() { this->first(); });
    QObject::connect(isEmptyButton, &QPushButton::clicked, [=]() { this->is_empty(); });
    QObject::connect(lenButton, &QPushButton::clicked, [=]() { this->_len(); });
    QObject::connect(rotateButton, &QPushButton::clicked, [=]() { this->rotate(); });

    this->update();

    return containerWidget;
}

// ==========================================
// 4. 동적 크기 확장 및 원형 배치 업데이트 구현
// ==========================================
void CIRCULAR_LINKED::update(){
    // [청소] 컨테이너 내부의 동적 노드들을 전부 비워줍니다.
    QList<QWidget*> oldCells = stackContainer->findChildren<QWidget*>();
    for (QWidget* oldCell : oldCells) {
        delete oldCell;
    }

    if (tail != nullptr) {
        Node* head = tail->next;
        Node* current = head;
        int idx = 0;

        // 타 모델 모듈들과 일관성 유지를 위한 노드 정규화 데이터 바인딩
        int cellW = 110;
        int cellH = 65;
        int minDistance = 140;
        int radius = 135;

        if (size > 1) {
            double calculatedRadius = minDistance / (2.0 * std::sin(3.1415926535 / size));
            if (calculatedRadius > radius) {
                radius = static_cast<int>(calculatedRadius);
            }
        }

        int padding = cellW + 60;
        int containerSize = (radius * 2) + padding;

        stackContainer->setFixedSize(containerSize, containerSize);
        stackContainer->setStyleSheet(
            QString("background-color: #11141A; border-radius: %1px; border: 1px dashed #30363D;")
                .arg(containerSize / 2)
            );

        int centerX = containerSize / 2;
        int centerY = containerSize / 2;

        // --- 삼각함수 좌표 계산 기반 순회 배치 ---
        do {
            double angle = (2.0 * 3.1415926535 * idx) / size - (3.1415926535 / 2.0);

            int x = centerX + static_cast<int>(radius * std::cos(angle)) - (cellW / 2);
            int y = centerY + static_cast<int>(radius * std::sin(angle)) - (cellH / 2);

            QLabel* cell = new QLabel(stackContainer);
            cell->setFixedSize(cellW, cellH);
            cell->move(x, y);
            cell->setAlignment(Qt::AlignCenter);
            cell->setTextFormat(Qt::RichText);

            // 포인터 태그 식별자 컬러 스타일링 통일
            QString tag = "";
            if (current == head && current == tail) tag = "<br><span style='font-size: 8pt; color: #FF7B72;'><b>(Head/Tail)</b></span>";
            else if (current == head) tag = "<br><span style='font-size: 8pt; color: #79C0FF;'><b>(Head)</b></span>";
            else if (current == tail) tag = "<br><span style='font-size: 8pt; color: #FF7B72;'><b>(Tail)</b></span>";

            cell->setText(QString("Data<br><span style='font-size: 14pt;'>%1</span>%2")
                              .arg(current->data)
                              .arg(tag));

            cell->setStyleSheet(
                "QLabel {"
                "   border-radius: 8px;"
                "   background-color: #D29922;"
                "   color: #0D1117;"
                "   font-weight: bold;"
                "}"
                );

            cell->show();

            current = current->next;
            idx++;
        } while (current != head);
    }
    else {
        stackContainer->setFixedSize(420, 420);
        stackContainer->setStyleSheet("background-color: #11141A; border-radius: 210px; border: 1px dashed #30363D;");

        QLabel* emptyCell = new QLabel(stackContainer);
        emptyCell->setFixedSize(110, 65);
        emptyCell->move(210 - 55, 210 - 32);
        emptyCell->setAlignment(Qt::AlignCenter);
        emptyCell->setTextFormat(Qt::RichText);
        emptyCell->setText("[ 0 ]<br><span style='font-size: 9pt;'>Empty</span>");

        emptyCell->setStyleSheet(
            "QLabel {"
            "   border: 2px dashed #30363D;"
            "   border-radius: 8px;"
            "   background-color: transparent;"
            "   color: #8B949E;"
            "   font-weight: bold;"
            "}"
            );
        emptyCell->show();
    }
}

// ==========================================
// 5. 비즈니스 로직 함수군 (터미널 문장 규칙 완벽 통일)
// ==========================================
void CIRCULAR_LINKED::enqueue(){
    QString text = enqueueInput->text().trimmed();
    if (text.isEmpty()){
        statusOutput->setText(">> Error: Cannot enqueue an empty string.");
        return;
    }

    Node* newNode = new Node();
    newNode->data = text;

    if (tail == nullptr) {
        newNode->next = newNode;
        tail = newNode;
    } else {
        newNode->next = tail->next;
        tail->next = newNode;
        tail = newNode;
    }

    size++;
    enqueueInput->clear();
    statusOutput->setText(">> Success: Enqueued value '" + text + "' to Tail Node.");
    update();
}

void CIRCULAR_LINKED::dequeue(){
    if (tail == nullptr) {
        statusOutput->setText(">> Error: Queue Underflow! No nodes left to dequeue.");
        return;
    }

    Node* head = tail->next;
    QString val = head->data;

    if (tail == head) {
        delete head;
        tail = nullptr;
    } else {
        tail->next = head->next;
        delete head;
    }

    size--;
    dequeueOutput->setText(val);
    statusOutput->setText(">> Success: Dequeued front element '" + val + "' from Head Node.");
    update();
}

void CIRCULAR_LINKED::first(){
    if (tail == nullptr) {
        statusOutput->setText(">> Warning: Queue is empty. Head is undefined.");
        firstOutput->setText("Empty");
        return;
    }
    firstOutput->setText(tail->next->data);
    statusOutput->setText(">> Peek: Current front (head) value is '" + tail->next->data + "'.");
}

void CIRCULAR_LINKED::is_empty(){
    if (tail == nullptr) {
        isEmptyOutput->setText("True");
        statusOutput->setText(">> Inspection: is_empty() -> TRUE (Circular Queue is currently empty).");
    } else {
        isEmptyOutput->setText("False");
        statusOutput->setText(">> Inspection: is_empty() -> FALSE (Circular Queue has " + QString::number(size) + " node(s)).");
    }
}

void CIRCULAR_LINKED::_len(){
    lenOutput->setText(QString::number(size) + " Node(s)");
    statusOutput->setText(">> Inspection: len() -> Circular Queue ring size is currently " + QString::number(size) + ".");
}

void CIRCULAR_LINKED::rotate() {
    if (tail == nullptr || tail == tail->next) {
        rotateOutput->setText("Skipped");
        statusOutput->setText(">> Warning: Strategy skipped. 0 or 1 element cannot be rotated.");
        return;
    }

    QString rotatedVal = tail->next->data;
    tail = tail->next;

    rotateOutput->setText(rotatedVal);
    statusOutput->setText(">> Success: Rotated topology ring. Old head '" + rotatedVal + "' became the new tail.");
    update();
}