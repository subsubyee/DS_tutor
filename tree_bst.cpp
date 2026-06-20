#include "tree_bst.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPen>
#include <QBrush>
#include <QColor>

// ==========================================
// 1. TREE_BST 생성자 및 소멸자 구현
// ==========================================
TREE_BST::TREE_BST(QMainWindow* _parent) : QObject(_parent) {
    parent = _parent;
    root = nullptr;
    size = 0;
    highlightSearchKey = -1;
    highlightTargetKey = -1;
    highlightReplaceKey = -1;

    currentDeleteState = IDLE;
    currNode = nullptr;
    parentNode = nullptr;
    replaceNode = nullptr;
    replaceParent = nullptr;
    targetKey = -1;

    // 타이머 초기화 및 비동기 루프 슬롯 바인딩
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &TREE_BST::deleteStep);
}

TREE_BST::~TREE_BST() { clearTree(root); }

void TREE_BST::clearTree(BST_Node* node) {
    if (node) { clearTree(node->left); clearTree(node->right); delete node; }
}

// 애니메이션 진행 도중 다른 연산을 제한하는 안전장치
void TREE_BST::setControlsEnabled(bool enabled) {
    insertButton->setEnabled(enabled);
    deleteButton->setEnabled(enabled);
    searchButton->setEnabled(enabled);
    minButton->setEnabled(enabled);
    maxButton->setEnabled(enabled);
    isEmptyButton->setEnabled(enabled);
    lenButton->setEnabled(enabled);
    speedSlider->setEnabled(enabled); // 애니메이션 도중 속도 슬라이더 고정 (선택 사항)
}

