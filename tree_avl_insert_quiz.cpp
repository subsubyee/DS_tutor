#include "tree_avl_insert_quiz.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <cmath>
#include <algorithm>
#include <functional>
#include <QGroupBox>

// ==========================================
// 1. UI 렌더링 기하학 및 글로벌 크기 상수 정의
// ==========================================
static constexpr double NODE_WIDTH = 55.0;
static constexpr double NODE_HEIGHT = 35.0;
static constexpr double LEVEL_HEIGHT = 65.0;
static constexpr int BUTTON_FIXED_HEIGHT = 36; // 모든 제어 버튼의 세로 크기를 일괄 고정

TREE_AVL_INSERT_QUIZ::TREE_AVL_INSERT_QUIZ(QMainWindow* _parent) : QObject(_parent) {
    parent = _parent;
    avlRoot = nullptr;
    currentFocusNode = nullptr;
    isActualInserted = false;
    quizTargetInsertKey = -1;
    currentQuizState = QuizState::STEP1_PRE_START;
    currentStepIndex = 0;
    expectedSteps.clear();
    userSelectedNodes.clear();
}

TREE_AVL_INSERT_QUIZ::~TREE_AVL_INSERT_QUIZ() {
    clearTree(avlRoot);
}

void TREE_AVL_INSERT_QUIZ::clearTree(Quiz_AVL_Ins_Node* node) {
    if (node) {
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
}

void TREE_AVL_INSERT_QUIZ::updateHeight(Quiz_AVL_Ins_Node* n) {
    if (n) {
        int lh = (n->left && !n->left->isVirtual) ? n->left->height : 0;
        int rh = (n->right && !n->right->isVirtual) ? n->right->height : 0;
        n->height = 1 + std::max(lh, rh);
    }
}

int TREE_AVL_INSERT_QUIZ::getBalance(Quiz_AVL_Ins_Node* n) {
    if (!n) return 0;
    int lh = (n->left && !n->left->isVirtual) ? n->left->height : 0;
    int rh = (n->right && !n->right->isVirtual) ? n->right->height : 0;
    return lh - rh;
}

void TREE_AVL_INSERT_QUIZ::attachVirtualNodes(Quiz_AVL_Ins_Node* node) {
    if (!node || node->isVirtual) return;

    if (!node->left) {
        node->left = new Quiz_AVL_Ins_Node(-1, true, node);
    } else {
        attachVirtualNodes(node->left);
    }

    if (!node->right) {
        node->right = new Quiz_AVL_Ins_Node(-1, true, node);
    } else {
        attachVirtualNodes(node->right);
    }
}

void TREE_AVL_INSERT_QUIZ::detachVirtualNodes(Quiz_AVL_Ins_Node* node) {
    if (!node || node->isVirtual) return;

    if (node->left && node->left->isVirtual) {
        delete node->left;
        node->left = nullptr;
    } else {
        detachVirtualNodes(node->left);
    }

    if (node->right && node->right->isVirtual) {
        delete node->right;
        node->right = nullptr;
    } else {
        detachVirtualNodes(node->right);
    }
}

// ==========================================
// 2. UI 레이아웃 세팅 (오차 없는 치수 통일 버전)
// ==========================================
QWidget* TREE_AVL_INSERT_QUIZ::setting() {
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet(
        "QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }"
        );

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION CANVAS
    // ==========================================
    QGroupBox *leftGroup = new QGroupBox(containerWidget);
    leftGroup->setStyleSheet("QGroupBox { border: none; background: transparent; }");

    QVBoxLayout *leftLayout = new QVBoxLayout(leftGroup);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(12);

    // 제목과 설명문 간격 가독성 정밀 보정
    QLabel *visTitle = new QLabel("● AVL INSERTION CANVAS\n\nInteractive Balanced Tree Insertion & Rebalancing Sandbox", leftGroup);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; line-height: 1.4;");
    visTitle->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    leftLayout->addWidget(visTitle);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, leftGroup);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QBrush(QColor("#11141A")));
    view->setStyleSheet("QGraphicsView { border: 1px solid #21262D; border-radius: 12px; background-color: #11141A; }");

    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    view->viewport()->installEventFilter(this);
    leftLayout->addWidget(view, 1);

    // ==========================================
    // [Right Area] CONTROL PANEL
    // ==========================================
    QScrollArea *rightScrollArea = new QScrollArea(containerWidget);
    rightScrollArea->setWidgetResizable(true);
    rightScrollArea->setFixedWidth(420);
    rightScrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *rightContentWidget = new QWidget();
    rightContentWidget->setObjectName("RightContent");
    rightContentWidget->setStyleSheet("QWidget#RightContent { background: transparent; }");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightContentWidget);
    rightLayout->setContentsMargins(0, 0, 8, 0);
    rightLayout->setSpacing(14); // 패널 아이템 간 간격 통일

    // 미션 안내 카드 상단 배치
    quizQuestionLabel = new QLabel("Press 'Initialize Insertion Quiz' to play.", rightContentWidget);
    quizQuestionLabel->setWordWrap(true);
    quizQuestionLabel->setAlignment(Qt::AlignCenter);
    quizQuestionLabel->setStyleSheet(
        "color: #8B949E; background-color: #161B22; font-size: 13px; font-weight: bold; "
        "padding: 16px; border: 2px dashed #30363D; border-radius: 10px;"
        );
    rightLayout->addWidget(quizQuestionLabel);

    // 서브 타이틀 줄바꿈 밸런스 조정
    QLabel *controlsTitle = new QLabel("● SYSTEM CONTROL DASHBOARD", rightContentWidget);
    controlsTitle->setStyleSheet("font-size: 11px; font-weight: bold; color: #8B949E; letter-spacing: 1px; margin-top: 6px;");
    rightLayout->addWidget(controlsTitle);

    QGridLayout *controlGrid = new QGridLayout();
    controlGrid->setSpacing(12);

    // 스타일시트 및 버튼 세로폭 일괄 통일 정책
    QString cardStyle   = "QWidget { background-color: #161B22; border: 1px solid #21262D; border-radius: 10px; }";
    QString descStyle   = "color: #8B949E; font-size: 11px; background: transparent; border: none;";
    QString stepStyle   = "color: #56B6C2; font-size: 11px; font-weight: bold; font-family: monospace; background: transparent; border: none;";

    QString genBtnStyle = QString(
                              "QPushButton { background-color: #238636; color: #FFFFFF; border-radius: 6px; padding: 0px 16px; font-weight: bold; border: none; height: %1px; }"
                              "QPushButton:hover { background-color: #2EA043; }"
                              "QPushButton:pressed { background-color: #248039; }"
                              ).arg(BUTTON_FIXED_HEIGHT);

    QString actionBtnStyle = QString(
                                 "QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 0px 14px; font-weight: bold; border: 1px solid #30363D; height: %1px; }"
                                 "QPushButton:disabled { background-color: #161B22; color: #484F58; border: 1px solid #21262D; }"
                                 ).arg(BUTTON_FIXED_HEIGHT);

    // --- Card 1: Quiz Init ---
    QWidget *initCard = new QWidget(rightContentWidget); initCard->setAttribute(Qt::WA_StyledBackground, true); initCard->setStyleSheet(cardStyle);
    QHBoxLayout *initL = new QHBoxLayout(initCard); initL->setContentsMargins(14, 12, 14, 12);
    QLabel *initT = new QLabel("Step 1", initCard); initT->setStyleSheet(stepStyle);
    QLabel *initD = new QLabel("Deploy Framework", initCard); initD->setStyleSheet(descStyle);
    startQuizButton = new QPushButton("Initialize Quiz", initCard);
    startQuizButton->setStyleSheet(genBtnStyle);
    startQuizButton->setFixedHeight(BUTTON_FIXED_HEIGHT); // 하드웨어 렌더링 세로길이 강제 고정
    initL->addWidget(initT); initL->addWidget(initD); initL->addStretch(); initL->addWidget(startQuizButton);
    controlGrid->addWidget(initCard, 0, 0);

    // --- Card 2: Rebalance Verification ---
    QWidget *rotateCard = new QWidget(rightContentWidget); rotateCard->setAttribute(Qt::WA_StyledBackground, true); rotateCard->setStyleSheet(cardStyle);
    QHBoxLayout *rotL = new QHBoxLayout(rotateCard); rotL->setContentsMargins(14, 12, 14, 12);
    QLabel *rotT = new QLabel("Step 2", rotateCard); rotT->setStyleSheet(stepStyle);
    QLabel *rotD = new QLabel("Verify Rotations", rotateCard); rotD->setStyleSheet(descStyle);
    rotateButton = new QPushButton("Execute Rotation", rotateCard);
    rotateButton->setEnabled(false);
    rotateButton->setStyleSheet(actionBtnStyle);
    rotateButton->setFixedHeight(BUTTON_FIXED_HEIGHT); // 하드웨어 렌더링 세로길이 강제 고정
    rotL->addWidget(rotT); rotL->addWidget(rotD); rotL->addStretch(); rotL->addWidget(rotateButton);
    controlGrid->addWidget(rotateCard, 1, 0);

    rightLayout->addLayout(controlGrid);

    // --- Live Log 디스플레이 영역 ---
    QLabel *logTitle = new QLabel(">_ Interactive Quiz Live Log", rightContentWidget);
    logTitle->setStyleSheet("color: #8B949E; font-size: 11px; font-family: monospace; font-weight: bold; margin-top: 4px;");
    rightLayout->addWidget(logTitle);

    statusOutput = new QLabel("Quiz Engine ready. Framework suspended.", rightContentWidget);
    statusOutput->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    statusOutput->setWordWrap(true);
    statusOutput->setStyleSheet(
        "background-color: #0D1117; border: 1px solid #21262D; border-radius: 8px; "
        "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
        );
    rightLayout->addWidget(statusOutput);

    rightScrollArea->setWidget(rightContentWidget);

    mainLayout->addWidget(leftGroup, 1);
    mainLayout->addWidget(rightScrollArea);

    connect(startQuizButton, &QPushButton::clicked, this, &TREE_AVL_INSERT_QUIZ::startNewQuiz);
    connect(rotateButton, &QPushButton::clicked, this, &TREE_AVL_INSERT_QUIZ::executeRotation);

    this->updateVisuals();
    return containerWidget;
}

