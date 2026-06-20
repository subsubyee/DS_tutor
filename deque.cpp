#include "deque.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QScrollBar>

// ==========================================
// 1. DEQUE 구조체 생성자 & 소멸자
// ==========================================
DEQUE::DEQUE(int _max, QMainWindow* _parent){
    max = _max;
    parent = _parent;
}

DEQUE::~DEQUE() {
    delete[] list;
    delete[] dequeCells;
}

// ==========================================
// 2. UI 레이아웃 세팅 (모던 다크 카드 뷰 & 인덱스 바닥 고정형)
// ==========================================
QWidget* DEQUE::setting(){
    list = new QString[max];
    str = 0;
    fin = 0;
    size = 0;
    dequeCells = new QLabel*[max];

    // --- 메인 컨테이너 및 전역 배경 설정 (Modern Obsidian Dark Theme) ---
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet(
        "QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }"
        );

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION (고정형 덱 슬롯 레이아웃)
    // ==========================================
    QWidget *leftWidget = new QWidget(containerWidget);
    QVBoxLayout *leftVLayout = new QVBoxLayout(leftWidget);
    leftVLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nFixed Array Deque Slots", leftWidget);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 8px;");
    leftVLayout->addWidget(visTitle);

    // 덱 슬롯들이 넘칠 때를 대비한 부드러운 스크롤 영역
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

    QWidget *dequeContainer = new QWidget();
    dequeContainer->setStyleSheet("background-color: #11141A; border-radius: 12px; border: 1px solid #21262D;");

    QVBoxLayout *dequeLayout = new QVBoxLayout(dequeContainer);
    dequeLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    dequeLayout->setContentsMargins(20, 20, 20, 20);
    dequeLayout->setSpacing(10); // 슬롯 간 컴팩트한 간격 조정

    // [역순 배치] index 0번 슬롯이 바닥(가장 아래)에 보이도록 루프 설계
    for (int i = max - 1; i >= 0; --i) {
        dequeCells[i] = new QLabel(dequeContainer);
        dequeCells[i]->setAlignment(Qt::AlignCenter);
        dequeCells[i]->setFixedSize(180, 70); // 양식에 맞춘 180 x 70 해상도 고정
        dequeLayout->addWidget(dequeCells[i]);
    }

    scrollArea->setWidget(dequeContainer);
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

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nDeque Operations Dashboard", rightContentWidget);
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

    // --- Card 0: add_first ---
    QWidget *afCard = new QWidget(rightContentWidget); afCard->setAttribute(Qt::WA_StyledBackground, true); afCard->setStyleSheet(cardStyle);
    QVBoxLayout *afL = new QVBoxLayout(afCard); QHBoxLayout *afTop = new QHBoxLayout();
    QLabel *afTitleText = new QLabel("add_first()", afCard); afTitleText->setStyleSheet(titleStyle);
    QLabel *afDesc = new QLabel("Insert at front", afCard); afDesc->setStyleSheet(descStyle);
    QLabel *afComp = new QLabel("O(1)", afCard); afComp->setStyleSheet(compStyle);
    afTop->addWidget(afTitleText); afTop->addWidget(afDesc); afTop->addStretch(); afTop->addWidget(afComp);
    QHBoxLayout *afBot = new QHBoxLayout(); afInput = new QLineEdit(afCard); afInput->setPlaceholderText("Enter value..."); afInput->setStyleSheet(inputStyle);
    afButton = new QPushButton("Run", afCard); afButton->setStyleSheet(btnStyle);
    afBot->addWidget(afInput); afBot->addWidget(afButton); afL->addLayout(afTop); afL->addLayout(afBot);
    gridLayout->addWidget(afCard, 0, 0);

    // --- Card 1: add_last ---
    QWidget *alCard = new QWidget(rightContentWidget); alCard->setAttribute(Qt::WA_StyledBackground, true); alCard->setStyleSheet(cardStyle);
    QVBoxLayout *alL = new QVBoxLayout(alCard); QHBoxLayout *alTop = new QHBoxLayout();
    QLabel *alTitleText = new QLabel("add_last()", alCard); alTitleText->setStyleSheet(titleStyle);
    QLabel *alDesc = new QLabel("Insert at rear", alCard); alDesc->setStyleSheet(descStyle);
    QLabel *alComp = new QLabel("O(1)", alCard); alComp->setStyleSheet(compStyle);
    alTop->addWidget(alTitleText); alTop->addWidget(alDesc); alTop->addStretch(); alTop->addWidget(alComp);
    QHBoxLayout *alBot = new QHBoxLayout(); alInput = new QLineEdit(alCard); alInput->setPlaceholderText("Enter value..."); alInput->setStyleSheet(inputStyle);
    alButton = new QPushButton("Run", alCard); alButton->setStyleSheet(btnStyle);
    alBot->addWidget(alInput); alBot->addWidget(alButton); alL->addLayout(alTop); alL->addLayout(alBot);
    gridLayout->addWidget(alCard, 0, 1);

    // --- Card 2: delete_first ---
    QWidget *dfCard = new QWidget(rightContentWidget); dfCard->setAttribute(Qt::WA_StyledBackground, true); dfCard->setStyleSheet(cardStyle);
    QVBoxLayout *dfL = new QVBoxLayout(dfCard); QHBoxLayout *dfTop = new QHBoxLayout();
    QLabel *dfTitleText = new QLabel("delete_first()", dfCard); dfTitleText->setStyleSheet(titleStyle);
    QLabel *dfDescText = new QLabel("Remove from front", dfCard); dfDescText->setStyleSheet(descStyle);
    QLabel *dfComp = new QLabel("O(1)", dfCard); dfComp->setStyleSheet(compStyle);
    dfTop->addWidget(dfTitleText); dfTop->addWidget(dfDescText); dfTop->addStretch(); dfTop->addWidget(dfComp);
    QHBoxLayout *dfBot = new QHBoxLayout(); dfOutput = new QLabel("Ready", dfCard); dfOutput->setStyleSheet(outputStyle);
    dfButton = new QPushButton("Run", dfCard); dfButton->setStyleSheet(btnStyle);
    dfBot->addWidget(dfOutput, 1); dfBot->addWidget(dfButton); dfL->addLayout(dfTop); dfL->addLayout(dfBot);
    gridLayout->addWidget(dfCard, 1, 0);

    // --- Card 3: delete_last ---
    QWidget *dlCard = new QWidget(rightContentWidget); dlCard->setAttribute(Qt::WA_StyledBackground, true); dlCard->setStyleSheet(cardStyle);
    QVBoxLayout *dlL = new QVBoxLayout(dlCard); QHBoxLayout *dlTop = new QHBoxLayout();
    QLabel *dlTitleText = new QLabel("delete_last()", dlCard); dlTitleText->setStyleSheet(titleStyle);
    QLabel *dlDescText = new QLabel("Remove from rear", dlCard); dlDescText->setStyleSheet(descStyle);
    QLabel *dlComp = new QLabel("O(1)", dlCard); dlComp->setStyleSheet(compStyle);
    dlTop->addWidget(dlTitleText); dlTop->addWidget(dlDescText); dlTop->addStretch(); dlTop->addWidget(dlComp);
    QHBoxLayout *dlBot = new QHBoxLayout(); dlOutput = new QLabel("Ready", dlCard); dlOutput->setStyleSheet(outputStyle);
    dlButton = new QPushButton("Run", dlCard); dlButton->setStyleSheet(btnStyle);
    dlBot->addWidget(dlOutput, 1); dlBot->addWidget(dlButton); dlL->addLayout(dlTop); dlL->addLayout(dlBot);
    gridLayout->addWidget(dlCard, 1, 1);

    // --- Card 4: first ---
    QWidget *fCard = new QWidget(rightContentWidget); fCard->setAttribute(Qt::WA_StyledBackground, true); fCard->setStyleSheet(cardStyle);
    QVBoxLayout *fL = new QVBoxLayout(fCard); QHBoxLayout *fTop = new QHBoxLayout();
    QLabel *fTitleText = new QLabel("first()", fCard); fTitleText->setStyleSheet(titleStyle);
    QLabel *fDescText = new QLabel("Peek front element", fCard); fDescText->setStyleSheet(descStyle);
    QLabel *fComp = new QLabel("O(1)", fCard); fComp->setStyleSheet(compStyle);
    fTop->addWidget(fTitleText); fTop->addWidget(fDescText); fTop->addStretch(); fTop->addWidget(fComp);
    QHBoxLayout *fBot = new QHBoxLayout(); fOutput = new QLabel("Ready", fCard); fOutput->setStyleSheet(outputStyle);
    fButton = new QPushButton("Run", fCard); fButton->setStyleSheet(btnStyle);
    fBot->addWidget(fOutput, 1); fBot->addWidget(fButton); fL->addLayout(fTop); fL->addLayout(fBot);
    gridLayout->addWidget(fCard, 2, 0);

    // --- Card 5: last ---
    QWidget *lCard = new QWidget(rightContentWidget); lCard->setAttribute(Qt::WA_StyledBackground, true); lCard->setStyleSheet(cardStyle);
    QVBoxLayout *lL = new QVBoxLayout(lCard); QHBoxLayout *lTop = new QHBoxLayout();
    QLabel *lTitleText = new QLabel("last()", lCard); lTitleText->setStyleSheet(titleStyle);
    QLabel *lDescText = new QLabel("Peek rear element", lCard); lDescText->setStyleSheet(descStyle);
    QLabel *lComp = new QLabel("O(1)", lCard); lComp->setStyleSheet(compStyle);
    lTop->addWidget(lTitleText); lTop->addWidget(lDescText); lTop->addStretch(); lTop->addWidget(lComp);
    QHBoxLayout *lBot = new QHBoxLayout(); lOutput = new QLabel("Ready", lCard); lOutput->setStyleSheet(outputStyle);
    lButton = new QPushButton("Run", lCard); lButton->setStyleSheet(btnStyle);
    lBot->addWidget(lOutput, 1); lBot->addWidget(lButton); lL->addLayout(lTop); lL->addLayout(lBot);
    gridLayout->addWidget(lCard, 2, 1);

    // --- Card 6: is_empty ---
    QWidget *emptyCard = new QWidget(rightContentWidget); emptyCard->setAttribute(Qt::WA_StyledBackground, true); emptyCard->setStyleSheet(cardStyle);
    QVBoxLayout *emptyL = new QVBoxLayout(emptyCard); QHBoxLayout *emptyTop = new QHBoxLayout();
    QLabel *eTitle = new QLabel("is_empty()", emptyCard); eTitle->setStyleSheet(titleStyle);
    QLabel *eDesc = new QLabel("Check emptiness", emptyCard); eDesc->setStyleSheet(descStyle);
    QLabel *eComp = new QLabel("O(1)", emptyCard); eComp->setStyleSheet(compStyle);
    emptyTop->addWidget(eTitle); emptyTop->addWidget(eDesc); emptyTop->addStretch(); emptyTop->addWidget(eComp);
    QHBoxLayout *emptyBot = new QHBoxLayout(); isEmptyOutput = new QLabel("Ready", emptyCard); isEmptyOutput->setStyleSheet(outputStyle);
    isEmptyButton = new QPushButton("Run", emptyCard); isEmptyButton->setStyleSheet(btnStyle);
    emptyBot->addWidget(isEmptyOutput, 1); emptyBot->addWidget(isEmptyButton); emptyL->addLayout(emptyTop); emptyL->addLayout(emptyBot);
    gridLayout->addWidget(emptyCard, 3, 0);

    // --- Card 7: len ---
    QWidget *lenCard = new QWidget(rightContentWidget); lenCard->setAttribute(Qt::WA_StyledBackground, true); lenCard->setStyleSheet(cardStyle);
    QVBoxLayout *lenL = new QVBoxLayout(lenCard); QHBoxLayout *lenTop = new QHBoxLayout();
    QLabel *lTitle = new QLabel("len()", lenCard); lTitle->setStyleSheet(titleStyle);
    QLabel *lDesc = new QLabel("Get deque size", lenCard); lDesc->setStyleSheet(descStyle);
    lComp = new QLabel("O(1)", lenCard); lComp->setStyleSheet(compStyle);
    lenTop->addWidget(lTitle); lenTop->addWidget(lDesc); lenTop->addStretch(); lenTop->addWidget(lComp);
    QHBoxLayout *lenBot = new QHBoxLayout(); lenOutput = new QLabel("Ready", lenCard); lenOutput->setStyleSheet(outputStyle);
    lenButton = new QPushButton("Run", lenCard); lenButton->setStyleSheet(btnStyle);
    lenBot->addWidget(lenOutput, 1); lenBot->addWidget(lenButton); lenL->addLayout(lenTop); lenL->addLayout(lenBot);
    gridLayout->addWidget(lenCard, 3, 1);

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

    QObject::connect(afButton, &QPushButton::clicked, [=]() { this->add_first(); });
    QObject::connect(alButton, &QPushButton::clicked, [=]() { this->add_last(); });
    QObject::connect(dfButton, &QPushButton::clicked, [=]() { this->delete_first(); });
    QObject::connect(dlButton, &QPushButton::clicked, [=]() { this->delete_last(); });
    QObject::connect(fButton, &QPushButton::clicked, [=]() { this->first(); });
    QObject::connect(lButton, &QPushButton::clicked, [=]() { this->last(); });
    QObject::connect(isEmptyButton, &QPushButton::clicked, [=]() { this->is_empty(); });
    QObject::connect(lenButton, &QPushButton::clicked, [=]() { this->_len(); });

    this->update();

    return containerWidget;
}