// ==========================================
// 2. UI 레이아웃 세팅 (모던 옥시디언 다크 테마 반영)
// ==========================================
QWidget* TREE_BST::setting() {
    // --- 메인 컨테이너 및 전역 배경 설정 (Modern Obsidian Dark Theme) ---
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet(
        "QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }"
        );

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION (QGraphicsView 다크 최적화)
    // ==========================================
    leftGroup = new QGroupBox(containerWidget);
    leftGroup->setStyleSheet("QGroupBox { border: none; background: transparent; }");

    leftLayout = new QVBoxLayout(leftGroup);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nBinary Search Tree Topology", leftGroup);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 14px;");
    leftLayout->addWidget(visTitle);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, leftGroup);
    view->setRenderHint(QPainter::Antialiasing);

    // 캔버스 배경 및 컨테이너 테두리를 타 모듈 사양과 완벽히 통일
    view->setBackgroundBrush(QBrush(QColor("#11141A")));
    view->setStyleSheet("QGraphicsView { border: 1px solid #21262D; border-radius: 12px; background-color: #11141A; }");

    leftLayout->addWidget(view);

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

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nBinary Search Tree Operations Dashboard", rightContentWidget);
    controlsTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #8B949E; letter-spacing: 1px; margin-bottom: 4px;");
    rightLayout->addWidget(controlsTitle);

    QGridLayout *controlGrid = new QGridLayout();
    controlGrid->setSpacing(12);

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

    // 🌟 슬라이더 전용 모던 Obsidian 다크 스타일시트 정의
    QString sliderStyle = "QSlider::groove:horizontal { height: 6px; background: #0D1117; border-radius: 3px; }"
                          "QSlider::sub-page:horizontal { background: #A371F7; border-radius: 3px; }" // 퍼플 포인트 배색 일치화
                          "QSlider::handle:horizontal { background: #E6EDF3; width: 14px; margin-top: -4px; margin-bottom: -4px; border-radius: 7px; }"
                          "QSlider::handle:horizontal:hover { background: #58A6FF; }";

    // --- Card 0: insert (0행 0열) ---
    QWidget *insertCard = new QWidget(rightContentWidget); insertCard->setAttribute(Qt::WA_StyledBackground, true); insertCard->setStyleSheet(cardStyle);
    QVBoxLayout *insL = new QVBoxLayout(insertCard); QHBoxLayout *insTop = new QHBoxLayout();
    QLabel *insT = new QLabel("insert()", insertCard); insT->setStyleSheet(titleStyle);
    QLabel *insD = new QLabel("Insert key node", insertCard); insD->setStyleSheet(descStyle);
    QLabel *insC = new QLabel("O(h)", insertCard); insC->setStyleSheet(compStyle);
    insTop->addWidget(insT); insTop->addWidget(insD); insTop->addStretch(); insTop->addWidget(insC);
    QHBoxLayout *insBot = new QHBoxLayout(); insertInput = new QLineEdit(insertCard); insertInput->setPlaceholderText("Key..."); insertInput->setStyleSheet(inputStyle);
    insertButton = new QPushButton("Insert", insertCard); insertButton->setStyleSheet(btnStyle);
    insBot->addWidget(insertInput); insBot->addWidget(insertButton); insL->addLayout(insTop); insL->addLayout(insBot);
    controlGrid->addWidget(insertCard, 0, 0);

    // --- Card 1: delete (0행 1열) ---
    QWidget *deleteCard = new QWidget(rightContentWidget); deleteCard->setAttribute(Qt::WA_StyledBackground, true); deleteCard->setStyleSheet(cardStyle);
    QVBoxLayout *delL = new QVBoxLayout(deleteCard); QHBoxLayout *delTop = new QHBoxLayout();
    QLabel *delT = new QLabel("delete()", deleteCard); delT->setStyleSheet(titleStyle);
    QLabel *delD = new QLabel("Remove key node", deleteCard); delD->setStyleSheet(descStyle);
    QLabel *delC = new QLabel("O(h)", deleteCard); delC->setStyleSheet(compStyle);
    delTop->addWidget(delT); delTop->addWidget(delD); delTop->addStretch(); delTop->addWidget(delC);
    QHBoxLayout *delBot = new QHBoxLayout(); deleteInput = new QLineEdit(deleteCard); deleteInput->setPlaceholderText("Key..."); deleteInput->setStyleSheet(inputStyle);
    deleteButton = new QPushButton("Trigger", deleteCard); deleteButton->setStyleSheet(triggerStyle);
    delBot->addWidget(deleteInput); delBot->addWidget(deleteButton); delL->addLayout(delTop); delL->addLayout(delBot);
    controlGrid->addWidget(deleteCard, 0, 1);

    // --- Card 2: search (1행 0열) ---
    QWidget *searchCard = new QWidget(rightContentWidget); searchCard->setAttribute(Qt::WA_StyledBackground, true); searchCard->setStyleSheet(cardStyle);
    QVBoxLayout *schL = new QVBoxLayout(searchCard); QHBoxLayout *schTop = new QHBoxLayout();
    QLabel *schT = new QLabel("search()", searchCard); schT->setStyleSheet(titleStyle);
    QLabel *schD = new QLabel("Find key node", searchCard); schD->setStyleSheet(descStyle);
    QLabel *schC = new QLabel("O(h)", searchCard); schC->setStyleSheet(compStyle);
    schTop->addWidget(schT); schTop->addWidget(schD); schTop->addStretch(); schTop->addWidget(schC);
    QHBoxLayout *schBot = new QHBoxLayout(); searchInput = new QLineEdit(searchCard); searchInput->setPlaceholderText("Key..."); searchInput->setStyleSheet(inputStyle);
    searchButton = new QPushButton("Search", searchCard); searchButton->setStyleSheet(btnStyle);
    schBot->addWidget(searchInput); schBot->addWidget(searchButton); schL->addLayout(schTop); schL->addLayout(schBot);
    controlGrid->addWidget(searchCard, 1, 0);

    // --- Card 3: min (1행 1열) ---
    QWidget *minCard = new QWidget(rightContentWidget); minCard->setAttribute(Qt::WA_StyledBackground, true); minCard->setStyleSheet(cardStyle);
    QVBoxLayout *minL = new QVBoxLayout(minCard); QHBoxLayout *minTop = new QHBoxLayout();
    QLabel *minT = new QLabel("min()", minCard); minT->setStyleSheet(titleStyle);
    QLabel *minD = new QLabel("Find minimum key", minCard); minD->setStyleSheet(descStyle);
    QLabel *minC = new QLabel("O(h)", minCard); minC->setStyleSheet(compStyle);
    minTop->addWidget(minT); minTop->addWidget(minD); minTop->addStretch(); minTop->addWidget(minC);
    QHBoxLayout *minBot = new QHBoxLayout(); minOutput = new QLabel("Ready", minCard); minOutput->setStyleSheet(outputStyle);
    minButton = new QPushButton("Peek", minCard); minButton->setStyleSheet(btnStyle);
    minBot->addWidget(minOutput, 1); minBot->addWidget(minButton); minL->addLayout(minTop); minL->addLayout(minBot);
    controlGrid->addWidget(minCard, 1, 1);

    // --- Card 4: max (2행 0열) ---
    QWidget *maxCard = new QWidget(rightContentWidget); maxCard->setAttribute(Qt::WA_StyledBackground, true); maxCard->setStyleSheet(cardStyle);
    QVBoxLayout *maxL = new QVBoxLayout(maxCard); QHBoxLayout *maxTop = new QHBoxLayout();
    QLabel *maxT = new QLabel("max()", maxCard); maxT->setStyleSheet(titleStyle);
    QLabel *maxD = new QLabel("Find maximum key", maxCard); maxD->setStyleSheet(descStyle);
    QLabel *maxC = new QLabel("O(h)", maxCard); maxC->setStyleSheet(compStyle);
    maxTop->addWidget(maxT); maxTop->addWidget(maxD); maxTop->addStretch(); maxTop->addWidget(maxC);
    QHBoxLayout *maxBot = new QHBoxLayout(); maxOutput = new QLabel("Ready", maxCard); maxOutput->setStyleSheet(outputStyle);
    maxButton = new QPushButton("Peek", maxCard); maxButton->setStyleSheet(btnStyle);
    maxBot->addWidget(maxOutput, 1); maxBot->addWidget(maxButton); maxL->addLayout(maxTop); maxL->addLayout(maxBot);
    controlGrid->addWidget(maxCard, 2, 0);

    // --- Card 5: is_empty (2행 1열) ---
    QWidget *ieCard = new QWidget(rightContentWidget); ieCard->setAttribute(Qt::WA_StyledBackground, true); ieCard->setStyleSheet(cardStyle);
    QVBoxLayout *ieL = new QVBoxLayout(ieCard); QHBoxLayout *ieTop = new QHBoxLayout();
    QLabel *ieT = new QLabel("is_empty()", ieCard); ieT->setStyleSheet(titleStyle);
    QLabel *ieD = new QLabel("Check emptiness", ieCard); ieD->setStyleSheet(descStyle);
    QLabel *ieC = new QLabel("O(1)", ieCard); ieC->setStyleSheet(compStyle);
    ieTop->addWidget(ieT); ieTop->addWidget(ieD); ieTop->addStretch(); ieTop->addWidget(ieC);
    QHBoxLayout *ieBot = new QHBoxLayout(); isEmptyOutput = new QLabel("Ready", ieCard); isEmptyOutput->setStyleSheet(outputStyle);
    isEmptyButton = new QPushButton("Check", ieCard); isEmptyButton->setStyleSheet(btnStyle);
    ieBot->addWidget(isEmptyOutput, 1); ieBot->addWidget(isEmptyButton); ieL->addLayout(ieTop); ieL->addLayout(ieBot);
    controlGrid->addWidget(ieCard, 2, 1);

    // --- Card 6: len (3행 0열) ---
    QWidget *lenCard = new QWidget(rightContentWidget); lenCard->setAttribute(Qt::WA_StyledBackground, true); lenCard->setStyleSheet(cardStyle);
    QVBoxLayout *lenL = new QVBoxLayout(lenCard); QHBoxLayout *lenTop = new QHBoxLayout();
    QLabel *lenT = new QLabel("len()", lenCard); lenT->setStyleSheet(titleStyle);
    QLabel *lenD = new QLabel("Get node count", lenCard); lenD->setStyleSheet(descStyle);
    QLabel *lenC = new QLabel("O(1)", lenCard); lenC->setStyleSheet(compStyle);
    lenTop->addWidget(lenT); lenTop->addWidget(lenD); lenTop->addStretch(); lenTop->addWidget(lenC);
    QHBoxLayout *lenBot = new QHBoxLayout(); lenOutput = new QLabel("Ready", lenCard); lenOutput->setStyleSheet(outputStyle);
    lenButton = new QPushButton("Size", lenCard); lenButton->setStyleSheet(btnStyle);
    lenBot->addWidget(lenOutput, 1); lenBot->addWidget(lenButton); lenL->addLayout(lenTop); lenL->addLayout(lenBot);
    controlGrid->addWidget(lenCard, 3, 0);

    // 🌟 --- 신규 Card 7: Animation Speed Control (3행 1열 배치로 대시보드 공백 결합) ---
    QWidget *speedCard = new QWidget(rightContentWidget); speedCard->setAttribute(Qt::WA_StyledBackground, true); speedCard->setStyleSheet(cardStyle);
    QVBoxLayout *speedL = new QVBoxLayout(speedCard); QHBoxLayout *speedTop = new QHBoxLayout();
    QLabel *spTitle = new QLabel("Interval Speed", speedCard); spTitle->setStyleSheet(titleStyle);
    QLabel *spDesc = new QLabel("Adjust refresh rate", speedCard); spDesc->setStyleSheet(descStyle);
    speedLabel = new QLabel("700 ms", speedCard); speedLabel->setStyleSheet("color: #56B6C2; font-size: 11px; font-weight: bold; font-family: monospace;");
    speedTop->addWidget(spTitle); speedTop->addWidget(spDesc); speedTop->addStretch(); speedTop->addWidget(speedLabel);
    QHBoxLayout *speedBot = new QHBoxLayout();
    speedSlider = new QSlider(Qt::Horizontal, speedCard); speedSlider->setRange(100, 2000); speedSlider->setValue(700); speedSlider->setStyleSheet(sliderStyle);
    speedBot->addWidget(speedSlider); speedL->addLayout(speedTop); speedL->addLayout(speedBot);
    controlGrid->addWidget(speedCard, 3, 1);

    rightLayout->addLayout(controlGrid);

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

    QObject::connect(insertButton, &QPushButton::clicked, [=]() { this->insert_op(); });
    QObject::connect(deleteButton, &QPushButton::clicked, [=]() { this->delete_op(); });
    QObject::connect(searchButton, &QPushButton::clicked, [=]() { this->search_op(); });
    QObject::connect(minButton, &QPushButton::clicked, [=]() { this->min_op(); });
    QObject::connect(maxButton, &QPushButton::clicked, [=]() { this->max_op(); });
    QObject::connect(isEmptyButton, &QPushButton::clicked, [=]() { this->is_empty(); });
    QObject::connect(lenButton, &QPushButton::clicked, [=]() { this->_len(); });

    // 🌟 슬라이더 조절 시 실시간 속도 조절 이벤트 바인딩
    QObject::connect(speedSlider, &QSlider::valueChanged, this, &TREE_BST::updateSpeed);

    this->updateVisuals();
    return containerWidget;
}

