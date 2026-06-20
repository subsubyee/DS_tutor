#include "doubly_linked.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>

// ==========================================
// 1. DOUBLY_LINKED 생성자 및 소멸자 구현
// ==========================================
DOUBLY_LINKED::DOUBLY_LINKED(QMainWindow* _parent) : QObject(_parent) {
    parent = _parent;
}

DOUBLY_LINKED::~DOUBLY_LINKED() {
    Double_Node* curr = header;
    while (curr) {
        Double_Node* nextNode = curr->next;
        delete curr;
        curr = nextNode;
    }
}

// ==========================================
// 2. UI 레이아웃 세팅 (모던 옥시디언 다크 테마 반영)
// ==========================================
QWidget* DOUBLY_LINKED::setting(){
    size = 0;
    currentState = IDLE;

    // 더미 노드 초기화
    header = new Double_Node();
    trailer = new Double_Node();
    header->data = "HEADER";  trailer->data = "TRAILER";
    header->prev = nullptr;   header->next = trailer;
    trailer->prev = header;   trailer->next = nullptr;

    // --- 메인 컨테이너 및 전역 배경 설정 (Modern Obsidian Dark Theme) ---
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet(
        "QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }"
        );

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION (모던 스크롤 뷰 컴포넌트)
    // ==========================================
    leftGroup = new QGroupBox(containerWidget);
    leftGroup->setStyleSheet("QGroupBox { border: none; background: transparent; }");

    QVBoxLayout *groupLayout = new QVBoxLayout(leftGroup);
    groupLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nDoubly Linked List Topology", leftGroup);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 14px;");
    groupLayout->addWidget(visTitle);

    QScrollArea *scrollArea = new QScrollArea(leftGroup);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(
        "QScrollArea { border: none; background-color: transparent; }"
        "QScrollBar:vertical { background: #161B22; width: 8px; }"
        "QScrollBar::handle:vertical { background: #30363D; border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: #8B949E; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { background: none; }"
        );

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background-color: #11141A; border-radius: 12px; border: 1px solid #21262D;");

    leftLayout = new QVBoxLayout(scrollContent);
    leftLayout->setContentsMargins(20, 20, 20, 20);
    leftLayout->setSpacing(10);
    leftLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    scrollArea->setWidget(scrollContent);
    groupLayout->addWidget(scrollArea, 1);

    // ==========================================
    // [Right Area] CONTROLS (그리드 카드 대시보드 - 우측 짤림 방지 스크롤)
    // ==========================================
    QScrollArea *rightScrollArea = new QScrollArea(containerWidget);
    rightScrollArea->setWidgetResizable(true);
    rightScrollArea->setFixedWidth(540); // 타 컴포넌트와 사이드 패널 규격 고정 통일
    rightScrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *rightContentWidget = new QWidget();
    rightContentWidget->setObjectName("RightContent");
    rightContentWidget->setStyleSheet("QWidget#RightContent { background: transparent; }");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightContentWidget);
    rightLayout->setContentsMargins(0, 0, 8, 0);
    rightLayout->setSpacing(12);

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nDoubly Linked List Operations Dashboard", rightContentWidget);
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
    QString triggerStyle= "QPushButton { background-color: #A371F7; color: #FFFFFF; border-radius: 6px; padding: 6px 16px; font-weight: bold; border: none; }"
                           "QPushButton:hover { background-color: #B48EFA; }"
                           "QPushButton:pressed { background-color: #8957E5; }";
    QString outputStyle = "QLabel { color: #EC5990; font-size: 12px; font-weight: bold; background-color: #0D1117; border: 1px solid #21262D; border-radius: 6px; padding: 6px 10px; }";

    // --- Card 0: insert_first (0행 0열) ---
    QWidget *ifCard = new QWidget(rightContentWidget); ifCard->setAttribute(Qt::WA_StyledBackground, true); ifCard->setStyleSheet(cardStyle);
    QVBoxLayout *ifL = new QVBoxLayout(ifCard); QHBoxLayout *ifTop = new QHBoxLayout();
    QLabel *ifT = new QLabel("insert_first()", ifCard); ifT->setStyleSheet(titleStyle);
    QLabel *ifD = new QLabel("Insert front", ifCard); ifD->setStyleSheet(descStyle);
    QLabel *ifC = new QLabel("O(1)", ifCard); ifC->setStyleSheet(compStyle);
    ifTop->addWidget(ifT); ifTop->addWidget(ifD); ifTop->addStretch(); ifTop->addWidget(ifC);
    QHBoxLayout *ifBot = new QHBoxLayout(); ifInput = new QLineEdit(ifCard); ifInput->setPlaceholderText("Value..."); ifInput->setStyleSheet(inputStyle);
    ifButton = new QPushButton("Insert", ifCard); ifButton->setStyleSheet(btnStyle);
    ifBot->addWidget(ifInput); ifBot->addWidget(ifButton); ifL->addLayout(ifTop); ifL->addLayout(ifBot);
    gridLayout->addWidget(ifCard, 0, 0);

    // --- Card 1: insert_last (0행 1열) ---
    QWidget *ilCard = new QWidget(rightContentWidget); ilCard->setAttribute(Qt::WA_StyledBackground, true); ilCard->setStyleSheet(cardStyle);
    QVBoxLayout *ilL = new QVBoxLayout(ilCard); QHBoxLayout *ilTop = new QHBoxLayout();
    QLabel *ilT = new QLabel("insert_last()", ilCard); ilT->setStyleSheet(titleStyle);
    QLabel *ilD = new QLabel("Insert back", ilCard); ilD->setStyleSheet(descStyle);
    QLabel *ilC = new QLabel("O(1)", ilCard); ilC->setStyleSheet(compStyle);
    ilTop->addWidget(ilT); ilTop->addWidget(ilD); ilTop->addStretch(); ilTop->addWidget(ilC);
    QHBoxLayout *ilBot = new QHBoxLayout(); ilInput = new QLineEdit(ilCard); ilInput->setPlaceholderText("Value..."); ilInput->setStyleSheet(inputStyle);
    ilButton = new QPushButton("Insert", ilCard); ilButton->setStyleSheet(btnStyle);
    ilBot->addWidget(ilInput); ilBot->addWidget(ilButton); ilL->addLayout(ilTop); ilL->addLayout(ilBot);
    gridLayout->addWidget(ilCard, 0, 1);

    // --- Card 2: insert_between (1행 0열) ---
    QWidget *ibCard = new QWidget(rightContentWidget); ibCard->setAttribute(Qt::WA_StyledBackground, true); ibCard->setStyleSheet(cardStyle);
    QVBoxLayout *ibL = new QVBoxLayout(ibCard); QHBoxLayout *ibTop = new QHBoxLayout();
    QLabel *ibT = new QLabel("insert_between()", ibCard); ibT->setStyleSheet(titleStyle);
    QLabel *ibD = new QLabel("Insert after node", ibCard); ibD->setStyleSheet(descStyle);
    QLabel *ibC = new QLabel("O(1)", ibCard); ibC->setStyleSheet(compStyle);
    ibTop->addWidget(ibT); ibTop->addWidget(ibD); ibTop->addStretch(); ibTop->addWidget(ibC);
    QHBoxLayout *ibBot = new QHBoxLayout(); ibInput = new QLineEdit(ibCard); ibInput->setPlaceholderText("Value..."); ibInput->setStyleSheet(inputStyle);
    ibButton = new QPushButton("Trigger", ibCard); ibButton->setStyleSheet(triggerStyle);
    ibBot->addWidget(ibInput); ibBot->addWidget(ibButton); ibL->addLayout(ibTop); ibL->addLayout(ibBot);
    gridLayout->addWidget(ibCard, 1, 0);

    // --- Card 3: delete_node (1행 1열) ---
    QWidget *dnCard = new QWidget(rightContentWidget); dnCard->setAttribute(Qt::WA_StyledBackground, true); dnCard->setStyleSheet(cardStyle);
    QVBoxLayout *dnL = new QVBoxLayout(dnCard); QHBoxLayout *dnTop = new QHBoxLayout();
    QLabel *dnT = new QLabel("delete_node()", dnCard); dnT->setStyleSheet(titleStyle);
    QLabel *dnD = new QLabel("Remove selected node", dnCard); dnD->setStyleSheet(descStyle);
    QLabel *dnC = new QLabel("O(1)", dnCard); dnC->setStyleSheet(compStyle);
    dnTop->addWidget(dnT); dnTop->addWidget(dnD); dnTop->addStretch(); dnTop->addWidget(dnC);
    dnButton = new QPushButton("Trigger Node Click Mode", dnCard); dnButton->setStyleSheet(triggerStyle);
    dnL->addLayout(dnTop); dnL->addWidget(dnButton);
    gridLayout->addWidget(dnCard, 1, 1);

    // --- Card 4: delete_first (2행 0열) ---
    QWidget *dfCard = new QWidget(rightContentWidget); dfCard->setAttribute(Qt::WA_StyledBackground, true); dfCard->setStyleSheet(cardStyle);
    QVBoxLayout *dfL = new QVBoxLayout(dfCard); QHBoxLayout *dfTop = new QHBoxLayout();
    QLabel *dfT = new QLabel("delete_first()", dfCard); dfT->setStyleSheet(titleStyle);
    QLabel *dfD = new QLabel("Remove front element", dfCard); dfD->setStyleSheet(descStyle);
    QLabel *dfC = new QLabel("O(1)", dfCard); dfC->setStyleSheet(compStyle);
    dfTop->addWidget(dfT); dfTop->addWidget(dfD); dfTop->addStretch(); dfTop->addWidget(dfC);
    QHBoxLayout *dfBot = new QHBoxLayout(); dfOutput = new QLabel("Ready", dfCard); dfOutput->setStyleSheet(outputStyle);
    dfButton = new QPushButton("Delete", dfCard); dfButton->setStyleSheet(btnStyle);
    dfBot->addWidget(dfOutput, 1); dfBot->addWidget(dfButton); dfL->addLayout(dfTop); dfL->addLayout(dfBot);
    gridLayout->addWidget(dfCard, 2, 0);

    // --- Card 5: delete_last (2행 1열) ---
    QWidget *dlCard = new QWidget(rightContentWidget); dlCard->setAttribute(Qt::WA_StyledBackground, true); dlCard->setStyleSheet(cardStyle);
    QVBoxLayout *dlL = new QVBoxLayout(dlCard); QHBoxLayout *dlTop = new QHBoxLayout();
    QLabel *dlT = new QLabel("delete_last()", dlCard); dlT->setStyleSheet(titleStyle);
    QLabel *dlD = new QLabel("Remove back element", dlCard); dlD->setStyleSheet(descStyle);
    QLabel *dlC = new QLabel("O(1)", dlCard); dlC->setStyleSheet(compStyle);
    dlTop->addWidget(dlT); dlTop->addWidget(dlD); dlTop->addStretch(); dlTop->addWidget(dlC);
    QHBoxLayout *dlBot = new QHBoxLayout(); dlOutput = new QLabel("Ready", dlCard); dlOutput->setStyleSheet(outputStyle);
    dlButton = new QPushButton("Delete", dlCard); dlButton->setStyleSheet(btnStyle);
    dlBot->addWidget(dlOutput, 1); dlBot->addWidget(dlButton); dlL->addLayout(dlTop); dlL->addLayout(dlBot);
    gridLayout->addWidget(dlCard, 2, 1);

    // --- Card 6: first (3행 0열) ---
    QWidget *fCard = new QWidget(rightContentWidget); fCard->setAttribute(Qt::WA_StyledBackground, true); fCard->setStyleSheet(cardStyle);
    QVBoxLayout *fL = new QVBoxLayout(fCard); QHBoxLayout *fTop = new QHBoxLayout();
    QLabel *fT = new QLabel("first()", fCard); fT->setStyleSheet(titleStyle);
    QLabel *fD = new QLabel("Peek front value", fCard); fD->setStyleSheet(descStyle);
    QLabel *fC = new QLabel("O(1)", fCard); fC->setStyleSheet(compStyle);
    fTop->addWidget(fT); fTop->addWidget(fD); fTop->addStretch(); fTop->addWidget(fC);
    QHBoxLayout *fBot = new QHBoxLayout(); fOutput = new QLabel("Ready", fCard); fOutput->setStyleSheet(outputStyle);
    fButton = new QPushButton("Peek", fCard); fButton->setStyleSheet(btnStyle);
    fBot->addWidget(fOutput, 1); fBot->addWidget(fButton); fL->addLayout(fTop); fL->addLayout(fBot);
    gridLayout->addWidget(fCard, 3, 0);

    // --- Card 7: last (3행 1열) ---
    QWidget *lCard = new QWidget(rightContentWidget); lCard->setAttribute(Qt::WA_StyledBackground, true); lCard->setStyleSheet(cardStyle);
    QVBoxLayout *lL = new QVBoxLayout(lCard); QHBoxLayout *lTop = new QHBoxLayout();
    QLabel *lT = new QLabel("last()", lCard); lT->setStyleSheet(titleStyle);
    QLabel *lD = new QLabel("Peek back value", lCard); lD->setStyleSheet(descStyle);
    QLabel *lC = new QLabel("O(1)", lCard); lC->setStyleSheet(compStyle);
    lTop->addWidget(lT); lTop->addWidget(lD); lTop->addStretch(); lTop->addWidget(lC);
    QHBoxLayout *lBot = new QHBoxLayout(); lOutput = new QLabel("Ready", lCard); lOutput->setStyleSheet(outputStyle);
    lButton = new QPushButton("Peek", lCard); lButton->setStyleSheet(btnStyle);
    lBot->addWidget(lOutput, 1); lBot->addWidget(lButton); lL->addLayout(lTop); lL->addLayout(lBot);
    gridLayout->addWidget(lCard, 3, 1);

    // --- Card 8: is_empty (4행 0열) ---
    QWidget *ieCard = new QWidget(rightContentWidget); ieCard->setAttribute(Qt::WA_StyledBackground, true); ieCard->setStyleSheet(cardStyle);
    QVBoxLayout *ieL = new QVBoxLayout(ieCard); QHBoxLayout *ieTop = new QHBoxLayout();
    QLabel *ieT = new QLabel("is_empty()", ieCard); ieT->setStyleSheet(titleStyle);
    QLabel *ieD = new QLabel("Check emptiness", ieCard); ieD->setStyleSheet(descStyle);
    QLabel *ieC = new QLabel("O(1)", ieCard); ieC->setStyleSheet(compStyle);
    ieTop->addWidget(ieT); ieTop->addWidget(ieD); ieTop->addStretch(); ieTop->addWidget(ieC);
    QHBoxLayout *ieBot = new QHBoxLayout(); isEmptyOutput = new QLabel("Ready", ieCard); isEmptyOutput->setStyleSheet(outputStyle);
    isEmptyButton = new QPushButton("Check", ieCard); isEmptyButton->setStyleSheet(btnStyle);
    ieBot->addWidget(isEmptyOutput, 1); ieBot->addWidget(isEmptyButton); ieL->addLayout(ieTop); ieL->addLayout(ieBot);
    gridLayout->addWidget(ieCard, 4, 0);

    // --- Card 9: len (4행 1열) ---
    QWidget *lenCard = new QWidget(rightContentWidget); lenCard->setAttribute(Qt::WA_StyledBackground, true); lenCard->setStyleSheet(cardStyle);
    QVBoxLayout *lenL = new QVBoxLayout(lenCard); QHBoxLayout *lenTop = new QHBoxLayout();
    QLabel *lenT = new QLabel("len()", lenCard); lenT->setStyleSheet(titleStyle);
    QLabel *lenD = new QLabel("Get node count", lenCard); lenD->setStyleSheet(descStyle);
    QLabel *lenC = new QLabel("O(1)", lenCard); lenC->setStyleSheet(compStyle);
    lenTop->addWidget(lenT); lenTop->addWidget(lenD); lenTop->addStretch(); lenTop->addWidget(lenC);
    QHBoxLayout *lenBot = new QHBoxLayout(); lenOutput = new QLabel("Ready", lenCard); lenOutput->setStyleSheet(outputStyle);
    lenButton = new QPushButton("Size", lenCard); lenButton->setStyleSheet(btnStyle);
    lenBot->addWidget(lenOutput, 1); lenBot->addWidget(lenButton); lenL->addLayout(lenTop); lenL->addLayout(lenBot);
    gridLayout->addWidget(lenCard, 4, 1);

    rightLayout->addLayout(gridLayout);

    // --- Bottom: Terminal Log ---
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
    // 3. 레이아웃 조립 및 이벤트 연결
    // ==========================================
    mainLayout->addWidget(leftGroup, 1);
    mainLayout->addWidget(rightScrollArea);

    QObject::connect(ifButton, &QPushButton::clicked, [=]() { this->insert_first(); });
    QObject::connect(ilButton, &QPushButton::clicked, [=]() { this->insert_last(); });
    QObject::connect(ibButton, &QPushButton::clicked, [=]() { this->insert_between(); });
    QObject::connect(dnButton, &QPushButton::clicked, [=]() { this->delete_node(); });
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
// 3. 시각화 셀 업데이트 (STACK 스타일과 100% 동일)
// ==========================================
void DOUBLY_LINKED::update() {
    for (QLabel* cell : std::as_const(listCells)) {
        leftLayout->removeWidget(cell);
        cell->removeEventFilter(this);
        delete cell;
    }
    listCells.clear();
    cellNodes.clear();

    QList<Double_Node*> nodes;
    Double_Node* curr = header;
    while (curr) {
        nodes.append(curr);
        curr = curr->next;
    }

    for (int i = 0; i < nodes.size(); ++i) {
        Double_Node* node = nodes[i];
        QString colorStyle = "background-color: #D29922; color: #0D1117;";
        QString metaTag = "";

        // 더미 노드 식별자 배지 디자인 정교화
        if (node == header) {
            colorStyle = "background-color: #1F6FEB; color: #FFFFFF;";
            metaTag = "<br><span style='font-size: 8pt; color: #CDD6F4;'><b>(Header Dummy)</b></span>";
        } else if (node == trailer) {
            colorStyle = "background-color: #238636; color: #FFFFFF;";
            metaTag = "<br><span style='font-size: 8pt; color: #CDD6F4;'><b>(Trailer Dummy)</b></span>";
        }

        QLabel* newCell = new QLabel(leftGroup);
        newCell->setTextFormat(Qt::RichText);
        newCell->setText(QString("Node [ %1 ]<br><span style='font-size: 14pt;'>%2</span>%3")
                             .arg(QString::number(i))
                             .arg(node->data)
                             .arg(metaTag));

        newCell->setAlignment(Qt::AlignCenter);
        newCell->setFixedSize(180, 70); // STACK 규격과 완전 통일
        newCell->setCursor(Qt::PointingHandCursor);
        newCell->setStyleSheet(QString("QLabel { border-radius: 8px; font-weight: bold; %1 }").arg(colorStyle));

        newCell->installEventFilter(this);
        leftLayout->addWidget(newCell);

        listCells.append(newCell);
        cellNodes.append(node);
    }
}

