#include "tree_avl.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <cmath>
#include <QGraphicsItem>
#include <QPainterPath>
#include <QGroupBox>

// ==========================================
// 1. UI 렌더링 기하학 상수정의 (사양 통일)
// ==========================================
static const double NODE_WIDTH = 70.0;
// 메타 뱃지 제거에 맞춰 퀴즈 모듈과 동일한 사양으로 노드 및 레벨 높이 매핑
static const double NODE_HEIGHT = 42.0;
static const double LEVEL_HEIGHT = 85.0;

TREE_AVL::TREE_AVL(QMainWindow* _parent) : QObject(_parent) {
    parent = _parent;
    root = nullptr;
    size = 0;
    highlightSearchKey = -1;
    highlightTargetKey = -1;
    highlightReplaceKey = -1;

    currentDeleteState = IDLE;
    currentInsertState = I_IDLE;
    currNode = nullptr;
    parentNode = nullptr;
    replaceNode = nullptr;
    replaceParent = nullptr;

    nodeA = nullptr;
    nodeB = nullptr;
    nodeC = nullptr;

    targetKey = -1;
    insertKey = -1;

    // 타이머 초기화 및 비동기 루프 통합 바인딩
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, [=]() {
        if (currentDeleteState != IDLE) {
            this->deleteStep();
        } else if (currentInsertState != I_IDLE) {
            this->insertStep();
        }
    });
}

TREE_AVL::~TREE_AVL() { clearTree(root); }

void TREE_AVL::clearTree(tree_avl* node) {
    if (node) { clearTree(node->left); clearTree(node->right); delete node; }
}

void TREE_AVL::setControlsEnabled(bool enabled) {
    insertButton->setEnabled(enabled);
    deleteButton->setEnabled(enabled);
    searchButton->setEnabled(enabled);
    minButton->setEnabled(enabled);
    maxButton->setEnabled(enabled);
    isEmptyButton->setEnabled(enabled);
    lenButton->setEnabled(enabled);
    speedSlider->setEnabled(enabled); // 애니메이션 도중 속도 조절 고정 (선택 사항)
}