// 🌟 신규 슬롯 함수 구현: 슬라이더 조절 시 동적 타이머 간격 실시간 치환
void TREE_BST::updateSpeed(int value) {
    speedLabel->setText(QString("%1 ms").arg(value));
    if (animationTimer->isActive()) {
        // 현재 적용되어 있는 삭제 프로세스 상태 단계별 가중치 비율을 유지하며 interval을 업데이트합니다.
        int base = speedSlider->value();
        if (currentDeleteState == TARGET_FOUND) {
            // 기존 1000ms / 1200ms / 1500ms 의 고정 비례 반영
            animationTimer->setInterval(base * 1.4);
        } else if (currentDeleteState == FIND_REPLACE) {
            animationTimer->setInterval(base * 1.1);
        } else {
            animationTimer->setInterval(base);
        }
    }
}

// ==========================================
// 3. 시각화 노드 업데이트 (다크 테마 톤업 매핑)
// ==========================================
void TREE_BST::updateVisuals() {
    scene->clear();
    if (!root) return;

    double initial_x_offset = 240.0;
    drawNode(root, 0, 0.0, initial_x_offset);

    view->resetTransform();
    QRectF sceneRect = scene->itemsBoundingRect().adjusted(-60, -40, 60, 60);
    scene->setSceneRect(sceneRect);

    double viewWidth = view->viewport()->width();
    double viewHeight = view->viewport()->height();

    if (sceneRect.width() > viewWidth || sceneRect.height() > viewHeight) {
        view->fitInView(sceneRect, Qt::KeepAspectRatio);
    } else {
        view->centerOn(0, sceneRect.height() / 2 - 40);
    }
}

