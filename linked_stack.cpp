#include "linked_stack.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QScrollBar>

// ==========================================
// 1. LINKED_STACK 구조체 생성자 & 소멸자
// ==========================================
LINKED_STACK::LINKED_STACK(QMainWindow* _parent){
    parent = _parent;
}

LINKED_STACK::~LINKED_STACK() {
    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
}

// ==========================================
// 2. UI 레이아웃 세팅 (모던 다크 카드 뷰 & 링크드 탑 다운 스택)
// ==========================================
QWidget* LINKED_STACK::setting(){
    head = nullptr;
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
    // [Left Area] VISUALIZATION (동적 스택 + 스크롤)
    // ==========================================
    QWidget *leftWidget = new QWidget(containerWidget);
    QVBoxLayout *leftVLayout = new QVBoxLayout(leftWidget);
    leftVLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nDynamic Linked Stack Nodes", leftWidget);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 8px;");
    leftVLayout->addWidget(visTitle);

    // 노드가 늘어날 때 부드럽게 스크롤되도록 지원하는 영역
    QScrollArea *scrollArea = new QScrollArea(leftWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea { border: none; background-color: transparent; }"
        "QScrollBar:vertical { background: #161B22; width: 8px; margin-left: 2px; }"
        "QScrollBar::handle:vertical { background: #30363D; border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: #8B949E; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { background: none; height: 0px; }"
        );

    QWidget *stackContainer = new QWidget();
    stackContainer->setStyleSheet("background-color: #11141A; border-radius: 12px; border: 1px solid #21262D;");

    leftLayout = new QVBoxLayout(stackContainer);
    leftLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    leftLayout->setContentsMargins(20, 20, 20, 20);
    leftLayout->setSpacing(10); // 링크드 노드 간 간격 통일

    scrollArea->setWidget(stackContainer);
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

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nLinked Stack Operations Dashboard", rightContentWidget);
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

    // --- Card 0: push ---
    QWidget *pushCard = new QWidget(rightContentWidget); pushCard->setAttribute(Qt::WA_StyledBackground, true); pushCard->setStyleSheet(cardStyle);
    QVBoxLayout *pushL = new QVBoxLayout(pushCard); QHBoxLayout *pushTop = new QHBoxLayout();
    QLabel *pTitle = new QLabel("push()", pushCard); pTitle->setStyleSheet(titleStyle);
    QLabel *pDesc = new QLabel("Add to top", pushCard); pDesc->setStyleSheet(descStyle);
    QLabel *pComp = new QLabel("O(1)", pushCard); pComp->setStyleSheet(compStyle);
    pushTop->addWidget(pTitle); pushTop->addWidget(pDesc); pushTop->addStretch(); pushTop->addWidget(pComp);
    QHBoxLayout *pushBot = new QHBoxLayout(); pushInput = new QLineEdit(pushCard); pushInput->setPlaceholderText("Enter value..."); pushInput->setStyleSheet(inputStyle);
    pushButton = new QPushButton("Push", pushCard); pushButton->setStyleSheet(btnStyle);
    pushBot->addWidget(pushInput); pushBot->addWidget(pushButton); pushL->addLayout(pushTop); pushL->addLayout(pushBot);
    gridLayout->addWidget(pushCard, 0, 0);

    // --- Card 1: pop ---
    QWidget *popCard = new QWidget(rightContentWidget); popCard->setAttribute(Qt::WA_StyledBackground, true); popCard->setStyleSheet(cardStyle);
    QVBoxLayout *popL = new QVBoxLayout(popCard); QHBoxLayout *popTop = new QHBoxLayout();
    QLabel *poTitle = new QLabel("pop()", popCard); poTitle->setStyleSheet(titleStyle);
    QLabel *poDesc = new QLabel("Remove top", popCard); poDesc->setStyleSheet(descStyle);
    QLabel *poComp = new QLabel("O(1)", popCard); poComp->setStyleSheet(compStyle);
    popTop->addWidget(poTitle); popTop->addWidget(poDesc); popTop->addStretch(); popTop->addWidget(poComp);
    QHBoxLayout *popBot = new QHBoxLayout(); popOutput = new QLabel("Ready", popCard); popOutput->setStyleSheet(outputStyle);
    popButton = new QPushButton("Pop", popCard); popButton->setStyleSheet(btnStyle);
    popBot->addWidget(popOutput, 1); popBot->addWidget(popButton); popL->addLayout(popTop); popL->addLayout(popBot);
    gridLayout->addWidget(popCard, 0, 1);

    // --- Card 2: top ---
    QWidget *topCard = new QWidget(rightContentWidget); topCard->setAttribute(Qt::WA_StyledBackground, true); topCard->setStyleSheet(cardStyle);
    QVBoxLayout *topL = new QVBoxLayout(topCard); QHBoxLayout *topTop = new QHBoxLayout();
    QLabel *tTitle = new QLabel("top()", topCard); tTitle->setStyleSheet(titleStyle);
    QLabel *tDesc = new QLabel("Peek top element", topCard); tDesc->setStyleSheet(descStyle);
    QLabel *tComp = new QLabel("O(1)", topCard); tComp->setStyleSheet(compStyle);
    topTop->addWidget(tTitle); topTop->addWidget(tDesc); topTop->addStretch(); topTop->addWidget(tComp);
    QHBoxLayout *topBot = new QHBoxLayout(); topOutput = new QLabel("Ready", topCard); topOutput->setStyleSheet(outputStyle);
    topButton = new QPushButton("Top", topCard); topButton->setStyleSheet(btnStyle);
    topBot->addWidget(topOutput, 1); topBot->addWidget(topButton); topL->addLayout(topTop); topL->addLayout(topBot);
    gridLayout->addWidget(topCard, 1, 0);

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
    QLabel *lDesc = new QLabel("Get stack size", lenCard); lDesc->setStyleSheet(descStyle);
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

    QObject::connect(pushButton, &QPushButton::clicked, [=]() { this->push(); });
    QObject::connect(popButton, &QPushButton::clicked, [=]() { this->pop(); });
    QObject::connect(topButton, &QPushButton::clicked, [=]() { this->top(); });
    QObject::connect(isEmptyButton, &QPushButton::clicked, [=]() { this->is_empty(); });
    QObject::connect(lenButton, &QPushButton::clicked, [=]() { this->_len(); });

    this->update();

    return containerWidget;
}

