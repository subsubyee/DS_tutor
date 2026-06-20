#include "tree_sort.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainterPath>
#include <QGraphicsTextItem>
#include <QFont>
#include <QGraphicsSceneMouseEvent>

// ==========================================
// UI 렌더링 기하학 상수 정의 (기존 사양 완벽 일치)
// ==========================================
static const double NODE_WIDTH = 70.0;
static const double NODE_HEIGHT = 42.0;
static const double LEVEL_HEIGHT = 85.0;

// 커스텀 그래픽스 아이템 마우스 클릭 이벤트 구현
TreeNodeItem::TreeNodeItem(int k, TREE_SORT* manager, QGraphicsItem* parent)
    : QGraphicsPathItem(parent), key(k), sortManager(manager) {
    setAcceptHoverEvents(true);
}

void TreeNodeItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (sortManager) {
        sortManager->handleNodeClick(key);
    }
    QGraphicsPathItem::mousePressEvent(event);
}

// TREE_SORT 클래스 구현
TREE_SORT::TREE_SORT(QMainWindow* _parent)
    : QObject(_parent), root(nullptr), size(0), currentFocusNode(nullptr), quizTargetKey(-1), lastWrongClickedKey(-1), currentSortIndex(0) {
    parent = _parent;
}

TREE_SORT::~TREE_SORT() {
    clearTree(root);
}

void TREE_SORT::clearTree(tree_node* node) {
    if (node) {
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
}

QWidget* TREE_SORT::setting() {
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet("QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI'; }");

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // [좌측 영역] 오직 시각화 뷰(캔버스)만 배치 (요청 사양: 나머지 모든 버튼 제거)
    QGroupBox *leftGroup = new QGroupBox(containerWidget);
    leftGroup->setStyleSheet("QGroupBox { border: none; }");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftGroup);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nBinary Tree Sort Interactive Quiz", leftGroup);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px;");
    leftLayout->addWidget(visTitle);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, leftGroup);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QBrush(QColor("#11141A")));
    view->setStyleSheet("QGraphicsView { border: 1px solid #21262D; border-radius: 12px; }");
    leftLayout->addWidget(view);

    // [우측 영역] 제어 대시보드 및 터미널 로그 통합
    QScrollArea *rightScrollArea = new QScrollArea(containerWidget);
    rightScrollArea->setWidgetResizable(true);
    rightScrollArea->setFixedWidth(420);
    rightScrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    QWidget *rightContent = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContent);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(16);

    QLabel *ctrlTitle = new QLabel("● CONTROLS\n\nSort Quiz Operations Dashboard", rightContent);
    ctrlTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #8B949E;");
    rightLayout->addWidget(ctrlTitle);

    // Modern Obsidian Dark 스타일시트 템플릿
    QString cardStyle = "QWidget { background-color: #161B22; border: 1px solid #21262D; border-radius: 10px; }";
    QString btnStyle = "QPushButton { background-color: #238636; color: white; border-radius: 6px; padding: 10px; font-weight: bold; border: none; } QPushButton:hover { background-color: #2EA043; }";
    QString resetBtnStyle = "QPushButton { background-color: #21262D; color: #F85149; border: 1px solid #30363D; border-radius: 6px; padding: 10px; font-weight: bold; } QPushButton:hover { background-color: #30363D; }";

    // 컨트롤 카드 패널
    QWidget *controlCard = new QWidget();
    controlCard->setStyleSheet(cardStyle);
    QVBoxLayout *cardLayout = new QVBoxLayout(controlCard);
    cardLayout->setSpacing(12);

    QLabel *instruction = new QLabel("Click the nodes in the correct order of Binary Tree Sort (In-order / LNR Traversal).", controlCard);
    instruction->setWordWrap(true);
    instruction->setStyleSheet("color: #8B949E; font-size: 12px; border: none;");
    cardLayout->addWidget(instruction);

    generateButton = new QPushButton("Generate Random Quiz Tree");
    generateButton->setStyleSheet(btnStyle);
    cardLayout->addWidget(generateButton);

    resetButton = new QPushButton("Reset Quiz State");
    resetButton->setStyleSheet(resetBtnStyle);
    cardLayout->addWidget(resetButton);

    rightLayout->addWidget(controlCard);

    // 라이브 시스템 로그 터미널
    statusOutput = new QLabel(">> System Ready. Click 'Generate Random Quiz Tree' to begin.");
    statusOutput->setWordWrap(true);
    statusOutput->setStyleSheet("background-color: #0D1117; border: 1px solid #21262D; color: #58A6FF; font-family: 'Consolas'; padding: 14px; min-height: 180px; font-size: 12px; border-radius: 10px;");
    statusOutput->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    rightLayout->addWidget(statusOutput);

    rightScrollArea->setWidget(rightContent);

    mainLayout->addWidget(leftGroup, 1);
    mainLayout->addWidget(rightScrollArea);

    // 이벤트 시그널 바인딩
    connect(generateButton, &QPushButton::clicked, this, &TREE_SORT::startQuiz);
    connect(resetButton, &QPushButton::clicked, this, &TREE_SORT::resetQuiz);

    return containerWidget;
}