void TREE_BST::drawNode(BST_Node* node, int level, double x_pos, double x_offset) {
    if (!node) return;

    double y_pos = level * LEVEL_HEIGHT;

    // 기본 노드: 슬레이트 블랙 배경 + 다크 골드 테두리
    QPen pen(QColor("#D29922"), 2);
    QBrush brush(QColor("#161B22"));
    QColor textColor(QString("#E6EDF3"));

    // 상태 전이에 따른 하이라이트 색상 매핑 수정
    if (node->key == highlightTargetKey) {
        brush = QBrush(QColor("#F85149"));        // 타겟 노드: 로즈 레드 (소멸/변경 대상)
        pen = QPen(QColor("#FF7B72"), 2);
        textColor = QColor("#0D1117");
    } else if (node->key == highlightReplaceKey) {
        brush = QBrush(QColor("#238636"));        // 대체 노드: 네온 그린 (스왑 노드)
        pen = QPen(QColor("#3FB950"), 2);
        textColor = QColor("#FFFFFF");
    } else if (node->key == highlightSearchKey) {
        brush = QBrush(QColor("#1F6FEB"));        // 탐색 노드: 오션 블루 (현재 추적 포인트)
        pen = QPen(QColor("#58A6FF"), 2);
        textColor = QColor("#FFFFFF");
    }

    QGraphicsRectItem *rectItem = new QGraphicsRectItem(x_pos - NODE_WIDTH/2, y_pos, NODE_WIDTH, NODE_HEIGHT);
    rectItem->setPen(pen);
    rectItem->setBrush(brush);
    scene->addItem(rectItem);

    QGraphicsTextItem *textItem = new QGraphicsTextItem(QString::number(node->key));
    textItem->setDefaultTextColor(textColor);
    QFont font = textItem->font(); font.setBold(true); font.setPointSize(10);
    textItem->setFont(font);

    double textWidth = textItem->boundingRect().width();
    double textHeight = textItem->boundingRect().height();
    textItem->setPos(x_pos - textWidth/2, y_pos + (NODE_HEIGHT - textHeight)/2);
    scene->addItem(textItem);

    double next_x_offset = x_offset / 2.0;
    QPointF parentBottom(x_pos, y_pos + NODE_HEIGHT);

    // 간선 에지 펜 스타일 지정 (#30363D 다크 그레이 간선)
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

// =========================================================================
// 4. 타이머 기반 비동기 삭제 연산 루프 (속도 제어 가변화)
// =========================================================================
void TREE_BST::delete_op() {
    if (currentDeleteState != IDLE) return;

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

    statusOutput->setText(QString(">> Searching: Accessing Node [ %1 ] in BST hierarchy...").arg(currNode->key));
    updateVisuals();

    // 🌟 고정수치(700) 대신 슬라이더의 현재 속도 설정값을 추출하여 반영
    animationTimer->start(speedSlider->value());
}

void TREE_BST::deleteStep() {
    static int checkStep = 0;
    int spVal = speedSlider->value(); // 🌟 실시간 속도 기준값 변수 바인딩

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
            statusOutput->setText(QString(">> Target Found: Node [ %1 ] hit. Checking left topology state...").arg(currNode->key));
            updateVisuals();
            animationTimer->setInterval(spVal * 1.4); // 🌟 상대적 시간 지연 유지 (기존 1000ms 비율)
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
        animationTimer->setInterval(spVal); // 🌟 속도값 매핑
    }

    else if (currentDeleteState == TARGET_FOUND) {
        // [Step 0]: 왼쪽 자식 존재 여부 판별 분기
        if (checkStep == 0) {
            if (!currNode->left) {
                highlightSearchKey = -1;
                statusOutput->setText(QString(">> Mode: Left link is empty. Verification routing shifted to right link...").arg(currNode->key));
                checkStep = 2;
            } else {
                highlightSearchKey = currNode->left->key;
                statusOutput->setText(QString(">> Mode: Left child detected at [ %1 ]. Inspecting right link status...").arg(currNode->left->key));
                checkStep = 1;
            }
            updateVisuals();
            animationTimer->setInterval(spVal * 1.7); // 🌟 기존 1200ms 비율 대응 가중치
            return;
        }

        // [Step 1]: 왼쪽 자식 유효 상태에서 오른쪽 자식 링크 추가 검사
        else if (checkStep == 1) {
            if (currNode->right) {
                highlightSearchKey = currNode->right->key;

                // 양쪽 자식이 모두 가득 차 있으므로 [Case 3] 내부 PPT 서브조건 판별
                bool isRootNode = (parentNode == nullptr);
                bool leftChildHasRight = (currNode->left->right != nullptr);
                QString caseStr = "";

                if (isRootNode) {
                    if (leftChildHasRight) {
                        caseStr = ">> Case 3b: Root element swap needed. Left child contains valid right sub-hierarchy.";
                    } else {
                        caseStr = ">> Case 3a: Root element swap needed. Left child has no further right branches.";
                    }
                } else {
                    bool deletedIsLeft = (parentNode->left == currNode);
                    if (deletedIsLeft && leftChildHasRight) {
                        caseStr = ">> Case 3d: Target is a left branch node, its left child contains right sub-hierarchy.";
                    } else if (!deletedIsLeft && leftChildHasRight) {
                        caseStr = ">> Case 3d: Target is a right branch node, its left child contains right sub-hierarchy.";
                    } else if (deletedIsLeft && !leftChildHasRight) {
                        caseStr = ">> Case 3c: Target is a left branch node, its left child has no right sub-hierarchy.";
                    } else {
                        caseStr = ">> Case 3c: Target is a right branch node, its left child has no right sub-hierarchy.";
                    }
                }

                statusOutput->setText(QString("%1").arg(caseStr));

                // 후계자(Replacement) 탐색 모드 전환
                replaceParent = currNode;
                replaceNode = currNode->left;
                highlightReplaceKey = replaceNode->key;
                highlightSearchKey = -1;

                currentDeleteState = FIND_REPLACE;
                updateVisuals();
                animationTimer->setInterval(spVal * 2.1); // 🌟 기존 1500ms 비율 대응 가중치
                return;
            } else {
                // 왼쪽 자식만 단독 존재 [Case 2]
                bool isLeftChild = (parentNode && parentNode->left == currNode);
                QString caseStr = "";
                if (!parentNode) caseStr = ">> Case 2: Tree root contains only a single left node hierarchy.";
                else if (isLeftChild) caseStr = ">> Case 2: Target is left branch with only a left child node link.";
                else caseStr = ">> Case 2: Target is right branch with only a left child node link.";

                statusOutput->setText(QString("%1").arg(caseStr));

                size--;
                BST_Node* temp = currNode->left;
                if (!parentNode) root = temp;
                else if (parentNode->left == currNode) parentNode->left = temp;
                else parentNode->right = temp;

                delete currNode;

                animationTimer->stop();
                currentDeleteState = IDLE;
                highlightTargetKey = -1;
                highlightSearchKey = -1;
                setControlsEnabled(true);
                updateVisuals();
                return;
            }
        }

        // [Step 2]: 왼쪽 자식이 없는 상태에서 오른쪽 자식 링크 단독 판별
        else if (checkStep == 2) {
            if (currNode->right) {
                highlightSearchKey = currNode->right->key;

                // 오른쪽 자식만 단독 존재 [Case 2]
                bool isLeftChild = (parentNode && parentNode->left == currNode);
                QString caseStr = "";
                if (!parentNode) caseStr = ">> Case 2: Tree root contains only a single right node hierarchy.";
                else if (isLeftChild) caseStr = ">> Case 2: Target is left branch with only a right child node link.";
                else caseStr = ">> Case 2: Target is right branch with only a right child node link.";

                statusOutput->setText(QString("%1").arg(caseStr));

                size--;
                BST_Node* temp = currNode->right;
                if (!parentNode) root = temp;
                else if (parentNode->left == currNode) parentNode->left = temp;
                else parentNode->right = temp;

                delete currNode;

                animationTimer->stop();
                currentDeleteState = IDLE;
                highlightTargetKey = -1;
                highlightSearchKey = -1;
                setControlsEnabled(true);
                updateVisuals();
                return;
            } else {
                // 자식이 둘 다 없는 단독 리프 노드 [Case 1]
                statusOutput->setText(QString(">> Case 1: Targeted item is a terminal Leaf node. Safe removal executing..."));

                size--;
                if (!parentNode) root = nullptr;
                else if (parentNode->left == currNode) parentNode->left = nullptr;
                else parentNode->right = nullptr;

                delete currNode;

                animationTimer->stop();
                currentDeleteState = IDLE;
                highlightTargetKey = -1;
                highlightSearchKey = -1;
                setControlsEnabled(true);
                updateVisuals();
                return;
            }
        }
    }

    else if (currentDeleteState == FIND_REPLACE) {
        // 왼쪽 서브트리의 최댓값(In-order Predecessor) 추적을 위해 우측 링크 끝까지 추적
        if (replaceNode->right) {
            replaceParent = replaceNode;
            replaceNode = replaceNode->right;
            highlightReplaceKey = replaceNode->key;
            statusOutput->setText(QString(">> Strategy: Seeking maximum candidate key via right-most pipeline -> [ %1 ]").arg(replaceNode->key));
            updateVisuals();
            animationTimer->setInterval(spVal * 1.1); // 🌟 기존 800ms 비율 대응
        } else {
            statusOutput->setText(QString(">> Strategy: Maximum key value discovered [ %1 ]. Initializing value overriding process.").arg(replaceNode->key));
            currentDeleteState = COPY_AND_DELETE;
            animationTimer->setInterval(spVal * 1.7); // 🌟 기존 1200ms 비율 대응
        }
    }

    else if (currentDeleteState == COPY_AND_DELETE) {
        // 1. 후계자 노드의 값을 타겟 노드로 복사 복사
        currNode->key = replaceNode->key;
        highlightTargetKey = currNode->key;
        highlightReplaceKey = -1;
        updateVisuals();

        // 2. 링크 변경 구조 승격 승환
        BST_Node* child = replaceNode->left;
        if (replaceParent->left == replaceNode) replaceParent->left = child;
        else replaceParent->right = child;

        delete replaceNode;

        // 3. 루프 정상 종료 및 제어 플래그 초기화
        animationTimer->stop();
        currentDeleteState = IDLE;
        highlightTargetKey = -1;
        setControlsEnabled(true);
        updateVisuals();
        statusOutput->setText(QString(">> Success: Target element securely removed. Struct validation passed."));
    }
}