// ==========================================
// 2. UI 레이아웃 세팅 (Modern Obsidian Dark Theme 완벽 반영)
// ==========================================
QWidget* TREE_AVL::setting() {
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet(
        "QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }"
        );
    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // [Left Area] VISUALIZATION
    leftGroup = new QGroupBox(containerWidget);
    leftGroup->setStyleSheet("QGroupBox { border: none; background: transparent; }");

    leftLayout = new QVBoxLayout(leftGroup);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nBalanced AVL Tree Topology", leftGroup);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 14px;");
    leftLayout->addWidget(visTitle);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, leftGroup);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QBrush(QColor("#11141A")));
    view->setStyleSheet("QGraphicsView { border: 1px solid #21262D; border-radius: 12px; background-color: #11141A; }");
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    leftLayout->addWidget(view);

    // [Right Area] CONTROLS
    QScrollArea *rightScrollArea = new QScrollArea(containerWidget);
    rightScrollArea->setWidgetResizable(true);
    rightScrollArea->setFixedWidth(420);
    rightScrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *rightContentWidget = new QWidget();
    rightContentWidget->setObjectName("RightContent");
    rightContentWidget->setStyleSheet("QWidget#RightContent { background: transparent; }");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightContentWidget);
    rightLayout->setContentsMargins(0, 0, 8, 0);
    rightLayout->setSpacing(12);

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nAVL Tree Operations Dashboard", rightContentWidget);
    controlsTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #8B949E; letter-spacing: 1px; margin-bottom: 4px;");
    rightLayout->addWidget(controlsTitle);

    QGridLayout *controlGrid = new QGridLayout();
    controlGrid->setSpacing(12);

    QString cardStyle   = "QWidget { background-color: #161B22; border: 1px solid #21262D; border-radius: 10px; }";
    QString titleStyle  = "color: #58A6FF; font-weight: bold; font-family: 'Consolas', monospace; font-size: 13px; background: transparent; border: none;";
    QString descStyle   = "color: #8B949E; font-size: 11px; background: transparent; border: none;";
    QString compStyle   = "color: #56B6C2; font-size: 11px; font-weight: bold; font-family: monospace; background: transparent; border: none;";
    QString inputStyle  = "QLineEdit { background-color: #0D1117; border: 1px solid #30363D; border-radius: 6px; color: #E6EDF3; padding: 6px 10px; selection-background-color: #238636; } QLineEdit:focus { border: 1px solid #58A6FF; }";
    QString btnStyle    = "QPushButton { background-color: #238636; color: #FFFFFF; border-radius: 6px; padding: 6px 16px; font-weight: bold; border: none; } QPushButton:hover { background-color: #2EA043; } QPushButton:pressed { background-color: #248039; } QPushButton:disabled { background-color: #21262D; color: #8B949E; }";
    QString triggerStyle= "QPushButton { background-color: #A371F7; color: #FFFFFF; border-radius: 6px; padding: 6px 16px; font-weight: bold; border: none; } QPushButton:hover { background-color: #B48EFA; } QPushButton:pressed { background-color: #8957E5; } QPushButton:disabled { background-color: #21262D; color: #8B949E; }";
    QString outputStyle = "QLabel { color: #EC5990; font-size: 12px; font-weight: bold; background-color: #0D1117; border: 1px solid #21262D; border-radius: 6px; padding: 6px 10px; }";

    // 🌟 슬라이더 전용 모던 다크 스타일시트 정의
    QString sliderStyle = "QSlider::groove:horizontal { height: 6px; background: #0D1117; border-radius: 3px; }"
                          "QSlider::sub-page:horizontal { background: #A371F7; border-radius: 3px; }" // 퍼플 포인트 배색 적용
                          "QSlider::handle:horizontal { background: #E6EDF3; width: 14px; margin-top: -4px; margin-bottom: -4px; border-radius: 7px; }"
                          "QSlider::handle:horizontal:hover { background: #58A6FF; }";

    // --- Card 0: insert ---
    QWidget *insertCard = new QWidget(rightContentWidget); insertCard->setAttribute(Qt::WA_StyledBackground, true); insertCard->setStyleSheet(cardStyle);
    QVBoxLayout *insL = new QVBoxLayout(insertCard); QHBoxLayout *insTop = new QHBoxLayout();
    QLabel *insT = new QLabel("insert()", insertCard); insT->setStyleSheet(titleStyle);
    QLabel *insD = new QLabel("Insert element safely", insertCard); insD->setStyleSheet(descStyle);
    QLabel *insC = new QLabel("O(log n)", insertCard); insC->setStyleSheet(compStyle);
    insTop->addWidget(insT); insTop->addWidget(insD); insTop->addStretch(); insTop->addWidget(insC);
    QHBoxLayout *insBot = new QHBoxLayout(); insertInput = new QLineEdit(insertCard); insertInput->setPlaceholderText("Key..."); insertInput->setStyleSheet(inputStyle);
    insertButton = new QPushButton("Insert", insertCard); insertButton->setStyleSheet(btnStyle);
    insBot->addWidget(insertInput); insBot->addWidget(insertButton);
    insL->addLayout(insTop); insL->addLayout(insBot);
    controlGrid->addWidget(insertCard, 0, 0);

    // --- Card 1: delete ---
    QWidget *deleteCard = new QWidget(rightContentWidget); deleteCard->setAttribute(Qt::WA_StyledBackground, true); deleteCard->setStyleSheet(cardStyle);
    QVBoxLayout *delL = new QVBoxLayout(deleteCard); QHBoxLayout *delTop = new QHBoxLayout();
    QLabel *delT = new QLabel("delete()", deleteCard); delT->setStyleSheet(titleStyle);
    QLabel *delD = new QLabel("Remove element & balance", deleteCard); delD->setStyleSheet(descStyle);
    QLabel *delC = new QLabel("O(log n)", deleteCard); delC->setStyleSheet(compStyle);
    delTop->addWidget(delT); delTop->addWidget(delD); delTop->addStretch(); delTop->addWidget(delC);
    QHBoxLayout *delBot = new QHBoxLayout(); deleteInput = new QLineEdit(deleteCard); deleteInput->setPlaceholderText("Key..."); deleteInput->setStyleSheet(inputStyle);
    deleteButton = new QPushButton("Trigger", deleteCard); deleteButton->setStyleSheet(triggerStyle);
    delBot->addWidget(deleteInput); delBot->addWidget(deleteButton);
    delL->addLayout(delTop); delL->addLayout(delBot);
    controlGrid->addWidget(deleteCard, 0, 1);

    // --- Card 2: search ---
    QWidget *searchCard = new QWidget(rightContentWidget); searchCard->setAttribute(Qt::WA_StyledBackground, true); searchCard->setStyleSheet(cardStyle);
    QVBoxLayout *schL = new QVBoxLayout(searchCard); QHBoxLayout *schTop = new QHBoxLayout();
    QLabel *schT = new QLabel("search()", searchCard); schT->setStyleSheet(titleStyle);
    QLabel *schD = new QLabel("Find targeted key", searchCard); schD->setStyleSheet(descStyle);
    QLabel *schC = new QLabel("O(log n)", searchCard); schC->setStyleSheet(compStyle);
    schTop->addWidget(schT); schTop->addWidget(schD); schTop->addStretch(); schTop->addWidget(schC);
    QHBoxLayout *schBot = new QHBoxLayout(); searchInput = new QLineEdit(searchCard); searchInput->setPlaceholderText("Key..."); searchInput->setStyleSheet(inputStyle);
    searchButton = new QPushButton("Search", searchCard); searchButton->setStyleSheet(btnStyle);
    schBot->addWidget(searchInput); schBot->addWidget(searchButton);
    schL->addLayout(schTop); schL->addLayout(schBot);
    controlGrid->addWidget(searchCard, 1, 0);

    // --- Card 3: min ---
    QWidget *minCard = new QWidget(rightContentWidget); minCard->setAttribute(Qt::WA_StyledBackground, true); minCard->setStyleSheet(cardStyle);
    QVBoxLayout *minL = new QVBoxLayout(minCard); QHBoxLayout *minTop = new QHBoxLayout();
    QLabel *minT = new QLabel("min()", minCard); minT->setStyleSheet(titleStyle);
    QLabel *minD = new QLabel("Find minimum key", minCard); minD->setStyleSheet(descStyle);
    QLabel *minC = new QLabel("O(log n)", minCard); minC->setStyleSheet(compStyle);
    minTop->addWidget(minT); minTop->addWidget(minD); minTop->addStretch(); minTop->addWidget(minC);
    QHBoxLayout *minBot = new QHBoxLayout(); minOutput = new QLabel("Ready", minCard); minOutput->setStyleSheet(outputStyle);
    minButton = new QPushButton("Peek", minCard); minButton->setStyleSheet(btnStyle);
    minBot->addWidget(minOutput, 1); minBot->addWidget(minButton);
    minL->addLayout(minTop); minL->addLayout(minBot);
    controlGrid->addWidget(minCard, 1, 1);

    // --- Card 4: max ---
    QWidget *maxCard = new QWidget(rightContentWidget); maxCard->setAttribute(Qt::WA_StyledBackground, true); maxCard->setStyleSheet(cardStyle);
    QVBoxLayout *maxL = new QVBoxLayout(maxCard); QHBoxLayout *maxTop = new QHBoxLayout();
    QLabel *maxT = new QLabel("max()", maxCard); maxT->setStyleSheet(titleStyle);
    QLabel *maxD = new QLabel("Find maximum key", maxCard); maxD->setStyleSheet(descStyle);
    QLabel *maxC = new QLabel("O(log n)", maxCard); maxC->setStyleSheet(compStyle);
    maxTop->addWidget(maxT); maxTop->addWidget(maxD); maxTop->addStretch(); maxTop->addWidget(maxC);
    QHBoxLayout *maxBot = new QHBoxLayout(); maxOutput = new QLabel("Ready", maxCard); maxOutput->setStyleSheet(outputStyle);
    maxButton = new QPushButton("Peek", maxCard); maxButton->setStyleSheet(btnStyle);
    maxBot->addWidget(maxOutput, 1); maxBot->addWidget(maxButton);
    maxL->addLayout(maxTop); maxL->addLayout(maxBot);
    controlGrid->addWidget(maxCard, 2, 0);

    // --- Card 5: is_empty ---
    QWidget *ieCard = new QWidget(rightContentWidget); ieCard->setAttribute(Qt::WA_StyledBackground, true); ieCard->setStyleSheet(cardStyle);
    QVBoxLayout *ieL = new QVBoxLayout(ieCard); QHBoxLayout *ieTop = new QHBoxLayout();
    QLabel *ieT = new QLabel("is_empty()", ieCard); ieT->setStyleSheet(titleStyle);
    QLabel *ieD = new QLabel("Check emptiness", ieCard); ieD->setStyleSheet(descStyle);
    QLabel *ieC = new QLabel("O(1)", ieCard); ieC->setStyleSheet(compStyle);
    ieTop->addWidget(ieT); ieTop->addWidget(ieD); ieTop->addStretch(); ieTop->addWidget(ieC);
    QHBoxLayout *ieBot = new QHBoxLayout(); isEmptyOutput = new QLabel("Ready", ieCard); isEmptyOutput->setStyleSheet(outputStyle);
    isEmptyButton = new QPushButton("Check", ieCard); isEmptyButton->setStyleSheet(btnStyle);
    ieBot->addWidget(isEmptyOutput, 1); ieBot->addWidget(isEmptyButton);
    ieL->addLayout(ieTop); ieL->addLayout(ieBot);
    controlGrid->addWidget(ieCard, 2, 1);

    // --- Card 6: len ---
    QWidget *lenCard = new QWidget(rightContentWidget); lenCard->setAttribute(Qt::WA_StyledBackground, true); lenCard->setStyleSheet(cardStyle);
    QVBoxLayout *lenL = new QVBoxLayout(lenCard); QHBoxLayout *lenTop = new QHBoxLayout();
    QLabel *lenT = new QLabel("len()", lenCard); lenT->setStyleSheet(titleStyle);
    QLabel *lenD = new QLabel("Get node count", lenCard); lenD->setStyleSheet(descStyle);
    QLabel *lenC = new QLabel("O(1)", lenCard); lenC->setStyleSheet(compStyle);
    lenTop->addWidget(lenT); lenTop->addWidget(lenD); lenTop->addStretch(); lenTop->addWidget(lenC);
    QHBoxLayout *lenBot = new QHBoxLayout(); lenOutput = new QLabel("Ready", lenCard); lenOutput->setStyleSheet(outputStyle);
    lenButton = new QPushButton("Size", lenCard); lenButton->setStyleSheet(btnStyle);
    lenBot->addWidget(lenOutput, 1); lenBot->addWidget(lenButton);
    lenL->addLayout(lenTop); lenL->addLayout(lenBot);
    controlGrid->addWidget(lenCard, 3, 0);

    // 🌟 --- New Card 7: Animation Speed Control (속도 조절 가로 와이드 카드 추가) ---
    QWidget *speedCard = new QWidget(rightContentWidget); speedCard->setAttribute(Qt::WA_StyledBackground, true); speedCard->setStyleSheet(cardStyle);
    QVBoxLayout *speedL = new QVBoxLayout(speedCard); QHBoxLayout *speedTop = new QHBoxLayout();
    QLabel *spTitle = new QLabel("Interval Speed", speedCard); spTitle->setStyleSheet(titleStyle);
    QLabel *spDesc = new QLabel("Adjust rotation refresh rate", speedCard); spDesc->setStyleSheet(descStyle);
    speedLabel = new QLabel("700 ms", speedCard); speedLabel->setStyleSheet("color: #56B6C2; font-size: 11px; font-weight: bold; font-family: monospace;");
    speedTop->addWidget(spTitle); speedTop->addWidget(spDesc); speedTop->addStretch(); speedTop->addWidget(speedLabel);
    QHBoxLayout *speedBot = new QHBoxLayout();
    speedSlider = new QSlider(Qt::Horizontal, speedCard); speedSlider->setRange(100, 2000); speedSlider->setValue(700); speedSlider->setStyleSheet(sliderStyle);
    speedBot->addWidget(speedSlider); speedL->addLayout(speedTop); speedL->addLayout(speedBot);
    controlGrid->addWidget(speedCard, 3, 1); // 3행 1열 배치로 레이아웃 공백 최소화

    rightLayout->addLayout(controlGrid);

    // --- Bottom: Terminal Log ---
    QLabel *logTitle = new QLabel(">_ System Live Log", rightContentWidget);
    logTitle->setStyleSheet("color: #8B949E; font-size: 11px; font-family: monospace; font-weight: bold; margin-top: 6px;");
    rightLayout->addWidget(logTitle);
    statusOutput = new QLabel("System initialized. AVL Balanced Model Ready.", rightContentWidget);
    statusOutput->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    statusOutput->setWordWrap(true);
    statusOutput->setStyleSheet(
        "background-color: #0D1117; border: 1px solid #21262D; border-radius: 8px; "
        "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
        );
    rightLayout->addWidget(statusOutput);

    rightScrollArea->setWidget(rightContentWidget);

    // ==========================================
    // 3. 레이아웃 조립 및 이벤트 바인딩
    // ==========================================
    mainLayout->addWidget(leftGroup, 1);
    mainLayout->addWidget(rightScrollArea);

    connect(insertButton, &QPushButton::clicked, this, &TREE_AVL::insert_op);
    connect(deleteButton, &QPushButton::clicked, this, &TREE_AVL::delete_op);
    connect(searchButton, &QPushButton::clicked, this, &TREE_AVL::search_op);
    connect(minButton, &QPushButton::clicked, this, &TREE_AVL::min_op);
    connect(maxButton, &QPushButton::clicked, this, &TREE_AVL::max_op);
    connect(isEmptyButton, &QPushButton::clicked, this, &TREE_AVL::is_empty);
    connect(lenButton, &QPushButton::clicked, this, &TREE_AVL::_len);

    // 🌟 슬라이더 조절 이벤트 연결
    connect(speedSlider, &QSlider::valueChanged, this, &TREE_AVL::updateSpeed);

    this->updateVisuals();
    return containerWidget;
}