// 요청하신 랜덤 트리 생성 알고리즘 본체 (tree_node 구조에 맞춰 완벽 이식)
void TREE_SORT::generateRandomTree() {
    clearTree(root);
    root = nullptr;
    size = 0;

    int totalNodesCount = QRandomGenerator::global()->bounded(8, 11);

    QSet<int> uniqueNumbers;
    while (uniqueNumbers.size() < totalNodesCount) {
        int randomVal = QRandomGenerator::global()->bounded(1, 101);
        uniqueNumbers.insert(randomVal);
    }

    QList<int> randomKeys = uniqueNumbers.values();

    for (int i = 0; i < randomKeys.size(); ++i) {
        int val = randomKeys[i];
        if (!root) {
            root = new tree_node(val);
            size++;
        } else {
            tree_node* C = root;
            tree_node* P = nullptr;
            while (C != nullptr) {
                P = C;
                if (val < C->key) C = C->left;
                else C = C->right;
            }
            if (val < P->key) P->left = new tree_node(val);
            else P->right = new tree_node(val);
            size++;
        }
    }

    if (root != nullptr) {
        QList<tree_node*> q = {root};
        while(!q.isEmpty()){
            tree_node* curr = q.takeFirst();
            if(curr){
                curr->isRevealed = true;
                curr->leftChecked = true;
                curr->rightChecked = true;
                q.append(curr->left);
                q.append(curr->right);
            }
        }
        currentFocusNode = root;

        QList<int> targetCandidates = randomKeys;
        targetCandidates.removeAll(root->key);
        if (targetCandidates.isEmpty()) targetCandidates.append(root->key);

        int randIdx = QRandomGenerator::global()->bounded(targetCandidates.size());
        quizTargetKey = targetCandidates[randIdx];
    } else {
        currentFocusNode = nullptr;
    }
}

// 정답 비교용 In-order(LNR) 순회 함수
void TREE_SORT::calculateInOrder(tree_node* node, QVector<int>& order) {
    if (!node) return;
    calculateInOrder(node->left, order);
    order.append(node->key);
    calculateInOrder(node->right, order);
}

void TREE_SORT::startQuiz() {
    generateRandomTree();

    // 퀴즈 상태 초기화
    correctOrder.clear();
    correctClickedNodes.clear();
    lastWrongClickedKey = -1;
    currentSortIndex = 0;

    if (root) {
        calculateInOrder(root, correctOrder);
        statusOutput->setText(QString(">> Quiz Started! Tree built with %1 nodes.\n>> Task: Click the nodes in order from smallest to largest value (Binary Tree Sort).").arg(size));
    }
    updateVisuals();
}

void TREE_SORT::resetQuiz() {
    correctClickedNodes.clear();
    lastWrongClickedKey = -1;
    currentSortIndex = 0;
    if (root) {
        statusOutput->setText(">> Quiz Reset. Progress cleared. Try traversing again!");
    } else {
        statusOutput->setText(">> No tree available. Click 'Generate Random Quiz Tree' first.");
    }
    updateVisuals();
}