// ==========================================
// 3. 인터랙션 및 필터 컨텍스트
// ==========================================
bool TREE_AVL_INSERT_QUIZ::eventFilter(QObject* obj, QEvent* event) {
    if (obj == view->viewport() && event->type() == QEvent::Resize) {
        return true;
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QGraphicsItem* item = view->itemAt(mouseEvent->pos());
        if (!item) return QObject::eventFilter(obj, event);

        QVariant data = item->data(0);
        if (!data.isValid()) return QObject::eventFilter(obj, event);

        Quiz_AVL_Ins_Node* clickedNode = reinterpret_cast<Quiz_AVL_Ins_Node*>(data.value<void*>());
        if (!clickedNode) return QObject::eventFilter(obj, event);

        if (currentQuizState == QuizState::STEP2_CLICK_EMPTY_NODE) {
            if (clickedNode->isVirtual) {
                Quiz_AVL_Ins_Node* p = clickedNode->virtual_parent;
                bool isCorrectPosition = false;

                if (quizTargetInsertKey < p->key && p->left == clickedNode) isCorrectPosition = true;
                if (quizTargetInsertKey > p->key && p->right == clickedNode) isCorrectPosition = true;

                if (isCorrectPosition) {
                    detachVirtualNodes(avlRoot);

                    if (quizTargetInsertKey < p->key) p->left = new Quiz_AVL_Ins_Node(quizTargetInsertKey);
                    else p->right = new Quiz_AVL_Ins_Node(quizTargetInsertKey);

                    isActualInserted = true;
                    currentQuizState = QuizState::STEP4_REBALANCE;

                    if (expectedSteps.isEmpty()) {
                        quizQuestionLabel->setStyleSheet(
                            "color: #56B6C2; background-color: #0E161B; font-size: 13px; font-weight: bold; "
                            "padding: 16px; border: 2px solid #56B6C2; border-radius: 10px;"
                            );
                        quizQuestionLabel->setText("STRUCTURE SECURED\nTree remains balanced. Click Verification to finish.");

                        rotateButton->setEnabled(true);
                        rotateButton->setStyleSheet(
                            "QPushButton { background-color: #238636; color: #FFFFFF; border-radius: 6px; padding: 0px 16px; font-weight: bold; border: none; height: 36px; }"
                            "QPushButton:hover { background-color: #2EA043; }"
                            );
                        statusOutput->setText(">> Topo Integrity Verified. Trigger Verification to complete.");
                    } else {
                        quizQuestionLabel->setStyleSheet(
                            "color: #F0883E; background-color: #1A1510; font-size: 13px; font-weight: bold; "
                            "padding: 16px; border: 2px solid #D29922; border-radius: 10px;"
                            );
                        quizQuestionLabel->setText(QString("IMBALANCE DETECTED (Required Rebalances: %1)\nAssert Pivot A, Child B, and Grandchild C sequentially.").arg(expectedSteps.size()));

                        statusOutput->setStyleSheet(
                            "background-color: #0D1117; border: 1px solid #21262D; border-radius: 8px; "
                            "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
                            );
                        statusOutput->setText(">> Structure updated. Map the structural imbalance path (A -> B -> C) inside viewport canvas.");
                    }
                    updateVisuals();
                } else {
                    if (p->left == clickedNode) p->left = nullptr;
                    else if (p->right == clickedNode) p->right = nullptr;
                    delete clickedNode;

                    statusOutput->setStyleSheet(
                        "background-color: #1C1212; border: 1px solid #F85149; border-radius: 8px; "
                        "color: #F85149; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
                        );
                    statusOutput->setText(">> Error: Incorrect placement boundary! That selection violates BST conditions. Try another placeholder.");
                    updateVisuals();
                }
                return true;
            }
        }

        if (currentQuizState == QuizState::STEP4_REBALANCE) {
            if (clickedNode->isVirtual) return true;

            if (userSelectedNodes.contains(clickedNode)) {
                userSelectedNodes.removeOne(clickedNode);
                statusOutput->setText(QString(">> System: Deselected node [ %1 ]").arg(clickedNode->key));
            } else {
                if (userSelectedNodes.size() < 3) {
                    userSelectedNodes.append(clickedNode);
                    QString labelName = (userSelectedNodes.size() == 1) ? "A (Pivot)" : (userSelectedNodes.size() == 2) ? "B (Child)" : "C (Grandchild)";
                    statusOutput->setText(QString(">> Assertion: Selected %1 [ %2 ]").arg(labelName).arg(clickedNode->key));
                } else {
                    userSelectedNodes.clear();
                    userSelectedNodes.append(clickedNode);
                    statusOutput->setText(QString(">> System: Selection Reset. Selected Pivot A: [ %1 ]").arg(clickedNode->key));
                }
            }

            if (userSelectedNodes.size() == 3) {
                rotateButton->setEnabled(true);
                rotateButton->setStyleSheet(
                    "QPushButton { background-color: #A371F7; color: #FFFFFF; border-radius: 6px; padding: 0px 16px; font-weight: bold; border: none; height: 36px; }"
                    "QPushButton:hover { background-color: #B48EFA; }"
                    "QPushButton:pressed { background-color: #8957E5; }"
                    );
            } else {
                rotateButton->setEnabled(false);
                rotateButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 0px 16px; font-weight: bold; border: 1px solid #30363D; height: 36px; }");
            }
            updateVisuals();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

Quiz_AVL_Ins_Node* TREE_AVL_INSERT_QUIZ::pureBSTInsert(Quiz_AVL_Ins_Node* node, int key) {
    if (!node || node->isVirtual) return new Quiz_AVL_Ins_Node(key);
    if (key < node->key) node->left = pureBSTInsert(node->left, key);
    else if (key > node->key) node->right = pureBSTInsert(node->right, key);
    updateHeight(node);
    return node;
}

// ==========================================
// 4. 알고리즘 시뮬레이션 및 데이터 코어
// ==========================================
void TREE_AVL_INSERT_QUIZ::generateRandomBaseTree() {
    clearTree(avlRoot);
    avlRoot = nullptr;
    isActualInserted = false;

    int totalNodesCount = QRandomGenerator::global()->bounded(8, 11);

    QSet<int> uniqueNumbers;
    while (uniqueNumbers.size() < totalNodesCount) {
        int randomVal = QRandomGenerator::global()->bounded(1, 101);
        uniqueNumbers.insert(randomVal);
    }

    QList<int> randomKeys = uniqueNumbers.values();
    std::sort(randomKeys.begin(), randomKeys.end(), std::greater<int>());

    int initialCount = totalNodesCount;

    for (int i = 0; i < initialCount; ++i) {
        avlRoot = pureBSTInsert(avlRoot, randomKeys[i]);

        std::function<Quiz_AVL_Ins_Node*(Quiz_AVL_Ins_Node*)> balance = [&](Quiz_AVL_Ins_Node* n) -> Quiz_AVL_Ins_Node* {
            if (!n) return nullptr;
            n->left = balance(n->left);
            n->right = balance(n->right);

            updateHeight(n);
            int b = getBalance(n);

            if (b > 1 && getBalance(n->left) >= 0) return rotateRight(n);
            if (b > 1 && getBalance(n->left) < 0) { n->left = rotateLeft(n->left); return rotateRight(n); }
            if (b < -1 && getBalance(n->right) <= 0) return rotateLeft(n);
            if (b < -1 && getBalance(n->right) > 0) { n->right = rotateRight(n->right); return rotateLeft(n); }
            return n;
        };
        avlRoot = balance(avlRoot);
    }

    quizTargetInsertKey = QRandomGenerator::global()->bounded(1, 101);
    if (avlRoot) currentFocusNode = avlRoot;
}

void TREE_AVL_INSERT_QUIZ::calculateCorrectAnswers() {
    expectedSteps.clear();
    currentStepIndex = 0;

    std::function<Quiz_AVL_Ins_Node*(Quiz_AVL_Ins_Node*)> cloneTree = [&](Quiz_AVL_Ins_Node* n) -> Quiz_AVL_Ins_Node* {
        if (!n || n->isVirtual) return nullptr;
        Quiz_AVL_Ins_Node* cp = new Quiz_AVL_Ins_Node(n->key);
        cp->height = n->height; cp->left = cloneTree(n->left); cp->right = cloneTree(n->right);
        return cp;
    };
    Quiz_AVL_Ins_Node* tempRoot = cloneTree(avlRoot);
    tempRoot = pureBSTInsert(tempRoot, quizTargetInsertKey);

    std::function<Quiz_AVL_Ins_Node*(Quiz_AVL_Ins_Node*, bool&)> simulateBalance = [&](Quiz_AVL_Ins_Node* n, bool &changed) -> Quiz_AVL_Ins_Node* {
        if (!n || changed) return n;
        n->left = simulateBalance(n->left, changed);
        n->right = simulateBalance(n->right, changed);
        if (changed) return n;

        updateHeight(n);
        int bal = getBalance(n);
        if (std::abs(bal) > 1) {
            RotationStep step;
            step.aKey = n->key;
            if (bal > 1) {
                step.bKey = n->left->key;
                if (getBalance(n->left) >= 0) {
                    step.cKey = n->left->left ? n->left->left->key : -1;
                    expectedSteps.append(step); changed = true; return rotateRight(n);
                } else {
                    step.cKey = n->left->right ? n->left->right->key : -1;
                    expectedSteps.append(step); changed = true;
                    n->left = rotateLeft(n->left); return rotateRight(n);
                }
            } else {
                step.bKey = n->right->key;
                if (getBalance(n->right) <= 0) {
                    step.cKey = n->right->right ? n->right->right->key : -1;
                    expectedSteps.append(step); changed = true; return rotateLeft(n);
                } else {
                    step.cKey = n->right->left ? n->right->left->key : -1;
                    expectedSteps.append(step); changed = true;
                    n->right = rotateRight(n->right); return rotateLeft(n);
                }
            }
        }
        return n;
    };

    bool changed = true;
    while (changed) { changed = false; tempRoot = simulateBalance(tempRoot, changed); }
    clearTree(tempRoot);
}

void TREE_AVL_INSERT_QUIZ::startNewQuiz() {
    generateRandomBaseTree();
    calculateCorrectAnswers();

    attachVirtualNodes(avlRoot);

    userSelectedNodes.clear();
    rotateButton->setEnabled(false);
    rotateButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 0px 16px; font-weight: bold; border: 1px solid #30363D; height: 36px; }");

    currentQuizState = QuizState::STEP2_CLICK_EMPTY_NODE;

    quizQuestionLabel->setStyleSheet(
        "color: #F0883E; background-color: #1A1510; font-size: 13px; font-weight: bold; "
        "padding: 16px; border: 2px solid #D29922; border-radius: 10px;"
        );
    quizQuestionLabel->setText(QString("MISSION: Insert Key [ %1 ]\nFind and click the appropriate grey placeholder node.").arg(quizTargetInsertKey));

    statusOutput->setStyleSheet(
        "background-color: #0D1117; border: 1px solid #21262D; border-radius: 8px; "
        "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
        );
    statusOutput->setText(QString(">> All leaves appended with placeholders. Click correct spot to structuralize [ %1 ].").arg(quizTargetInsertKey));

    updateVisuals();
}

void TREE_AVL_INSERT_QUIZ::executeRotation() {
    if (currentQuizState != QuizState::STEP4_REBALANCE) return;
    if (expectedSteps.isEmpty()) {
        currentQuizState = QuizState::STEP5_QUIZ_COMPLETED;
        quizQuestionLabel->setStyleSheet(
            "color: #56B6C2; background-color: #0E161B; font-size: 13px; font-weight: bold; "
            "padding: 16px; border: 2px solid #56B6C2; border-radius: 10px;"
            );
        quizQuestionLabel->setText("CHALLENGE COMPLETE: AVL RESTORED CLEANLY");
        statusOutput->setStyleSheet(
            "background-color: #0F1914; border: 1px solid #238636; border-radius: 8px; "
            "color: #3FB950; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
            );
        statusOutput->setText(">> Success: Validation Pass! AVL Tree infrastructure balanced without modifications.");
        return;
    }

    if (userSelectedNodes.size() != 3) {
        statusOutput->setText(">> Prompt Error: Parameter stack incomplete. Exactly 3 nodes required.");
        return;
    }

    RotationStep currentExpected = expectedSteps[currentStepIndex];
    if (userSelectedNodes[0]->key != currentExpected.aKey ||
        userSelectedNodes[1]->key != currentExpected.bKey ||
        userSelectedNodes[2]->key != currentExpected.cKey) {
        statusOutput->setStyleSheet(
            "background-color: #1C1212; border: 1px solid #F85149; border-radius: 8px; "
            "color: #F85149; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
            );
        statusOutput->setText(QString(">> Assertion Failure [Stage %1]: Architectural pivot violation. Re-examine weights and try again.").arg(currentStepIndex + 1));

        userSelectedNodes.clear();
        rotateButton->setEnabled(false);
        rotateButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 0px 16px; font-weight: bold; border: 1px solid #30363D; height: 36px; }");
        updateVisuals();
        return;
    }

    std::function<Quiz_AVL_Ins_Node*(Quiz_AVL_Ins_Node*, int, bool&)> applySingleRotation =
        [&](Quiz_AVL_Ins_Node* n, int targetKey, bool &done) -> Quiz_AVL_Ins_Node* {
        if (!n || done) return n;
        n->left = applySingleRotation(n->left, targetKey, done);
        n->right = applySingleRotation(n->right, targetKey, done);
        if (done) return n;
        if (n->key == targetKey) {
            int b = getBalance(n);
            if (b > 1) {
                if (getBalance(n->left) < 0) n->left = rotateLeft(n->left);
                n = rotateRight(n);
            } else if (b < -1) {
                if (getBalance(n->right) > 0) n->right = rotateRight(n->right);
                n = rotateLeft(n);
            }
            done = true;
        }
        updateHeight(n);
        return n;
    };

    bool done = false;
    avlRoot = applySingleRotation(avlRoot, currentExpected.aKey, done);

    userSelectedNodes.clear();
    rotateButton->setEnabled(false);
    rotateButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 0px 16px; font-weight: bold; border: 1px solid #30363D; height: 36px; }");
    currentStepIndex++;

    if (currentStepIndex < expectedSteps.size()) {
        quizQuestionLabel->setText(QString("STAGE %1 UNLOCKED (Remaining: %2/%3)\nTrace next imbalance vector.").arg(currentStepIndex + 1).arg(currentStepIndex + 1).arg(expectedSteps.size()));
        statusOutput->setStyleSheet(
            "background-color: #101F30; border: 1px solid #1F6FEB; border-radius: 8px; "
            "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
            );
        statusOutput->setText(">> Sector Resolved: Subtree aligned. Propagating weight recalculation upward.");
        currentFocusNode = avlRoot;
        updateVisuals();
    } else {
        currentQuizState = QuizState::STEP5_QUIZ_COMPLETED;
        quizQuestionLabel->setStyleSheet(
            "color: #56B6C2; background-color: #0E161B; font-size: 13px; font-weight: bold; "
            "padding: 16px; border: 2px solid #56B6C2; border-radius: 10px;"
            );
        quizQuestionLabel->setText("CHALLENGE COMPLETE: AVL RESTORED CLEANLY");
        statusOutput->setStyleSheet(
            "background-color: #0F1914; border: 1px solid #238636; border-radius: 8px; "
            "color: #3FB950; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
            );
        statusOutput->setText(QString(">> Success: Validation Pass! Total %1 multi-stage balancing operations fully calculated.").arg(expectedSteps.size()));

        std::function<void(Quiz_AVL_Ins_Node*)> finalHeightSync = [&](Quiz_AVL_Ins_Node* n) {
            if (!n) return;
            finalHeightSync(n->left); finalHeightSync(n->right); updateHeight(n);
        };
        finalHeightSync(avlRoot);
        currentFocusNode = avlRoot;
        updateVisuals();
    }
}

Quiz_AVL_Ins_Node* TREE_AVL_INSERT_QUIZ::rotateRight(Quiz_AVL_Ins_Node* y) {
    Quiz_AVL_Ins_Node* x = y->left; Quiz_AVL_Ins_Node* T2 = x->right;
    x->right = y; y->left = T2; updateHeight(y); updateHeight(x); return x;
}

Quiz_AVL_Ins_Node* TREE_AVL_INSERT_QUIZ::rotateLeft(Quiz_AVL_Ins_Node* x) {
    Quiz_AVL_Ins_Node* y = x->right; Quiz_AVL_Ins_Node* T2 = y->left;
    y->left = x; x->right = T2; updateHeight(x); updateHeight(y); return y;
}

void TREE_AVL_INSERT_QUIZ::updateVisuals() {
    scene->clear();
    if (!avlRoot) return;

    double initial_x_offset = 160.0;
    drawNode(avlRoot, 0, 0.0, initial_x_offset);

    QRectF bounds = scene->itemsBoundingRect();
    scene->setSceneRect(bounds.adjusted(-80, -40, 80, 80));
}

// ==========================================
// 5. 시각화 노드 및 가상 간선 렌더링 파이프라인
// ==========================================
void TREE_AVL_INSERT_QUIZ::drawNode(Quiz_AVL_Ins_Node* node, int level, double x_pos, double x_offset) {
    if (!node) return;
    double y_pos = level * LEVEL_HEIGHT;

    QPen pen(QColor("#D29922"), 2);
    QBrush brush(QColor("#161B22"));
    QColor textColor(QColor("#E6EDF3"));

    // 가상 더미 플레이스홀더 노드 전용 대시 보더 스타일
    if (node->isVirtual) {
        pen = QPen(QColor("#484F58"), 1, Qt::DashLine);
        brush = QBrush(QColor("#21262D"));
        textColor = QColor("#8B949E");
    }

    if (!node->isVirtual && node == currentFocusNode && currentQuizState != QuizState::STEP4_REBALANCE) {
        brush = QBrush(QColor("#1F6FEB"));
        pen = QPen(QColor("#58A6FF"), 2);
        textColor = QColor("#FFFFFF");
    }

    // 선택 순서 인덱스 기반 개별 불균형 복구 노드 하이라이팅
    if (!node->isVirtual && userSelectedNodes.contains(node)) {
        int index = userSelectedNodes.indexOf(node);
        if (index == 0) {
            brush = QBrush(QColor("#FF7B72"));
            pen = QPen(QColor("#F85149"), 2);
            textColor = QColor("#0D1117");
        }
        else if (index == 1) {
            brush = QBrush(QColor("#3FB950"));
            pen = QPen(QColor("#238636"), 2);
            textColor = QColor("#FFFFFF");
        }
        else if (index == 2) {
            brush = QBrush(QColor("#B48EFA"));
            pen = QPen(QColor("#A371F7"), 2);
            textColor = QColor("#FFFFFF");
        }
    }

    if (!node->isVirtual && node->key == quizTargetInsertKey && isActualInserted) {
        pen = QPen(QColor("#58A6FF"), 2, Qt::DashLine);
        brush = QBrush(QColor("#0D1F30"));
    }

    auto rectItem = new QGraphicsRectItem(x_pos - NODE_WIDTH/2, y_pos, NODE_WIDTH, NODE_HEIGHT);
    rectItem->setPen(pen);
    rectItem->setBrush(brush);
    rectItem->setData(0, QVariant::fromValue(static_cast<void*>(node)));
    scene->addItem(rectItem);

    QString centerText = node->isVirtual ? "?" : QString::number(node->key);
    auto textItem = new QGraphicsTextItem(centerText);
    textItem->setDefaultTextColor(textColor);

    QFont font = textItem->font();
    font.setBold(true);
    font.setPointSize(10);
    textItem->setFont(font);

    textItem->setPos(x_pos - textItem->boundingRect().width()/2, y_pos + (NODE_HEIGHT - textItem->boundingRect().height())/2);
    textItem->setData(0, QVariant::fromValue(static_cast<void*>(node)));
    scene->addItem(textItem);

    double next_x_offset = x_offset * 0.44;
    QPointF parentBottom(x_pos, y_pos + NODE_HEIGHT);

    if (node->left) {
        QPen linePen = node->left->isVirtual ? QPen(QColor("#21262D"), 1, Qt::DotLine) : QPen(QColor("#30363D"), 2);
        scene->addLine(parentBottom.x(), parentBottom.y(), x_pos - x_offset, (level + 1) * LEVEL_HEIGHT, linePen);
        drawNode(node->left, level + 1, x_pos - x_offset, next_x_offset);
    }
    if (node->right) {
        QPen linePen = node->right->isVirtual ? QPen(QColor("#21262D"), 1, Qt::DotLine) : QPen(QColor("#30363D"), 2);
        scene->addLine(parentBottom.x(), parentBottom.y(), x_pos + x_offset, (level + 1) * LEVEL_HEIGHT, linePen);
        drawNode(node->right, level + 1, x_pos + x_offset, next_x_offset);
    }
}