// 🌟 신규 비동기 타이머 속도 갱신 슬롯 유닛 구현
void TREE_AVL::updateSpeed(int value) {
    speedLabel->setText(QString("%1 ms").arg(value));
    // 만약 애니메이션 루프가 동작 중이라면 타이머 리프레시 간격을 즉시 동적 치환합니다.
    if (animationTimer->isActive()) {
        animationTimer->setInterval(value);
    }
}

// ==========================================
// 3. 기본 코어 밸런싱 및 회전 연산 로직 (기존 소스 완전 유지)
// ==========================================
tree_avl* TREE_AVL::rotateRight(tree_avl* y) {
    tree_avl* x = y->left;
    tree_avl* T2 = x->right;
    x->right = y;
    y->left = T2;
    updateHeight(y);
    updateHeight(x);
    return x;
}

tree_avl* TREE_AVL::rotateLeft(tree_avl* x) {
    tree_avl* y = x->right;
    tree_avl* T2 = y->left;
    y->left = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

tree_avl* TREE_AVL::balanceTree(tree_avl* node, bool& rotated) {
    if (!node) return nullptr;
    updateHeight(node);
    int balance = getBalance(node);

    if (balance > 1 && getBalance(node->left) >= 0) {
        statusOutput->setText(QString(">> Rotation: Executing LL Rotation at Node [ %1 ]...").arg(node->key));
        rotated = true;
        return rotateRight(node);
    }
    if (balance > 1 && getBalance(node->left) < 0) {
        statusOutput->setText(QString(">> Rotation: Executing LR Rotation at Node [ %1 ]...").arg(node->key));
        node->left = rotateLeft(node->left);
        rotated = true;
        return rotateRight(node);
    }
    if (balance < -1 && getBalance(node->right) <= 0) {
        statusOutput->setText(QString(">> Rotation: Executing RR Rotation at Node [ %1 ]...").arg(node->key));
        rotated = true;
        return rotateLeft(node);
    }
    if (balance < -1 && getBalance(node->right) > 0) {
        statusOutput->setText(QString(">> Rotation: Executing RL Rotation at Node [ %1 ]...").arg(node->key));
        node->right = rotateRight(node->right);
        rotated = true;
        return rotateLeft(node);
    }
    return node;
}

// ==========================================
// 4. 타이머 기반 비동기 삽입 루프 (속도 가변 대응 업데이트)
// ==========================================
void TREE_AVL::insert_op() {
    if (currentInsertState != I_IDLE || currentDeleteState != IDLE) return;
    bool ok;
    insertKey = insertInput->text().toInt(&ok);
    if (!ok) { statusOutput->setText(">> Error: Please verify the integer conversion casting."); return; }

    insertInput->clear();
    setControlsEnabled(false);
    pathStack.clear();
    nodeA = nodeB = nodeC = nullptr;
    if (!root) {
        root = new tree_avl(insertKey);
        size++;
        statusOutput->setText(QString(">> Success: Assigned key value [ %1 ] as the initial Root node.").arg(insertKey));
        setControlsEnabled(true);
        updateVisuals();
        return;
    }

    currentInsertState = I_SEARCHING;
    currNode = root;

    highlightSearchKey = currNode->key;
    highlightReplaceKey = -1;
    statusOutput->setText(QString(">> Searching: Comparing with Node [ %1 ] to insert [ %2 ]...").arg(currNode->key).arg(insertKey));
    updateVisuals();

    // 하드코딩(700) 대신 사용자가 설정한 슬라이더 밀리초 값을 연동 추적
    animationTimer->start(speedSlider->value());
}

void TREE_AVL::insertStep() {
    int spVal = speedSlider->value(); // 동적 속도 캐싱
    switch (currentInsertState) {
    case I_SEARCHING: {
        pathStack.push_back(currNode);
        if (insertKey == currNode->key) {
            statusOutput->setText(QString(">> Error: Key duplicate restriction! Value [ %1 ] already exists.").arg(insertKey));
            animationTimer->stop();
            currentInsertState = I_IDLE;
            highlightSearchKey = -1;
            setControlsEnabled(true);
            updateVisuals();
            return;
        }

        if (insertKey < currNode->key) {
            if (currNode->left == nullptr) {
                currentInsertState = I_DONE;
                animationTimer->setInterval(spVal * 0.7); // 상대 가중치 유지
                return;
            }
            currNode = currNode->left;
        } else {
            if (currNode->right == nullptr) {
                currentInsertState = I_DONE;
                animationTimer->setInterval(spVal * 0.7);
                return;
            }
            currNode = currNode->right;
        }

        highlightSearchKey = currNode->key;
        statusOutput->setText(QString(">> Searching: Routing down to Node [ %1 ] for insertion slot...").arg(currNode->key));
        updateVisuals();
        break;
    }

    case I_DONE: {
        tree_avl* newNode = new tree_avl(insertKey);
        if (insertKey < currNode->key) currNode->left = newNode;
        else currNode->right = newNode;

        size++;
        pathStack.push_back(newNode);

        highlightSearchKey = -1;
        highlightReplaceKey = newNode->key;
        statusOutput->setText(QString(">> Success: Node [ %1 ] appended. Backtracking for balance verification...").arg(insertKey));
        updateVisuals();

        currentInsertState = I_BACKTRACK;
        animationTimer->setInterval(spVal * 1.4);
        break;
    }

    case I_BACKTRACK: {
        if (pathStack.isEmpty()) {
            statusOutput->setText(">> Success: Structural rebalance completed. AVL validation passed.");
            animationTimer->stop();
            currentInsertState = I_IDLE;
            highlightReplaceKey = -1;
            setControlsEnabled(true);
            updateVisuals();
            return;
        }

        currNode = pathStack.takeLast();
        updateHeight(currNode);
        int bf = getBalance(currNode);

        highlightSearchKey = currNode->key;
        statusOutput->setText(QString(">> Validation: Backtracking Node [ %1 ] -> Balance Factor: %2").arg(currNode->key).arg(bf));
        updateVisuals();
        if (std::abs(bf) > 1) {
            nodeA = currNode;
            currentInsertState = I_SELECT_A;
            animationTimer->setInterval(spVal * 1.4);
        }
        break;
    }

    case I_SELECT_A: {
        highlightSearchKey = -1;
        statusOutput->setText(QString(">> Balance Alert: Critical threshold hit! Tracking Grandparent (Node A: [ %1 ])").arg(nodeA->key));
        updateVisuals();

        currentInsertState = I_SELECT_B;
        animationTimer->setInterval(spVal * 1.4);
        break;
    }

    case I_SELECT_B: {
        if (insertKey < nodeA->key) nodeB = nodeA->left;
        else nodeB = nodeA->right;

        statusOutput->setText(QString(">> Balance Alert: Tracking Parent on routing pipeline (Node B: [ %1 ])").arg(nodeB->key));
        updateVisuals();
        currentInsertState = I_SELECT_C;
        animationTimer->setInterval(spVal * 1.4);
        break;
    }

    case I_SELECT_C: {
        if (insertKey < nodeB->key) nodeC = nodeB->left;
        else nodeC = nodeB->right;

        statusOutput->setText(QString(">> Balance Alert: Tracking Child path descriptor (Node C: [ %1 ])").arg(nodeC->key));
        updateVisuals();

        currentInsertState = I_ROTATING;
        animationTimer->setInterval(spVal * 2.1);
        break;
    }

    case I_ROTATING: {
        QString caseName = "";
        if (nodeB == nodeA->left && nodeC == nodeB->left) caseName = "LL Case (Single Right Rotation)";
        else if (nodeB == nodeA->right && nodeC == nodeB->right) caseName = "RR Case (Single Left Rotation)";
        else if (nodeB == nodeA->left && nodeC == nodeB->right) caseName = "LR Case (Double Left-Right Rotation)";
        else if (nodeB == nodeA->right && nodeC == nodeB->left) caseName = "RL Case (Double Right-Left Rotation)";
        statusOutput->setText(QString(">> Strategy: Matrix matched %1. Rebalancing execution...").arg(caseName));

        tree_avl* parentOfA = pathStack.isEmpty() ? nullptr : pathStack.last();
        bool rotatedUnused = false;
        tree_avl* rotatedSubtree = balanceTree(nodeA, rotatedUnused);

        if (!parentOfA) root = rotatedSubtree;
        else if (parentOfA->left == nodeA) parentOfA->left = rotatedSubtree;
        else parentOfA->right = rotatedSubtree;

        nodeA = nodeB = nodeC = nullptr;
        highlightReplaceKey = -1;
        updateVisuals();

        animationTimer->stop();
        currentInsertState = I_IDLE;
        setControlsEnabled(true);
        break;
    }
    }
}

// ==========================================
// 5. 타이머 기반 비동기 삭제 루프 (속도 가변 대응 업데이트)
// ==========================================
void TREE_AVL::delete_op() {
    if (currentDeleteState != IDLE || currentInsertState != I_IDLE) return;
    bool ok;
    targetKey = deleteInput->text().toInt(&ok);
    if (!ok || !root) { statusOutput->setText(">> Error: Invalid key format or empty tree sequence."); return; }

    deleteInput->clear();
    setControlsEnabled(false);

    currentDeleteState = SEARCHING;
    currNode = root;
    parentNode = nullptr;
    replaceNode = nullptr;
    replaceParent = nullptr;

    highlightSearchKey = currNode->key;
    highlightTargetKey = -1;
    highlightReplaceKey = -1;
    statusOutput->setText(QString(">> Searching: Accessing Node [ %1 ] in AVL hierarchy...").arg(currNode->key));
    updateVisuals();

    animationTimer->start(speedSlider->value());
}

void TREE_AVL::deleteStep() {
    static int checkStep = 0;
    int spVal = speedSlider->value();

    if (currentDeleteState == SEARCHING) {
        if (!currNode) {
            statusOutput->setText(">> Error: Targeted Node key cannot be found in this tree.");
            animationTimer->stop();
            currentDeleteState = IDLE;
            highlightSearchKey = -1;
            setControlsEnabled(true);
            updateVisuals();
            return;
        }

        if (targetKey == currNode->key) {
            currentDeleteState = TARGET_FOUND;
            checkStep = 0;
            highlightSearchKey = -1;
            highlightTargetKey = currNode->key;
            statusOutput->setText(QString(">> Target Found: Node [ %1 ] hit. Checking left child link state...").arg(currNode->key));
            updateVisuals();
            animationTimer->setInterval(spVal * 1.4);
            return;
        }

        parentNode = currNode;
        if (targetKey < currNode->key) currNode = currNode->left;
        else currNode = currNode->right;
        if (currNode) {
            highlightSearchKey = currNode->key;
            statusOutput->setText(QString(">> Searching: Routing down to Node [ %1 ]...").arg(currNode->key));
        }
        updateVisuals();
        animationTimer->setInterval(spVal);
    }

    else if (currentDeleteState == TARGET_FOUND) {
        if (checkStep == 0) {
            if (!currNode->left) {
                highlightSearchKey = -1;
                statusOutput->setText(">> Mode: Left link is empty. Verification routing shifted to right branch...");
                checkStep = 2;
            } else {
                highlightSearchKey = currNode->left->key;
                statusOutput->setText(QString(">> Mode: Left child detected at [ %1 ]. Inspecting right link status...").arg(currNode->left->key));
                checkStep = 1;
            }
            updateVisuals();
            animationTimer->setInterval(spVal * 1.7);
            return;
        }

        else if (checkStep == 1) {
            if (currNode->right) {
                highlightSearchKey = currNode->right->key;
                bool isRootNode = (parentNode == nullptr);
                bool leftChildHasRight = (currNode->left->right != nullptr);
                QString caseStr = "";
                if (isRootNode) {
                    if (leftChildHasRight) caseStr = ">> Case 3b: Root element swap needed. Left child contains valid right sub-hierarchy.";
                    else caseStr = ">> Case 3a: Root element swap needed. Left child has no further right branches.";
                } else {
                    bool deletedIsLeft = (parentNode->left == currNode);
                    if (deletedIsLeft && leftChildHasRight) caseStr = ">> Case 3d: Target is left branch node, its left child contains right sub-hierarchy.";
                    else if (!deletedIsLeft && leftChildHasRight) caseStr = ">> Case 3d: Target is right branch node, its left child contains right sub-hierarchy.";
                    else if (deletedIsLeft && !leftChildHasRight) caseStr = ">> Case 3c: Target is left branch node, its left child has no right sub-hierarchy.";
                    else caseStr = ">> Case 3c: Target is right branch node, its left child has no right sub-hierarchy.";
                }

                statusOutput->setText(QString("%1 Seeking predecessor reference...").arg(caseStr));
                replaceParent = currNode;
                replaceNode = currNode->left;
                highlightReplaceKey = replaceNode->key;
                highlightSearchKey = -1;

                currentDeleteState = FIND_REPLACE;
                updateVisuals();
                animationTimer->setInterval(spVal * 2.1);
                return;
            } else {
                bool isLeftChild = (parentNode && parentNode->left == currNode);
                QString caseStr = "";
                if (!parentNode) caseStr = ">> Case 2: Tree root contains only a single left node hierarchy.";
                else if (isLeftChild) caseStr = ">> Case 2: Target is left branch with only a left child node link.";
                else caseStr = ">> Case 2: Target is right branch with only a left child node link.";

                statusOutput->setText(caseStr);
                size--;
                tree_avl* temp = currNode->left;
                if (!parentNode) root = temp;
                else if (parentNode->left == currNode) parentNode->left = temp;
                else parentNode->right = temp;

                delete currNode;
                currNode = nullptr;

                statusOutput->setText(">> Success: Node bypass executed. Shifting context to AVL Rebalancing pipeline...");
                currentDeleteState = REBALANCING;
                animationTimer->setInterval(spVal * 1.7);
                updateVisuals();
                return;
            }
        }

        else if (checkStep == 2) {
            if (currNode->right) {
                highlightSearchKey = currNode->right->key;
                bool isLeftChild = (parentNode && parentNode->left == currNode);
                QString caseStr = "";
                if (!parentNode) caseStr = ">> Case 2: Tree root contains only a single right node hierarchy.";
                else if (isLeftChild) caseStr = ">> Case 2: Target is left branch with only a right child node link.";
                else caseStr = ">> Case 2: Target is right branch with only a right child node link.";

                statusOutput->setText(caseStr);
                size--;
                tree_avl* temp = currNode->right;
                if (!parentNode) root = temp;
                else if (parentNode->left == currNode) parentNode->left = temp;
                else parentNode->right = temp;

                delete currNode;
                currNode = nullptr;
            } else {
                statusOutput->setText(">> Case 1: Targeted item is a terminal Leaf node. Safe removal executing...");
                size--;
                if (!parentNode) root = nullptr;
                else if (parentNode->left == currNode) parentNode->left = nullptr;
                else parentNode->right = nullptr;

                delete currNode;
                currNode = nullptr;
            }
            currentDeleteState = REBALANCING;
            animationTimer->setInterval(spVal * 1.7);
            updateVisuals();
            return;
        }
    }

    else if (currentDeleteState == FIND_REPLACE) {
        if (replaceNode->right) {
            replaceParent = replaceNode;
            replaceNode = replaceNode->right;
            highlightReplaceKey = replaceNode->key;
            statusOutput->setText(QString(">> Strategy: Seeking maximum candidate key via right-most pipeline -> [ %1 ]").arg(replaceNode->key));
            updateVisuals();
            animationTimer->setInterval(spVal * 1.1);
        } else {
            statusOutput->setText(QString(">> Strategy: Maximum value discovered [ %1 ]. Initializing override pipeline.").arg(replaceNode->key));
            currentDeleteState = COPY_AND_DELETE;
            animationTimer->setInterval(spVal * 1.7);
        }
    }

    else if (currentDeleteState == COPY_AND_DELETE) {
        currNode->key = replaceNode->key;
        highlightTargetKey = currNode->key;
        highlightReplaceKey = -1;
        updateVisuals();

        tree_avl* child = replaceNode->left;
        if (replaceParent->left == replaceNode) replaceParent->left = child;
        else replaceParent->right = child;

        delete replaceNode;
        replaceNode = nullptr;
        size--;

        statusOutput->setText(">> Success: Predecessor bypass completed. Entering recursive AVL Rebalancing stage...");
        currentDeleteState = REBALANCING;
        animationTimer->setInterval(spVal * 1.7);
    }

    else if (currentDeleteState == REBALANCING) {
        bool rotated = false;
        std::function<tree_avl*(tree_avl*)> rebalanceRecursive = [&](tree_avl* node) -> tree_avl* {
            if (!node) return nullptr;
            node->left = rebalanceRecursive(node->left);
            node->right = rebalanceRecursive(node->right);
            return balanceTree(node, rotated);
        };

        root = rebalanceRecursive(root);
        updateVisuals();

        animationTimer->stop();
        currentDeleteState = IDLE;
        highlightTargetKey = -1;
        highlightSearchKey = -1;
        highlightReplaceKey = -1;
        setControlsEnabled(true);
        if (rotated) {
            updateVisuals();
        } else {
            statusOutput->setText(">> Success: Target element securely removed. Balanced state preserved.");
        }
    }
}

// ==========================================
// 6. 기본 제어 연산 로직 구현 (기존 소스 완전 유지)
// ==========================================
void TREE_AVL::search_op() {
    bool ok;
    int val = searchInput->text().toInt(&ok);
    if (!ok) { statusOutput->setText(">> Error: Unable to cast input parameter to a valid Integer."); return; }

    tree_avl* C = root;
    while (C != nullptr) {
        if (val == C->key) { statusOutput->setText(QString(">> Success: Binary search hit! Key [ %1 ] is active.").arg(val)); return; }
        if (val < C->key) C = C->left; else C = C->right;
    }
    statusOutput->setText(">> Failure: Requested key does not belong to this tree entity.");
}

void TREE_AVL::min_op() {
    if(!root) { statusOutput->setText(">> Warning: Search terminated. Node size is 0."); return; }
    tree_avl* c=root; while(c->left) c=c->left;
    minOutput->setText(QString::number(c->key));
    statusOutput->setText(">> Inspection: Left-bound scan hit. Minimum key value is " + QString::number(c->key));
}

void TREE_AVL::max_op() {
    if(!root) { statusOutput->setText(">> Warning: Search terminated. Node size is 0."); return; }
    tree_avl* c=root; while(c->right) c=c->right;
    maxOutput->setText(QString::number(c->key));
    statusOutput->setText(">> Inspection: Right-bound scan hit. Maximum key value is " + QString::number(c->key));
}

void TREE_AVL::is_empty() {
    if (size == 0) { isEmptyOutput->setText("True"); statusOutput->setText(">> Inspection: is_empty() -> TRUE (Tree contains zero valid references)."); }
    else { isEmptyOutput->setText("False"); statusOutput->setText(">> Inspection: is_empty() -> FALSE (Active balanced nodes verified)."); }
}

void TREE_AVL::_len() {
    lenOutput->setText(QString::number(size));
    statusOutput->setText(">> Inspection: len() -> Active entity nodes tracked in memory: " + QString::number(size));
}

// ==========================================
// 7. 시각화 업데이트 및 노드 드로잉 로직 (기존 소스 완전 유지)
// ==========================================
void TREE_AVL::updateVisuals() {
    scene->clear();
    if (!root) return;

    double initial_x_offset = 160.0;
    drawNode(root, 0, 0.0, initial_x_offset);

    QRectF bounds = scene->itemsBoundingRect();
    scene->setSceneRect(bounds.adjusted(-80, -40, 80, 80));
}

void TREE_AVL::drawNode(tree_avl* node, int level, double x_pos, double x_offset) {
    if (!node) return;
    double y_pos = level * LEVEL_HEIGHT;

    QPen pen(QColor("#D29922"), 2);
    QBrush brush(QColor("#161B22"));
    QColor textColor(QColor("#E6EDF3"));

    if (node == nodeA) { brush = QBrush(QColor("#FF7B72")); pen = QPen(QColor("#F85149"), 2); textColor = QColor("#0D1117"); }
    else if (node == nodeB) { brush = QBrush(QColor("#3FB950")); pen = QPen(QColor("#238636"), 2); textColor = QColor("#FFFFFF"); }
    else if (node == nodeC) { brush = QBrush(QColor("#B48EFA")); pen = QPen(QColor("#A371F7"), 2); textColor = QColor("#FFFFFF"); }
    else if (node->key == highlightTargetKey) { brush = QBrush(QColor("#F85149")); pen = QPen(QColor("#FF7B72"), 2); textColor = QColor("#0D1117"); }
    else if (node->key == highlightReplaceKey) { brush = QBrush(QColor("#238636")); pen = QPen(QColor("#3FB950"), 2); textColor = QColor("#FFFFFF"); }
    else if (node->key == highlightSearchKey) { brush = QBrush(QColor("#1F6FEB")); pen = QPen(QColor("#58A6FF"), 2); textColor = QColor("#FFFFFF"); }

    QPainterPath rectPath;
    rectPath.addRoundedRect(x_pos - NODE_WIDTH/2, y_pos, NODE_WIDTH, NODE_HEIGHT, 6.0, 6.0);
    QGraphicsPathItem *pathItem = new QGraphicsPathItem(rectPath);
    pathItem->setPen(pen); pathItem->setBrush(brush);
    scene->addItem(pathItem);

    QGraphicsTextItem *textItem = new QGraphicsTextItem(QString::number(node->key));
    textItem->setDefaultTextColor(textColor);
    QFont font = textItem->font();
    font.setBold(true); font.setPointSize(10);
    textItem->setFont(font);

    double textWidth = textItem->boundingRect().width();
    double textHeight = textItem->boundingRect().height();
    textItem->setPos(x_pos - textWidth/2, y_pos + (NODE_HEIGHT - textHeight)/2);
    scene->addItem(textItem);

    double next_x_offset = x_offset * 0.44;
    QPointF parentBottom(x_pos, y_pos + NODE_HEIGHT);
    QPen edgePen(QColor("#30363D"), 2);

    if (node->left) {
        double left_x = x_pos - x_offset;
        scene->addLine(parentBottom.x(), parentBottom.y(), left_x, (level + 1) * LEVEL_HEIGHT, edgePen);
        drawNode(node->left, level + 1, left_x, next_x_offset);
    }
    if (node->right) {
        double right_x = x_pos + x_offset;
        scene->addLine(parentBottom.x(), parentBottom.y(), right_x, (level + 1) * LEVEL_HEIGHT, edgePen);
        drawNode(node->right, level + 1, right_x, next_x_offset);
    }
}