// ==========================================
// 5. 기본 제어 연산 로직 구현 (로그 규칙 통일)
// ==========================================
void TREE_BST::insert_op() {
    bool ok; int val = insertInput->text().toInt(&ok);
    if (!ok) { statusOutput->setText(">> Error: Please verify the integer conversion casting."); return; }

    if (!root) {
        root = new BST_Node(val);
        size++;
        statusOutput->setText(">> Success: Assigned key value " + QString::number(val) + " as the initial Root node.");
    }
    else {
        BST_Node* C = root; BST_Node* P = nullptr;
        while (C != nullptr) {
            if (val == C->key) { statusOutput->setText(">> Error: Key duplicate restriction! Value already exists."); return; }
            P = C;
            if (val < C->key) C = C->left; else C = C->right;
        }
        if (val < P->key) P->left = new BST_Node(val); else P->right = new BST_Node(val);
        size++;
        statusOutput->setText(QString(">> Success: Key node [ %1 ] has been added to tree.").arg(val));
    }
    insertInput->clear(); updateVisuals();
}

void TREE_BST::search_op() {
    bool ok; int val = searchInput->text().toInt(&ok);
    if (!ok) { statusOutput->setText(">> Error: Unable to cast input parameter to a valid Integer."); return; }

    BST_Node* C = root;
    while (C != nullptr) {
        if (val == C->key) { statusOutput->setText(QString(">> Success: Binary search hit! Key [ %1 ] is active.").arg(val)); return; }
        if (val < C->key) C = C->left; else C = C->right;
    }
    statusOutput->setText(">> Failure: Requested key does not belong to this tree entity.");
}