// 노드 선택 시 올바른 정렬 과정인지 검증하는 핵심 핸들러
void TREE_SORT::handleNodeClick(int clickedKey) {
    if (currentSortIndex >= correctOrder.size()) {
        statusOutput->setText(">> Complete: You have already successfully sorted all nodes!");
        return;
    }

    // 올바른 순서의 노드를 클릭했을 때 (정답 처리)
    if (clickedKey == correctOrder[currentSortIndex]) {
        correctClickedNodes.insert(clickedKey);
        currentSortIndex++;
        lastWrongClickedKey = -1; // 오답 하이라이트 초기화

        QString logText = QString(">> Correct! [ %1 ] is the right next node.\n>> Sorted Sequence: ").arg(clickedKey);
        for (int i = 0; i < currentSortIndex; ++i) {
            logText += QString("[ %1 ] ").arg(correctOrder[i]);
        }
        statusOutput->setText(logText);

        if (currentSortIndex == correctOrder.size()) {
            statusOutput->setText(statusOutput->text() + "\n\n🎉 Success! You completed the Binary Tree Sort perfectly!");
        }
    } else {
        // 이미 정렬 완료된 노드를 다시 클릭한 경우
        if (correctClickedNodes.contains(clickedKey)) {
            statusOutput->setText(QString(">> Note: Node [ %1 ] is already correctly sorted and locked.").arg(clickedKey));
            return;
        }
        // 정렬 순서가 틀린 노드를 클릭했을 때 (오답 처리)
        lastWrongClickedKey = clickedKey;
        statusOutput->setText(QString(">> Wrong Click! Node [ %1 ] is not the correct next item.\n>> Hint: Find the smallest value among the unvisited nodes.").arg(clickedKey));
    }
    updateVisuals();
}

void TREE_SORT::updateVisuals() {
    scene->clear();
    if (root) drawNode(root, 0, 0.0, 200.0);
    QRectF bounds = scene->itemsBoundingRect();
    scene->setSceneRect(bounds.adjusted(-60, -20, 60, 60));
}

void TREE_SORT::drawNode(tree_node* node, int level, double x_pos, double x_offset) {
    if (!node) return;
    double y_pos = level * LEVEL_HEIGHT;

    // 기본 디자인 펜 및 브러시 세팅
    QPen pen(QColor("#21262D"), 2);
    QBrush brush(QColor("#161B22"));
    QColor textColor(QColor("#E6EDF3"));

    // 인터랙티브 클릭 상태에 따른 다이나믹 색상 변경 분기
    if (correctClickedNodes.contains(node->key)) {
        brush = QBrush(QColor("#238636")); // 올바른 정렬 상태: Modern Green
        pen = QPen(QColor("#3FB950"), 2);
    } else if (node->key == lastWrongClickedKey) {
        brush = QBrush(QColor("#DA3633")); // 잘못된 정렬 상태: Modern Red
        pen = QPen(QColor("#F85149"), 2);
    } else {
        pen = QPen(QColor("#D29922"), 2);  // 대기 상태 테두리: 오리지널 골드 피치 일치
    }

    // 클릭 감지가 가능한 커스텀 그래픽스 아이템 레이어로 씬에 추가
    TreeNodeItem *nodeItem = new TreeNodeItem(node->key, this);
    QPainterPath path;
    path.addRoundedRect(x_pos - NODE_WIDTH/2, y_pos, NODE_WIDTH, NODE_HEIGHT, 6.0, 6.0);
    nodeItem->setPath(path);
    nodeItem->setPen(pen);
    nodeItem->setBrush(brush);
    scene->addItem(nodeItem);

    // 내부 텍스트 렌더링
    QGraphicsTextItem *textItem = new QGraphicsTextItem(QString::number(node->key));
    textItem->setDefaultTextColor(textColor);
    QFont font = textItem->font();
    font.setBold(true); font.setPointSize(10);
    textItem->setFont(font);

    double textWidth = textItem->boundingRect().width();
    double textHeight = textItem->boundingRect().height();
    textItem->setPos(x_pos - textWidth/2, y_pos + (NODE_HEIGHT - textHeight)/2);
    scene->addItem(textItem);

    // 지정 사양 가로 배율 폭 0.44 정확히 적용
    double next_x_offset = x_offset * 0.44;

    if (node->left) {
        scene->addLine(x_pos, y_pos + NODE_HEIGHT, x_pos - x_offset, (level+1)*LEVEL_HEIGHT, QPen(QColor("#30363D"), 2));
        drawNode(node->left, level + 1, x_pos - x_offset, next_x_offset);
    }
    if (node->right) {
        scene->addLine(x_pos, y_pos + NODE_HEIGHT, x_pos + x_offset, (level+1)*LEVEL_HEIGHT, QPen(QColor("#30363D"), 2));
        drawNode(node->right, level + 1, x_pos + x_offset, next_x_offset);
    }
}