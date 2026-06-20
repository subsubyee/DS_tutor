#include "tree_avl_delete_quiz.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QPainterPath>
#include <cmath>
#include <algorithm>
#include <qgroupbox.h>

// ==========================================
// 1. UI 렌더링 기하학 상수정의 (시뮬레이터 사양과 100% 일치)
// ==========================================
static constexpr double NODE_WIDTH = 55.0;
static constexpr double NODE_HEIGHT = 35.0;
static constexpr double LEVEL_HEIGHT = 65.0;

TREE_AVL_DELETE_QUIZ::TREE_AVL_DELETE_QUIZ(QMainWindow* _parent) : QObject(_parent) {
    parent = _parent;
    avlRoot = nullptr;
    currentFocusNode = nullptr;
    quizTargetDeleteKey = -1;
    currentQuizState = DeleteQuizState::STEP1_PRE_START;
    currentStepIndex = 0;
}

TREE_AVL_DELETE_QUIZ::~TREE_AVL_DELETE_QUIZ() { clearTree(avlRoot); }

void TREE_AVL_DELETE_QUIZ::clearTree(Quiz_AVL_Del_Node* node) {
    if (node) { clearTree(node->left); clearTree(node->right); delete node; }
}

void TREE_AVL_DELETE_QUIZ::updateHeight(Quiz_AVL_Del_Node* n) {
    if (n) {
        int lh = n->left ? n->left->height : 0;
        int rh = n->right ? n->right->height : 0;
        n->height = 1 + std::max(lh, rh);
    }
}

int TREE_AVL_DELETE_QUIZ::getBalance(Quiz_AVL_Del_Node* n) {
    if (!n) return 0;
    return (n->left ? n->left->height : 0) - (n->right ? n->right->height : 0);
}

Quiz_AVL_Del_Node* TREE_AVL_DELETE_QUIZ::findSuccessor(Quiz_AVL_Del_Node* node) {
    Quiz_AVL_Del_Node* current = node->right;
    while (current && current->left) current = current->left;
    return current;
}

Quiz_AVL_Del_Node* TREE_AVL_DELETE_QUIZ::pureBSTInsert(Quiz_AVL_Del_Node* node, int key) {
    if (!node) return new Quiz_AVL_Del_Node(key);
    if (key < node->key) node->left = pureBSTInsert(node->left, key);
    else if (key > node->key) node->right = pureBSTInsert(node->right, key);
    updateHeight(node);
    return node;
}