void TREE_BST::min_op() {
    if(!root) { statusOutput->setText(">> Warning: Search terminated. Node size is 0."); return; }
    BST_Node* c = root;
    while(c->left) c = c->left;
    minOutput->setText(QString::number(c->key));
    statusOutput->setText(">> Inspection: Left-bound scan hit. Minimum key value is " + QString::number(c->key));
}

void TREE_BST::max_op() {
    if(!root) { statusOutput->setText(">> Warning: Search terminated. Node size is 0."); return; }
    BST_Node* c = root;
    while(c->right) c = c->right;
    maxOutput->setText(QString::number(c->key));
    statusOutput->setText(">> Inspection: Right-bound scan hit. Maximum key value is " + QString::number(c->key));
}

void TREE_BST::is_empty() {
    if (size == 0) { isEmptyOutput->setText("True"); statusOutput->setText(">> Inspection: is_empty() -> TRUE (Tree contains zero valid references)."); }
    else { isEmptyOutput->setText("False"); statusOutput->setText(">> Inspection: is_empty() -> FALSE (Active binary nodes verified)."); }
}

void TREE_BST::_len() {
    lenOutput->setText(QString::number(size));
    statusOutput->setText(">> Inspection: len() -> Active entity nodes tracked in memory: " + QString::number(size));
}