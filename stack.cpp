#include "stack.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QScrollBar>

// ==========================================
// 1. STACK 구조체 생성자 & 소멸자
// ==========================================
STACK::STACK(int _max, QMainWindow* _parent){
    max = _max;
    parent = _parent;
}

STACK::~STACK() {
    delete[] list;
    delete[] stackCells;
}

// ==========================================
// 2. UI 레이아웃 세팅 (모던 다크 카드 뷰 & 인덱스 바닥 고정형)
// ==========================================
QWidget* STACK::setting(){
    list = new QString[max];
    len = 0;
    stackCells = new QLabel*[max];

    // --- 메인 컨테이너 및 전역 배경 설정 (Modern Obsidian Dark Theme) ---
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet(
        "QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }"
        );

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION (고정형 스택 슬롯 레이아웃)
    // ==========================================
    QWidget *leftWidget = new QWidget(containerWidget);
    QVBoxLayout *leftVLayout = new QVBoxLayout(leftWidget);
    leftVLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nFixed Array Stack Slots", leftWidget);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 8px;");
    leftVLayout->addWidget(visTitle);

    // 스택 슬롯들이 넘칠 때를 대비한 부드러운 스크롤 영역
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

    QWidget *stackContainer = new QWidget();
    stackContainer->setStyleSheet("background-color: #11141A; border-radius: 12px; border: 1px solid #21262D;");

    QVBoxLayout *stackLayout = new QVBoxLayout(stackContainer);
    stackLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    stackLayout->setContentsMargins(20, 20, 20, 20);
    stackLayout->setSpacing(10); // 슬롯 간 컴팩트한 간격 조정

    // [역순 배치] index 0번 슬롯이 바닥(가장 아래)에 보이도록 루프 설계
    for (int i = max - 1; i >= 0; --i) {
        stackCells[i] = new QLabel(stackContainer);
        stackCells[i]->setAlignment(Qt::AlignCenter);
        stackCells[i]->setFixedSize(290, 48); // 균형 잡힌 박스 비율 기본 고정
        stackLayout->addWidget(stackCells[i]);
    }

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

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nStack Operations Dashboard", rightContentWidget);
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
    QLabel *pDesc = new QLabel("Insert element", pushCard); pDesc->setStyleSheet(descStyle);
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
    QLabel *tDesc = new QLabel("Peek element", topCard); tDesc->setStyleSheet(descStyle);
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

    QObject::connect(pushButton, &QPushButton::clicked, [=]() { this->push(); });
    QObject::connect(popButton, &QPushButton::clicked, [=]() { this->pop(); });
    QObject::connect(topButton, &QPushButton::clicked, [=]() { this->top(); });
    QObject::connect(isEmptyButton, &QPushButton::clicked, [=]() { this->is_empty(); });
    QObject::connect(lenButton, &QPushButton::clicked, [=]() { this->_len(); });

    this->update();

    return containerWidget;
}

// ==========================================
// 4. 시각화 셀 스타일 드로잉 제어 (LINKED_STACK 크기 및 디자인 통일)
// ==========================================
void STACK::update(){
    for (int i = 0; i < max; ++i) {
        stackCells[i]->setTextFormat(Qt::RichText);

        // 모든 셀의 크기를 LINKED_STACK과 동일하게 180 x 70으로 고정
        stackCells[i]->setFixedSize(180, 70);

        if (i < len) {
            // [통일] 데이터가 존재하는 노드 (LINKED_STACK의 스타일 및 구조 적용)
            stackCells[i]->setText(
                QString("Data (Idx: %1)<br><span style='font-size: 16pt;'>%2</span>%3")
                    .arg(i)
                    .arg(list[i])
                    .arg((i == len - 1) ? "<br><span style='font-size: 10pt; color: #FF7B72;'><b>(Top)</b></span>" : "")
                );

            stackCells[i]->setStyleSheet(
                "QLabel {"
                "   border-radius: 8px;"
                "   background-color: #D29922;"
                "   color: #0D1117;"
                "   font-weight: bold;"
                "}"
                );
        }
        else {
            // [통일] 비어 있는 슬롯 (LINKED_STACK의 Empty 셀 스타일 적용)
            stackCells[i]->setText(QString("[ %1 ]<br><span style='font-size: 10pt;'>Empty</span>").arg(i));

            stackCells[i]->setStyleSheet(
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
// 5. 비즈니스 로직 함수군 (기존 기능 완벽 유지)
// ==========================================
void STACK::push(){
    QString text = pushInput->text().trimmed();
    if (text.isEmpty()){
        statusOutput->setText(">> Error: Cannot push an empty string.");
        return;
    }

    if (len == max){
        statusOutput->setText(">> Error: Stack Overflow! Dynamic limit reached (" + QString::number(max) + ").");
        return;
    }

    list[len] = text;
    len++;
    pushInput->clear();
    statusOutput->setText(">> Success: Pushed value '" + text + "' into Slot [" + QString::number(len-1) + "].");
    update();
}

void STACK::pop(){
    if (len == 0) {
        statusOutput->setText(">> Error: Stack Underflow! No items left to pop.");
        return;
    }

    len--;
    QString val = list[len];
    popOutput->setText(val);
    statusOutput->setText(">> Success: Popped element '" + val + "' from Slot [" + QString::number(len) + "].");
    update();
}

void STACK::top(){
    if (len == 0) {
        statusOutput->setText(">> Warning: Stack is empty. Top is undefined.");
        topOutput->setText("Empty");
        return;
    }
    topOutput->setText(list[len - 1]);
    statusOutput->setText(">> Peek: Current top value is '" + list[len - 1] + "' at Slot [" + QString::number(len-1) + "].");
}

void STACK::is_empty(){
    if (len == 0) {
        isEmptyOutput->setText("True");
        statusOutput->setText(">> Inspection: is_empty() -> TRUE (Stack is currently empty).");
    } else {
        isEmptyOutput->setText("False");
        statusOutput->setText(">> Inspection: is_empty() -> FALSE (Stack has " + QString::number(len) + " item(s)).");
    }
}

void STACK::_len(){
    lenOutput->setText(QString::number(len) + " / " + QString::number(max));
    statusOutput->setText(">> Inspection: len() -> Stack utilizes " + QString::number(len) + " out of " + QString::number(max) + " slots.");
}