// ==========================================
// 4. 시각화 셀 동적 생성 및 스타일링 제어
// ==========================================
void LINKED_STACK::update(){
    // [청소] 기존 뷰의 컴포넌트들을 완벽하게 초기화
    QLayoutItem *child;
    while ((child = leftLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    Node* current = head;
    int index = 0;

    // Head(Top) 노드부터 체인을 따라 역순으로 드로잉 배치
    while (current != nullptr) {
        QLabel* cell = new QLabel(leftLayout->parentWidget());
        cell->setAlignment(Qt::AlignCenter);
        cell->setFixedSize(180, 70); // 스택/큐/덱 표준 해상도 크기 매칭 통일
        cell->setTextFormat(Qt::RichText);

        // 탑 포인터 지시자 스타일링 통일
        QString pointerStr = (current == head) ? "<br><span style='font-size: 10pt; color: #FF7B72;'><b>(Top)</b></span>" : "";

        cell->setText(
            QString("Data (Node: %1)<br><span style='font-size: 16pt;'>%2</span>%3")
                .arg(index++)
                .arg(current->data)
                .arg(pointerStr)
            );

        cell->setStyleSheet(
            "QLabel {"
            "   border-radius: 8px;"
            "   background-color: #D29922;"
            "   color: #0D1117;"
            "   font-weight: bold;"
            "}"
            );

        leftLayout->addWidget(cell);
        current = current->next;
    }

    // 완전히 비어 있는 고유 상태 뷰
    if (size == 0) {
        QLabel* emptyCell = new QLabel(leftLayout->parentWidget());
        emptyCell->setAlignment(Qt::AlignCenter);
        emptyCell->setFixedSize(180, 70);
        emptyCell->setTextFormat(Qt::RichText);
        emptyCell->setText("[ 0 ]<br><span style='font-size: 10pt;'>Empty</span>");

        emptyCell->setStyleSheet(
            "QLabel {"
            "   border: 2px dashed #30363D;"
            "   border-radius: 8px;"
            "   background-color: transparent;"
            "   color: #8B949E;"
            "   font-weight: bold;"
            "}"
            );
        leftLayout->addWidget(emptyCell);
    }
}

// ==========================================
// 5. 비즈니스 로직 함수군 (터미널 문장 규칙 완벽 통일)
// ==========================================
void LINKED_STACK::push(){
    QString text = pushInput->text().trimmed();
    if (text.isEmpty()){
        statusOutput->setText(">> Error: Cannot push an empty string.");
        return;
    }

    Node* newNode = new Node();
    newNode->data = text;
    newNode->next = head;
    head = newNode;
    size++;

    pushInput->clear();
    statusOutput->setText(">> Success: Pushed value '" + text + "' to Top Node.");
    update();
}

void LINKED_STACK::pop(){
    if (head == nullptr) {
        statusOutput->setText(">> Error: Stack Underflow! No nodes left to pop.");
        return;
    }

    Node* temp = head;
    QString val = temp->data;
    head = head->next;
    delete temp;
    size--;

    popOutput->setText(val);
    statusOutput->setText(">> Success: Popped top element '" + val + "' from Stack chain.");
    update();
}

void LINKED_STACK::top(){
    if (head == nullptr) {
        statusOutput->setText(">> Warning: Stack is empty. Top is undefined.");
        topOutput->setText("Empty");
        return;
    }
    topOutput->setText(head->data);
    statusOutput->setText(">> Peek: Current top value is '" + head->data + "'.");
}

void LINKED_STACK::is_empty(){
    if (head == nullptr) {
        isEmptyOutput->setText("True");
        statusOutput->setText(">> Inspection: is_empty() -> TRUE (Linked Stack is currently empty).");
    } else {
        isEmptyOutput->setText("False");
        statusOutput->setText(">> Inspection: is_empty() -> FALSE (Linked Stack has " + QString::number(size) + " node(s)).");
    }
}

void LINKED_STACK::_len(){
    lenOutput->setText(QString::number(size) + " Node(s)");
    statusOutput->setText(">> Inspection: len() -> Stack dynamic footprint size is currently " + QString::number(size) + ".");
}