bool DOUBLY_LINKED::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QLabel* clickedCell = qobject_cast<QLabel*>(obj);
        if (clickedCell) {
            int idx = listCells.indexOf(clickedCell);
            if (idx != -1) {
                handleBlockClick(cellNodes[idx]);
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

// ==========================================
// 4. 클릭 트리거 비즈니스 핸들러
// ==========================================
void DOUBLY_LINKED::handleBlockClick(Double_Node* targetNode) {
    if (currentState == IDLE) return;

    if (currentState == WAITING_INSERT_CLICK) {
        if (targetNode == trailer) {
            statusOutput->setText(">> Error: Strategy skipped. Cannot insert behind the Trailer Dummy.");
            currentState = IDLE;
            return;
        }

        QString text = ibInput->text().trimmed();
        Double_Node* newNode = new Double_Node();
        newNode->data = text;
        Double_Node* n = targetNode->next;

        newNode->prev = targetNode;
        newNode->next = n;
        targetNode->next = newNode;
        n->prev = newNode;

        size++;
        ibInput->clear();
        statusOutput->setText(">> Success: Node '" + text + "' inserted successfully after target.");
        currentState = IDLE;
        update();
    }
    else if (currentState == WAITING_DELETE_CLICK) {
        if (targetNode == header || targetNode == trailer) {
            statusOutput->setText(">> Error: Protected boundary node! Cannot delete Dummy elements.");
            currentState = IDLE;
            return;
        }

        QString targetData = targetNode->data;
        targetNode->prev->next = targetNode->next;
        targetNode->next->prev = targetNode->prev;

        delete targetNode;
        size--;

        statusOutput->setText(">> Success: Targeted node containing '" + targetData + "' has been deleted.");
        currentState = IDLE;
        update();
    }
}

// ==========================================
// 5. 기본 8가지 제어 연산 로직 구현 (로그 규칙 통일)
// ==========================================
void DOUBLY_LINKED::insert_first() {
    QString text = ifInput->text().trimmed();
    if (text.isEmpty()) { statusOutput->setText(">> Error: Cannot insert an empty string."); return; }

    Double_Node* newNode = new Double_Node();
    newNode->data = text;
    Double_Node* p = header;
    Double_Node* n = header->next;

    newNode->prev = p; newNode->next = n; p->next = newNode; n->prev = newNode;
    size++; ifInput->clear(); statusOutput->setText(">> Success: Inserted value '" + text + "' at front (after Header)."); update();
}

void DOUBLY_LINKED::insert_last() {
    QString text = ilInput->text().trimmed();
    if (text.isEmpty()) { statusOutput->setText(">> Error: Cannot insert an empty string."); return; }

    Double_Node* newNode = new Double_Node();
    newNode->data = text;
    Double_Node* p = trailer->prev;
    Double_Node* n = trailer;

    newNode->prev = p; newNode->next = n; p->next = newNode; n->prev = newNode;
    size++; ilInput->clear(); statusOutput->setText(">> Success: Inserted value '" + text + "' at back (before Trailer)."); update();
}

void DOUBLY_LINKED::insert_between() {
    QString text = ibInput->text().trimmed();
    if (text.isEmpty()) { statusOutput->setText(">> Error: Cannot insert an empty string."); return; }
    currentState = WAITING_INSERT_CLICK;
    statusOutput->setText(">> Mode: Click a target node in the viewport to insert '" + text + "' RIGHT AFTER it.");
}

void DOUBLY_LINKED::delete_node() {
    if (size == 0) { statusOutput->setText(">> Error: List Underflow! No dynamic nodes to delete."); return; }
    currentState = WAITING_DELETE_CLICK;
    statusOutput->setText(">> Mode: Click any dynamic data node in the viewport to target it for removal.");
}

void DOUBLY_LINKED::delete_first() {
    if (size == 0) { statusOutput->setText(">> Error: List Underflow! Nothing to remove."); return; }
    Double_Node* target = header->next; QString val = target->data;
    header->next = target->next; target->next->prev = header;
    delete target; size--;
    dfOutput->setText(val); statusOutput->setText(">> Success: Removed first element node containing '" + val + "'."); update();
}

void DOUBLY_LINKED::delete_last() {
    if (size == 0) { statusOutput->setText(">> Error: List Underflow! Nothing to remove."); return; }
    Double_Node* target = trailer->prev; QString val = target->data;
    target->prev->next = trailer; trailer->prev = target->prev;
    delete target; size--;
    dlOutput->setText(val); statusOutput->setText(">> Success: Removed last element node containing '" + val + "'."); update();
}

void DOUBLY_LINKED::first() {
    if (size == 0) { statusOutput->setText(">> Warning: List is empty. Front element is undefined."); fOutput->setText("Empty"); return; }
    fOutput->setText(header->next->data); statusOutput->setText(">> Peek: Current front value is '" + header->next->data + "'.");
}

void DOUBLY_LINKED::last() {
    if (size == 0) { statusOutput->setText(">> Warning: List is empty. Back element is undefined."); lOutput->setText("Empty"); return; }
    lOutput->setText(trailer->prev->data); statusOutput->setText(">> Peek: Current back value is '" + trailer->prev->data + "'.");
}

void DOUBLY_LINKED::is_empty() {
    if (size == 0) { isEmptyOutput->setText("True"); statusOutput->setText(">> Inspection: is_empty() -> TRUE (List has 0 data elements)."); }
    else { isEmptyOutput->setText("False"); statusOutput->setText(">> Inspection: is_empty() -> FALSE (List contains " + QString::number(size) + " data node(s))."); }
}

void DOUBLY_LINKED::_len() {
    lenOutput->setText(QString::number(size) + " Node(s)"); statusOutput->setText(">> Inspection: len() -> Current node count is " + QString::number(size) + " (excluding Dummies).");
}