// ==========================================
// 2. UI 레이아웃 세팅 (시뮬레이터 디자인 기조 완벽 통일)
// ==========================================
QWidget* TREE_AVL_DELETE_QUIZ::setting() {
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet(
        "QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }"
        );

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION CANVAS (고정 크기 스크롤 대응)
    // ==========================================
    QGroupBox *leftGroup = new QGroupBox(containerWidget);
    leftGroup->setStyleSheet("QGroupBox { border: none; background: transparent; }");

    QVBoxLayout *leftLayout = new QVBoxLayout(leftGroup);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● AVL DELETION CANVAS\n\nInteractive Balanced Tree Rebalancing Quiz Sandbox", leftGroup);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 14px;");
    leftLayout->addWidget(visTitle);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, leftGroup);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QBrush(QColor("#11141A")));
    view->setStyleSheet("QGraphicsView { border: 1px solid #21262D; border-radius: 12px; background-color: #11141A; }");

    // fitInView 억제 및 넘칠 때 스크롤바 활성화 처리 변경
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    view->viewport()->installEventFilter(this);
    leftLayout->addWidget(view);

    // ==========================================
    // [Right Area] CONTROL PANEL (너비 420px 통일)
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
    rightLayout->setSpacing(12);

    // 미션 카드
    quizQuestionLabel = new QLabel("Press 'Initialize Deletion Quiz' to play.", rightContentWidget);
    quizQuestionLabel->setWordWrap(true);
    quizQuestionLabel->setAlignment(Qt::AlignCenter);
    quizQuestionLabel->setStyleSheet(
        "color: #8B949E; background-color: #161B22; font-size: 13px; font-weight: bold; "
        "padding: 16px; border: 2px dashed #30363D; border-radius: 10px;"
        );
    rightLayout->addWidget(quizQuestionLabel);

    QLabel *controlsTitle = new QLabel("● SYSTEM CONTROL DASHBOARD", rightContentWidget);
    controlsTitle->setStyleSheet("font-size: 12px; font-weight: bold; color: #8B949E; letter-spacing: 1px; margin-top: 4px;");
    rightLayout->addWidget(controlsTitle);

    QGridLayout *controlGrid = new QGridLayout();
    controlGrid->setSpacing(12);

    // 공용 스타일셋
    QString cardStyle   = "QWidget { background-color: #161B22; border: 1px solid #21262D; border-radius: 10px; }";
    QString descStyle   = "color: #8B949E; font-size: 11px; background: transparent; border: none;";
    QString stepStyle   = "color: #56B6C2; font-size: 11px; font-weight: bold; font-family: monospace; background: transparent; border: none;";
    QString genBtnStyle = "QPushButton { background-color: #238636; color: #FFFFFF; border-radius: 6px; padding: 8px 16px; font-weight: bold; border: none; }"
                          "QPushButton:hover { background-color: #2EA043; }"
                          "QPushButton:pressed { background-color: #248039; }";
    QString actionBtn   = "QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 8px 14px; font-weight: bold; border: 1px solid #30363D; }";

    // --- Card 1: Quiz Init ---
    QWidget *initCard = new QWidget(rightContentWidget); initCard->setAttribute(Qt::WA_StyledBackground, true); initCard->setStyleSheet(cardStyle);
    QHBoxLayout *initL = new QHBoxLayout(initCard); initL->setContentsMargins(14, 12, 14, 12);
    QLabel *initT = new QLabel("Step 1", initCard); initT->setStyleSheet(stepStyle);
    QLabel *initD = new QLabel("Deploy Infrastructure", initCard); initD->setStyleSheet(descStyle);
    QPushButton* startBtn = new QPushButton("Initialize Quiz", initCard); startBtn->setStyleSheet(genBtnStyle);
    initL->addWidget(initT); initL->addWidget(initD); initL->addStretch(); initL->addWidget(startBtn);
    controlGrid->addWidget(initCard, 0, 0);

    // --- Card 2: Structural Deletion ---
    QWidget *deleteCard = new QWidget(rightContentWidget); deleteCard->setAttribute(Qt::WA_StyledBackground, true); deleteCard->setStyleSheet(cardStyle);
    QHBoxLayout *delL = new QHBoxLayout(deleteCard); delL->setContentsMargins(14, 12, 14, 12);
    QLabel *delT = new QLabel("Step 2", deleteCard); delT->setStyleSheet(stepStyle);
    QLabel *delD = new QLabel("Trigger Physical Deletion", deleteCard); delD->setStyleSheet(descStyle);
    insertActionButton = new QPushButton("Execute Delete", deleteCard); insertActionButton->setStyleSheet(actionBtn); insertActionButton->setEnabled(false);
    delL->addWidget(delT); delL->addWidget(delD); delL->addStretch(); delL->addWidget(insertActionButton);
    controlGrid->addWidget(deleteCard, 1, 0);

    // --- Card 3: Rebalance Verification ---
    QWidget *rotateCard = new QWidget(rightContentWidget); rotateCard->setAttribute(Qt::WA_StyledBackground, true); rotateCard->setStyleSheet(cardStyle);
    QHBoxLayout *rotL = new QHBoxLayout(rotateCard); rotL->setContentsMargins(14, 12, 14, 12);
    QLabel *rotT = new QLabel("Step 3", rotateCard); rotT->setStyleSheet(stepStyle);
    QLabel *rotD = new QLabel("Verify Rotations", rotateCard); rotD->setStyleSheet(descStyle);
    rotateButton = new QPushButton("Execute Rotation", rotateCard); rotateButton->setEnabled(false); rotateButton->setStyleSheet(actionBtn);
    rotL->addWidget(rotT); rotL->addWidget(rotD); rotL->addStretch(); rotL->addWidget(rotateButton);
    controlGrid->addWidget(rotateCard, 2, 0);

    rightLayout->addLayout(controlGrid);

    // --- Live Log 시스템 ---
    QLabel *logTitle = new QLabel(">_ Interactive Quiz Live Log", rightContentWidget);
    logTitle->setStyleSheet("color: #8B949E; font-size: 11px; font-family: monospace; font-weight: bold; margin-top: 6px;");
    rightLayout->addWidget(logTitle);

    statusOutput = new QLabel("Quiz Engine initialized. Ready to deploy AVL framework.", rightContentWidget);
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

    // 커넥션 관리 (기존 컴포넌트 축소에 대응)
    connect(startBtn, &QPushButton::clicked, this, &TREE_AVL_DELETE_QUIZ::startNewQuiz);
    connect(insertActionButton, &QPushButton::clicked, this, &TREE_AVL_DELETE_QUIZ::executeNodeDeletion);
    connect(rotateButton, &QPushButton::clicked, this, &TREE_AVL_DELETE_QUIZ::executeRotation);

    this->updateVisuals();
    return containerWidget;
}