// ==========================================
// 4. 시각화 셀 스타일 드로잉 제어
// ==========================================
void DEQUE::update(){
    for (int i = 0; i < max; ++i) {
        dequeCells[i]->setTextFormat(Qt::RichText);
        dequeCells[i]->setFixedSize(180, 70); // 모든 셀의 크기를 180 x 70으로 고정

        // 포인터 텍스트 포맷 생성
        QString pointerStr = "";
        if (str == fin) {
            if (i == str) pointerStr = "<br><span style='font-size: 10pt; color: #FF7B72;'><b>(Front, Rear)</b></span>";
        } else {
            if (i == str) pointerStr = "<br><span style='font-size: 10pt; color: #FF7B72;'><b>(Front)</b></span>";
            if (i == fin) pointerStr = "<br><span style='font-size: 10pt; color: #58A6FF;'><b>(Rear)</b></span>";
        }

        // Active node condition
        bool isDataPresent = false;
        if (size > 0) {
            if (str < fin) {
                isDataPresent = (i >= str && i < fin);
            } else if (str > fin) {
                isDataPresent = (i >= str || i < fin);
            } else {
                isDataPresent = true;
            }
        }

        if (isDataPresent) {
            // [통일] 데이터가 존재하는 노드
            dequeCells[i]->setText(
                QString("Data (Idx: %1)<br><span style='font-size: 16pt;'>%2</span>%3")
                    .arg(i)
                    .arg(list[i])
                    .arg(pointerStr)
                );

            dequeCells[i]->setStyleSheet(
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
            dequeCells[i]->setText(
                QString("[ %1 ]<br><span style='font-size: 10pt;'>Empty</span>%2")
                    .arg(i)
                    .arg(pointerStr)
                );

            dequeCells[i]->setStyleSheet(
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
void DEQUE::add_first(){
    QString text = afInput->text().trimmed();
    if (text.isEmpty()){
        statusOutput->setText(">> Error: Cannot insert an empty string.");
        return;
    }
    if (size == max){
        statusOutput->setText(">> Error: Deque Overflow! Dynamic limit reached (" + QString::number(max) + ").");
        return;
    }

    str = (str - 1 + max) % max;
    list[str] = text;
    size++;

    afInput->clear();
    statusOutput->setText(">> Success: Added value '" + text + "' to front at Slot [" + QString::number(str) + "].");
    update();
}

void DEQUE::add_last(){
    QString text = alInput->text().trimmed();
    if (text.isEmpty()){
        statusOutput->setText(">> Error: Cannot insert an empty string.");
        return;
    }
    if (size == max){
        statusOutput->setText(">> Error: Deque Overflow! Dynamic limit reached (" + QString::number(max) + ").");
        return;
    }

    list[fin] = text;
    fin = (fin + 1) % max;
    size++;

    alInput->clear();
    statusOutput->setText(">> Success: Added value '" + text + "' to rear at Slot [" + QString::number((fin - 1 + max) % max) + "].");
    update();
}

void DEQUE::delete_first(){
    if (size == 0) {
        statusOutput->setText(">> Error: Deque Underflow! No items left to remove.");
        return;
    }

    QString val = list[str];
    str = (str + 1) % max;
    size--;

    dfOutput->setText(val);
    statusOutput->setText(">> Success: Removed front element '" + val + "' from Slot [" + QString::number((str - 1 + max) % max) + "].");
    update();
}

void DEQUE::delete_last(){
    if (size == 0) {
        statusOutput->setText(">> Error: Deque Underflow! No items left to remove.");
        return;
    }

    fin = (fin - 1 + max) % max;
    QString val = list[fin];
    size--;

    dlOutput->setText(val);
    statusOutput->setText(">> Success: Removed rear element '" + val + "' from Slot [" + QString::number(fin) + "].");
    update();
}

void DEQUE::first(){
    if (size == 0) {
        statusOutput->setText(">> Warning: Deque is empty. First is undefined.");
        fOutput->setText("Empty");
        return;
    }
    fOutput->setText(list[str]);
    statusOutput->setText(">> Peek: Current front value is '" + list[str] + "' at Slot [" + QString::number(str) + "].");
}

void DEQUE::last(){
    if (size == 0) {
        statusOutput->setText(">> Warning: Deque is empty. Last is undefined.");
        lOutput->setText("Empty");
        return;
    }
    int lastIdx = (fin - 1 + max) % max;
    lOutput->setText(list[lastIdx]);
    statusOutput->setText(">> Peek: Current rear value is '" + list[lastIdx] + "' at Slot [" + QString::number(lastIdx) + "].");
}

void DEQUE::is_empty(){
    if (size == 0) {
        isEmptyOutput->setText("True");
        statusOutput->setText(">> Inspection: is_empty() -> TRUE (Deque is currently empty).");
    } else {
        isEmptyOutput->setText("False");
        statusOutput->setText(">> Inspection: is_empty() -> FALSE (Deque has " + QString::number(size) + " item(s)).");
    }
}

void DEQUE::_len(){
    lenOutput->setText(QString::number(size) + " / " + QString::number(max));
    statusOutput->setText(">> Inspection: len() -> Deque utilizes " + QString::number(size) + " out of " + QString::number(max) + " slots.");
}