// ==========================================
// 3. 랜덤 트리 생성 및 가상 검증 파이프라인
// ==========================================
void TREE_AVL_DELETE_QUIZ::generateRandomBaseTreeWithRotationCheck() {
    while (true) {
        clearTree(avlRoot);
        avlRoot = nullptr;

        QList<int> fullPool;
        for (int i = 10; i <= 99; ++i) fullPool.append(i);

        QList<int> quizPool;
        for (int i = 0; i < 12; ++i) {
            int randomIdx = QRandomGenerator::global()->bounded(fullPool.size());
            quizPool.append(fullPool.takeAt(randomIdx));
        }

        auto balanceNode = [&](auto self, Quiz_AVL_Del_Node* n) -> Quiz_AVL_Del_Node* {
            if (!n) return nullptr;
            n->left = self(self, n->left);
            n->right = self(self, n->right);
            updateHeight(n);
            int b = getBalance(n);
            if (b > 1 && getBalance(n->left) >= 0) return rotateRight(n);
            if (b > 1 && getBalance(n->left) < 0) { n->left = rotateLeft(n->left); return rotateRight(n); }
            if (b < -1 && getBalance(n->right) <= 0) return rotateLeft(n);
            if (b < -1 && getBalance(n->right) > 0) { n->right = rotateRight(n->right); return rotateLeft(n); }
            return n;
        };

        for (int i = 0; i < 10; ++i) {
            avlRoot = pureBSTInsert(avlRoot, quizPool[i]);
            avlRoot = balanceNode(balanceNode, avlRoot);
        }

        int targetIdx = QRandomGenerator::global()->bounded(10);
        quizTargetDeleteKey = quizPool[targetIdx];

        calculateCorrectAnswers();

        // 삭제 후 반드시 1회 이상의 Rebalancing 회전이 필요한 트리 스키마가 도출될 때까지 보정 루프 수행
        if (!expectedSteps.isEmpty()) {
            break;
        }
    }
}

void TREE_AVL_DELETE_QUIZ::calculateCorrectAnswers() {
    expectedSteps.clear();
    currentStepIndex = 0;

    auto clone = [&](auto self, Quiz_AVL_Del_Node* n) -> Quiz_AVL_Del_Node* {
        if (!n) return nullptr;
        auto cp = new Quiz_AVL_Del_Node(n->key);
        cp->height = n->height; cp->left = self(self, n->left); cp->right = self(self, n->right);
        return cp;
    };

    Quiz_AVL_Del_Node* tempRoot = clone(clone, avlRoot);

    auto simulateDel = [&](auto self, Quiz_AVL_Del_Node* n, int k) -> Quiz_AVL_Del_Node* {
        if (!n) return nullptr;
        if (k < n->key) n->left = self(self, n->left, k);
        else if (k > n->key) n->right = self(self, n->right, k);
        else {
            if (!n->left || !n->right) {
                auto t = n->left ? n->left : n->right;
                delete n; return t;
            }
            auto s = findSuccessor(n); n->key = s->key;
            n->right = self(self, n->right, s->key);
        }
        return n;
    };

    tempRoot = simulateDel(simulateDel, tempRoot, quizTargetDeleteKey);

    auto simulateBalance = [&](auto self, Quiz_AVL_Del_Node* n, bool &changed) -> Quiz_AVL_Del_Node* {
        if (!n || changed) return n;
        n->left = self(self, n->left, changed);
        n->right = self(self, n->right, changed);
        if (changed) return n;

        updateHeight(n);
        int b = getBalance(n);
        if (std::abs(b) > 1) {
            DeleteRotationStep s; s.aKey = n->key;
            if (b > 1) {
                s.bKey = n->left->key;
                s.cKey = (getBalance(n->left) >= 0) ? (n->left->left ? n->left->left->key : -1) : (n->left->right->key);
                expectedSteps.append(s); changed = true;
                if (getBalance(n->left) < 0) n->left = rotateLeft(n->left);
                return rotateRight(n);
            } else {
                s.bKey = n->right->key;
                s.cKey = (getBalance(n->right) <= 0) ? (n->right->right ? n->right->right->key : -1) : (n->right->left->key);
                expectedSteps.append(s); changed = true;
                if (getBalance(n->right) > 0) n->right = rotateRight(n->right);
                return rotateLeft(n);
            }
        }
        return n;
    };

    bool changed = true;
    while(changed) { changed = false; tempRoot = simulateBalance(simulateBalance, tempRoot, changed); }
    clearTree(tempRoot);
}

// ==========================================
// 4. 퀴즈 핵심 상태 관리 및 액션 핸들러
// ==========================================
void TREE_AVL_DELETE_QUIZ::startNewQuiz() {
    generateRandomBaseTreeWithRotationCheck();

    currentFocusNode = nullptr; // 기존 고정 포커스 초기화
    userSelectedNodes.clear();
    currentStepIndex = 0;
    currentQuizState = DeleteQuizState::STEP2_NAVIGATING;

    insertActionButton->setEnabled(false);
    insertActionButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 8px 14px; font-weight: bold; border: 1px solid #30363D; }");

    quizQuestionLabel->setStyleSheet(
        "color: #F0883E; background-color: #1A1510; font-size: 13px; font-weight: bold; "
        "padding: 16px; border: 2px solid #D29922; border-radius: 10px;"
        );
    quizQuestionLabel->setText(QString("MISSION: Target Node [ %1 ]\nClick the target node directly on the viewport canvas.").arg(quizTargetDeleteKey));

    rotateButton->setEnabled(false);
    rotateButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 8px 16px; font-weight: bold; border: 1px solid #30363D; }");

    statusOutput->setStyleSheet(
        "background-color: #0D1117; border: 1px solid #21262D; border-radius: 8px; "
        "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
        );
    statusOutput->setText(QString(">> Direct-Click Mode Engaged. Find and click Node [ %1 ] to select.").arg(quizTargetDeleteKey));

    updateVisuals();
}

void TREE_AVL_DELETE_QUIZ::executeNodeDeletion() {
    if (currentQuizState != DeleteQuizState::STEP3_DELETE_READY || !currentFocusNode || currentFocusNode->key != quizTargetDeleteKey) {
        statusOutput->setStyleSheet(
            "background-color: #1C1212; border: 1px solid #F85149; border-radius: 8px; "
            "color: #F85149; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
            );
        statusOutput->setText(">> Error: Operation aborted. Please select the correct target node first.");
        return;
    }

    auto realDelete = [&](auto self, Quiz_AVL_Del_Node* n, int k) -> Quiz_AVL_Del_Node* {
        if (k < n->key) n->left = self(self, n->left, k);
        else if (k > n->key) n->right = self(self, n->right, k);
        else {
            if (!n->left || !n->right) {
                auto t = n->left ? n->left : n->right;
                delete n; return t;
            }
            auto s = findSuccessor(n); n->key = s->key;
            n->right = self(self, n->right, s->key);
        }
        updateHeight(n); return n;
    };

    avlRoot = realDelete(realDelete, avlRoot, quizTargetDeleteKey);
    currentFocusNode = nullptr;
    currentQuizState = DeleteQuizState::STEP4_REBALANCE;

    insertActionButton->setEnabled(false);
    insertActionButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 8px 14px; font-weight: bold; border: 1px solid #30363D; }");

    quizQuestionLabel->setStyleSheet(
        "color: #F0883E; background-color: #1A1510; font-size: 13px; font-weight: bold; "
        "padding: 16px; border: 2px solid #D29922; border-radius: 10px;"
        );
    quizQuestionLabel->setText(QString("IMBALANCE DETECTED (Required Rebalances: %1)\nAssert Pivot A, Child B, and Grandchild C sequentially.").arg(expectedSteps.size()));

    statusOutput->setStyleSheet(
        "background-color: #0D1117; border: 1px solid #21262D; border-radius: 8px; "
        "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
        );
    statusOutput->setText(">> Structure updated. Map the structural imbalance path (A -> B -> C) by clicking nodes in order.");

    updateVisuals();
}

void TREE_AVL_DELETE_QUIZ::executeRotation() {
    if (currentQuizState != DeleteQuizState::STEP4_REBALANCE) return;

    if (userSelectedNodes.size() != 3) {
        statusOutput->setText(">> Prompt Error: Incomplete nodes path. Exactly 3 nodes required.");
        return;
    }

    DeleteRotationStep target = expectedSteps[currentStepIndex];

    // [수정사항] 오답 유도 시 즉시 리셋하지 않고 다시 클릭하여 재도전하도록 세션 유지 보정
    if (userSelectedNodes[0]->key != target.aKey ||
        userSelectedNodes[1]->key != target.bKey ||
        userSelectedNodes[2]->key != target.cKey) {
        statusOutput->setStyleSheet(
            "background-color: #1C1212; border: 1px solid #F85149; border-radius: 8px; "
            "color: #F85149; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
            );
        statusOutput->setText(QString(">> Assertion Failure [Stage %1]: Incorrect imbalance path! Re-examine weights and try again.").arg(currentStepIndex + 1));

        // 유저 선택 초기화 후 리턴 (트리 초기화 방지)
        userSelectedNodes.clear();
        rotateButton->setEnabled(false);
        rotateButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 8px 16px; font-weight: bold; border: 1px solid #30363D; }");
        updateVisuals();
        return;
    }

    auto apply = [&](auto self, Quiz_AVL_Del_Node* n, int ak, bool &done) -> Quiz_AVL_Del_Node* {
        if (!n || done) return n;
        n->left = self(self, n->left, ak, done);
        n->right = self(self, n->right, ak, done);
        if (done) return n;
        if (n->key == ak) {
            int b = getBalance(n);
            if (b > 1) { if(getBalance(n->left) < 0) n->left = rotateLeft(n->left); n = rotateRight(n); }
            else { if(getBalance(n->right) > 0) n->right = rotateRight(n->right); n = rotateLeft(n); }
            done = true;
        }
        updateHeight(n); return n;
    };

    bool done = false;
    avlRoot = apply(apply, avlRoot, target.aKey, done);
    userSelectedNodes.clear();

    rotateButton->setEnabled(false);
    rotateButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 8px 16px; font-weight: bold; border: 1px solid #30363D; }");

    currentStepIndex++;

    if (currentStepIndex >= expectedSteps.size()) {
        currentQuizState = DeleteQuizState::STEP5_QUIZ_COMPLETED;
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
    } else {
        quizQuestionLabel->setText(QString("STAGE %1 UNLOCKED (Remaining: %2/%3)\nTrace next imbalance vector.").arg(currentStepIndex + 1).arg(currentStepIndex + 1).arg(expectedSteps.size()));
        statusOutput->setStyleSheet(
            "background-color: #101F30; border: 1px solid #1F6FEB; border-radius: 8px; "
            "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
            );
        statusOutput->setText(">> Sector Resolved: Subtree aligned. Propagating weight recalculation upward.");
    }
    updateVisuals();
}

// ==========================================
// 5. 캔버스 마우스 픽킹 커스텀 이벤트 필터
// ==========================================
bool TREE_AVL_DELETE_QUIZ::eventFilter(QObject* obj, QEvent* event) {
    // fitInView 자동 조절 필터 강제 비활성화 (시뮬레이터 사양 동치화)
    if (obj == view->viewport() && event->type() == QEvent::Resize) {
        return true;
    }

    if (event->type() == QEvent::MouseButtonPress) {
        auto m = static_cast<QMouseEvent*>(event);
        auto item = view->itemAt(m->pos());
        if (item) {
            QVariant data = item->data(0);
            if (data.isValid()) {
                auto n = reinterpret_cast<Quiz_AVL_Del_Node*>(data.value<void*>());
                if (!n) return QObject::eventFilter(obj, event);

                // 상태 1: 노드 삭제 대상 직접 클릭 모드
                if (currentQuizState == DeleteQuizState::STEP2_NAVIGATING || currentQuizState == DeleteQuizState::STEP3_DELETE_READY) {
                    currentFocusNode = n;
                    if (currentFocusNode->key == quizTargetDeleteKey) {
                        currentQuizState = DeleteQuizState::STEP3_DELETE_READY;
                        insertActionButton->setEnabled(true);
                        insertActionButton->setStyleSheet("QPushButton { background-color: #F85149; color: #FFFFFF; border-radius: 6px; padding: 8px 14px; font-weight: bold; border: none; }"
                                                          "QPushButton:hover { background-color: #FF7B72; }");
                        statusOutput->setText(">> System: Target pointer matched! Click 'Execute Delete' to commit.");
                    } else {
                        currentQuizState = DeleteQuizState::STEP2_NAVIGATING;
                        insertActionButton->setEnabled(false);
                        insertActionButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 8px 14px; font-weight: bold; border: 1px solid #30363D; }");
                        statusOutput->setText(QString(">> Warning: Clicked Node [ %1 ] is not the deletion target. Keep searching.").arg(currentFocusNode->key));
                    }
                    updateVisuals();
                    return true;
                }

                // 상태 2: 불균형 복구 회전 피벗 선택 모드 (A->B->C)
                if (currentQuizState == DeleteQuizState::STEP4_REBALANCE) {
                    if (userSelectedNodes.contains(n)) {
                        userSelectedNodes.removeOne(n);
                    } else {
                        if (userSelectedNodes.size() < 3) {
                            userSelectedNodes.append(n);
                        } else {
                            userSelectedNodes.clear();
                            userSelectedNodes.append(n);
                        }
                    }

                    if (userSelectedNodes.size() == 3) {
                        rotateButton->setEnabled(true);
                        rotateButton->setStyleSheet("QPushButton { background-color: #A371F7; color: #FFFFFF; border-radius: 6px; padding: 8px 16px; font-weight: bold; border: none; }"
                                                    "QPushButton:hover { background-color: #B48EFA; }"
                                                    "QPushButton:pressed { background-color: #8957E5; }");
                    } else {
                        rotateButton->setEnabled(false);
                        rotateButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 8px 16px; font-weight: bold; border: 1px solid #30363D; }");
                    }
                    updateVisuals();
                    return true;
                }
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

Quiz_AVL_Del_Node* TREE_AVL_DELETE_QUIZ::rotateRight(Quiz_AVL_Del_Node* y) {
    auto x = y->left; auto T2 = x->right; x->right = y; y->left = T2;
    updateHeight(y); updateHeight(x); return x;
}

Quiz_AVL_Del_Node* TREE_AVL_DELETE_QUIZ::rotateLeft(Quiz_AVL_Del_Node* x) {
    auto y = x->right; auto T2 = y->left; y->left = x; x->right = T2;
    updateHeight(x); updateHeight(y); return y;
}

// ==========================================
// 6. 시각화 업데이트 및 노드 드로잉 (메타 데이터 삭제형)
// ==========================================
void TREE_AVL_DELETE_QUIZ::updateVisuals() {
    scene->clear();
    if (!avlRoot) return;

    double initial_x_offset = 160.0; // 시뮬레이터 배율과 동치화
    drawNode(avlRoot, 0, 0.0, initial_x_offset);

    QRectF bounds = scene->itemsBoundingRect();
    scene->setSceneRect(bounds.adjusted(-80, -40, 80, 80));
}

void TREE_AVL_DELETE_QUIZ::drawNode(Quiz_AVL_Del_Node* node, int level, double x_pos, double x_offset) {
    if (!node) return;
    double y_pos = level * LEVEL_HEIGHT;

    // [BST 퀴즈 디자인 통일] 기본 스타일: 슬레이트 블랙 배경 + 다크 골드 테두리
    QPen pen(QColor("#D29922"), 2);
    QBrush brush(QColor("#161B22"));
    QColor textColor(QColor("#E6EDF3"));

    // [BST 퀴즈 디자인 통일] 기본 가이드 포커스 하이라이트 (선택 노드가 없을 때 블루 계열)
    if (node == currentFocusNode && userSelectedNodes.isEmpty()) {
        brush = QBrush(QColor("#1F6FEB"));
        pen = QPen(QColor("#58A6FF"), 2);
        textColor = QColor("#FFFFFF");
    }

    // [BST 퀴즈 디자인 통일] 선택 순서 인덱스 기반 개별 노드 전용 컬러 매핑
    if (userSelectedNodes.contains(node)) {
        int index = userSelectedNodes.indexOf(node);
        if (index == 0) {
            // 1순위 클릭 타겟 (Pivot A / Target) : Red
            brush = QBrush(QColor("#FF7B72"));
            pen = QPen(QColor("#F85149"), 2);
            textColor = QColor("#0D1117");
        }
        else if (index == 1) {
            // 2순위 클릭 타겟 (Child B / Replacement) : Green
            brush = QBrush(QColor("#3FB950"));
            pen = QPen(QColor("#238636"), 2);
            textColor = QColor("#FFFFFF");
        }
        else if (index == 2) {
            // 3순위 클릭 타겟 (Grandchild C) : Purple
            // (AVL의 특성상 3번째 노드가 선택될 수 있으므로 고유의 퍼플 하이라이트 유지)
            brush = QBrush(QColor("#B48EFA"));
            pen = QPen(QColor("#A371F7"), 2);
            textColor = QColor("#FFFFFF");
        }
    }

    // [BST 퀴즈 디자인 통일] 둥근 사각형(Path) 대신 각진 직사각형(QGraphicsRectItem) 사양으로 복구
    auto rectItem = new QGraphicsRectItem(x_pos - NODE_WIDTH/2, y_pos, NODE_WIDTH, NODE_HEIGHT);
    rectItem->setPen(pen);
    rectItem->setBrush(brush);
    rectItem->setData(0, QVariant::fromValue(static_cast<void*>(node)));
    scene->addItem(rectItem);

    // [BST 퀴즈 디자인 통일] 내부 정수 키값 출력 및 중앙 정렬
    auto textItem = new QGraphicsTextItem(QString::number(node->key));
    textItem->setDefaultTextColor(textColor);

    QFont font = textItem->font();
    font.setBold(true);
    font.setPointSize(10);
    textItem->setFont(font);

    textItem->setPos(x_pos - textItem->boundingRect().width()/2, y_pos + (NODE_HEIGHT - textItem->boundingRect().height())/2);
    textItem->setData(0, QVariant::fromValue(static_cast<void*>(node)));
    scene->addItem(textItem);

    // 가로 폭 배율 감소 오프셋 및 엣지 라인 연결
    double next_x_offset = x_offset * 0.44;
    QPointF parentBottom(x_pos, y_pos + NODE_HEIGHT);
    QPen linePen(QColor("#30363D"), 2);

    if (node->left) {
        scene->addLine(parentBottom.x(), parentBottom.y(), x_pos - x_offset, (level + 1) * LEVEL_HEIGHT, linePen);
        drawNode(node->left, level + 1, x_pos - x_offset, next_x_offset);
    }
    if (node->right) {
        scene->addLine(parentBottom.x(), parentBottom.y(), x_pos + x_offset, (level + 1) * LEVEL_HEIGHT, linePen);
        drawNode(node->right, level + 1, x_pos + x_offset, next_x_offset);
    }
}