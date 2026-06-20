#include "tree_bst_quiz.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGridLayout>
#include <QScrollArea>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QVariant>
#include <QList>
#include <algorithm>

// ==========================================
// 1. UI 렌더링 기하학 및 글로벌 크기 상수 정의
// ==========================================
static constexpr double NODE_WIDTH = 55.0;
static constexpr double NODE_HEIGHT = 35.0;
static constexpr double LEVEL_HEIGHT = 65.0;
static constexpr int BUTTON_FIXED_HEIGHT = 36; // 모든 제어 버튼의 세로 크기를 일괄 고정

TREE_BST_QUIZ::TREE_BST_QUIZ(QMainWindow* _parent) : QObject(_parent) {
    parent = _parent;
    root = nullptr;
    size = 0;
    currentFocusNode = nullptr;
    quizTargetKey = -1;
    currentQuizState = QuizState::PRE_START;

    // 타겟 및 후계자 노드 순서 제어용 컨테이너 초기화
    userSelectedNodes.clear();
}

TREE_BST_QUIZ::~TREE_BST_QUIZ() {
    clearTree(root);
}

void TREE_BST_QUIZ::clearTree(Quiz_Node* node) {
    if (node) {
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
}

// ==========================================
// 2. UI 레이아웃 세팅 (좌측 패널 및 버튼 치수 완벽 통일)
// ==========================================
QWidget* TREE_BST_QUIZ::setting() {
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet(
        "QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }"
        );

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION CANVAS (디자인 완전 통일)
    // ==========================================
    QGroupBox *leftGroup = new QGroupBox(containerWidget);
    leftGroup->setStyleSheet("QGroupBox { border: none; background: transparent; }");

    QVBoxLayout *leftLayout = new QVBoxLayout(leftGroup);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(12);

    // AVL 버전과 개행(\n), 글자 크기, 라인 높이, 자간 마진을 완벽하게 맞춤 복제
    QLabel *visTitle = new QLabel("● BST DELETION CANVAS\n\nInteractive Node Deletion & Replacement Sandbox", leftGroup);
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
    quizQuestionLabel = new QLabel("Press 'Generate Random Tree & Quiz' to start.", rightContentWidget);
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

    // 스타일시트 및 버튼 세로폭 일괄 통일 정책 적용 (padding 수치 조정)
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

    // --- Card 1: 퀴즈 시작 버튼 카드 (세로폭 강제 고정) ---
    QWidget *initCard = new QWidget(rightContentWidget); initCard->setAttribute(Qt::WA_StyledBackground, true); initCard->setStyleSheet(cardStyle);
    QHBoxLayout *initL = new QHBoxLayout(initCard); initL->setContentsMargins(14, 12, 14, 12);
    QLabel *initT = new QLabel("Setup", initCard); initT->setStyleSheet(stepStyle);
    QLabel *initD = new QLabel("Deploy Quiz Tree", initCard); initD->setStyleSheet(descStyle);
    startQuizButton = new QPushButton("Generate Random Tree", initCard);
    startQuizButton->setStyleSheet(genBtnStyle);
    startQuizButton->setFixedHeight(BUTTON_FIXED_HEIGHT); // 하드웨어 렌더링 세로길이 강제 고정
    initL->addWidget(initT); initL->addWidget(initD); initL->addStretch(); initL->addWidget(startQuizButton);
    controlGrid->addWidget(initCard, 0, 0);

    // --- Card 2: 검증 및 삭제 실행 버튼 카드 (세로폭 강제 고정) ---
    QWidget *verifyCard = new QWidget(rightContentWidget); verifyCard->setAttribute(Qt::WA_StyledBackground, true); verifyCard->setStyleSheet(cardStyle);
    QHBoxLayout *vfyL = new QHBoxLayout(verifyCard); vfyL->setContentsMargins(14, 12, 14, 12);
    QLabel *vfyT = new QLabel("Verify", verifyCard); vfyT->setStyleSheet(stepStyle);
    QLabel *vfyD = new QLabel("Commit Structure", verifyCard); vfyD->setStyleSheet(descStyle);
    executeDeleteButton = new QPushButton("Execute Delete", verifyCard);
    executeDeleteButton->setEnabled(false);
    executeDeleteButton->setStyleSheet(actionBtnStyle);
    executeDeleteButton->setFixedHeight(BUTTON_FIXED_HEIGHT); // 하드웨어 렌더링 세로길이 강제 고정
    vfyL->addWidget(vfyT); vfyL->addWidget(vfyD); vfyL->addStretch(); vfyL->addWidget(executeDeleteButton);
    controlGrid->addWidget(verifyCard, 1, 0);

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

    connect(startQuizButton, &QPushButton::clicked, this, &TREE_BST_QUIZ::startNewQuiz);
    connect(executeDeleteButton, &QPushButton::clicked, this, &TREE_BST_QUIZ::verifyAndExecute);

    this->updateVisuals();
    return containerWidget;
}

// ==========================================
// 3. 랜덤 트리 생성 및 상태 세팅 함수
// ==========================================
void TREE_BST_QUIZ::generateRandomTree() {
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
            root = new Quiz_Node(val);
            size++;
        } else {
            Quiz_Node* C = root;
            Quiz_Node* P = nullptr;
            while (C != nullptr) {
                P = C;
                if (val < C->key) C = C->left;
                else C = C->right;
            }
            if (val < P->key) P->left = new Quiz_Node(val);
            else P->right = new Quiz_Node(val);
            size++;
        }
    }

    if (root != nullptr) {
        QList<Quiz_Node*> q = {root};
        while(!q.isEmpty()){
            Quiz_Node* curr = q.takeFirst();
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

void TREE_BST_QUIZ::startNewQuiz() {
    generateRandomTree();
    userSelectedNodes.clear();

    executeDeleteButton->setEnabled(false);
    executeDeleteButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 0px 16px; font-weight: bold; border: 1px solid #30363D; height: 36px; }");

    currentQuizState = QuizState::PLAYING;

    quizQuestionLabel->setStyleSheet(
        "color: #F0883E; background-color: #1A1510; font-size: 13px; font-weight: bold; "
        "padding: 16px; border: 2px solid #D29922; border-radius: 10px;"
        );
    quizQuestionLabel->setText(QString("MISSION DETECTED: Delete Key [ %1 ]\nClick Target on canvas. (If it has children, select Replacement sequentially)").arg(quizTargetKey));

    statusOutput->setStyleSheet(
        "background-color: #0D1117; border: 1px solid #21262D; border-radius: 8px; "
        "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
        );
    statusOutput->setText(">> System: Canvas updated. Click a node directly to set it as the deletion target.");
    updateVisuals();
}

// ==========================================
// 4. 이벤트 필터 (인터랙션 피드백 스타일 통일)
// ==========================================
bool TREE_BST_QUIZ::eventFilter(QObject* obj, QEvent* event) {
    if (obj == view->viewport() && event->type() == QEvent::Resize) {
        return true;
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent) {
            QGraphicsItem* item = view->itemAt(mouseEvent->pos());
            if (item) {
                QVariant data = item->data(0);
                if (data.isValid()) {
                    Quiz_Node* clickedNode = reinterpret_cast<Quiz_Node*>(data.value<void*>());
                    if (!clickedNode || currentQuizState != QuizState::PLAYING) return QObject::eventFilter(obj, event);

                    if (userSelectedNodes.contains(clickedNode)) {
                        userSelectedNodes.removeOne(clickedNode);
                        statusOutput->setText(QString(">> System: Deselected node [ %1 ]").arg(clickedNode->key));
                    }
                    else {
                        if (userSelectedNodes.size() < 2) {
                            userSelectedNodes.append(clickedNode);
                            QString labelName = (userSelectedNodes.size() == 1) ? "Target" : "Replacement";
                            statusOutput->setText(QString(">> Assertion: Selected %1 [ %2 ]").arg(labelName).arg(clickedNode->key));
                        } else {
                            userSelectedNodes.clear();
                            userSelectedNodes.append(clickedNode);
                            statusOutput->setText(QString(">> System: Selection Reset. Selected Target: [ %1 ]").arg(clickedNode->key));
                        }
                    }

                    bool canEnableButton = false;
                    if (!userSelectedNodes.isEmpty()) {
                        Quiz_Node* targetNode = userSelectedNodes[0];
                        bool hasNoChildren = (!targetNode->left && !targetNode->right);

                        if (hasNoChildren) {
                            if (userSelectedNodes.size() == 1) {
                                canEnableButton = true;
                                statusOutput->setText(QString(">> System: Leaf Node [ %1 ] chosen. You can commit execution right away.").arg(targetNode->key));
                            }
                        } else {
                            if (userSelectedNodes.size() == 2) {
                                canEnableButton = true;
                            }
                        }
                    }

                    // 활성화 검증 시 보라색(A371F7) 테마 및 36px 높이 통일 유지
                    if (canEnableButton) {
                        executeDeleteButton->setEnabled(true);
                        executeDeleteButton->setStyleSheet(
                            "QPushButton { background-color: #A371F7; color: #FFFFFF; border-radius: 6px; padding: 0px 16px; font-weight: bold; border: none; height: 36px; }"
                            "QPushButton:hover { background-color: #B48EFA; }"
                            "QPushButton:pressed { background-color: #8957E5; }"
                            );
                    } else {
                        executeDeleteButton->setEnabled(false);
                        executeDeleteButton->setStyleSheet(
                            "QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 0px 16px; font-weight: bold; border: 1px solid #30363D; height: 36px; }"
                            );
                    }

                    updateVisuals();
                    return true;
                }
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

// ==========================================
// 5. 검증 및 트리 동기화 변형 로직
// ==========================================
void TREE_BST_QUIZ::verifyAndExecute() {
    if (userSelectedNodes.isEmpty()) return;

    Quiz_Node* userSelectedTarget = userSelectedNodes[0];
    bool hasNoChildren = (!userSelectedTarget->left && !userSelectedTarget->right);

    if (!hasNoChildren && userSelectedNodes.size() != 2) {
        statusOutput->setStyleSheet(
            "background-color: #1C1212; border: 1px solid #F85149; border-radius: 8px; color: #F85149; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
            );
        statusOutput->setText(">> Error: This node handles structural links. You must assert a replacement index.");
        return;
    }

    if (userSelectedTarget->key != quizTargetKey) {
        statusOutput->setStyleSheet(
            "background-color: #1C1212; border: 1px solid #F85149; border-radius: 8px; color: #F85149; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
            );
        statusOutput->setText(QString(">> Error: Incorrect target boundary! Key [ %1 ] does not match system criteria.").arg(userSelectedTarget->key));
        return;
    }

    Quiz_Node* correctReplacement = nullptr;
    if (userSelectedTarget->left && userSelectedTarget->right) {
        Quiz_Node* temp = userSelectedTarget->left;
        while (temp->right) { temp = temp->right; }
        correctReplacement = temp;
    } else if (userSelectedTarget->left) {
        correctReplacement = userSelectedTarget->left;
    } else if (userSelectedTarget->right) {
        correctReplacement = userSelectedTarget->right;
    } else {
        correctReplacement = userSelectedTarget;
    }

    if (!hasNoChildren) {
        Quiz_Node* userSelectedReplacement = userSelectedNodes[1];
        if (userSelectedReplacement != correctReplacement) {
            statusOutput->setStyleSheet(
                "background-color: #1C1212; border: 1px solid #F85149; border-radius: 8px; color: #F85149; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
                );
            statusOutput->setText(">> Error: Structural rule violation! Chosen replacement breaks global BST properties.");
            return;
        }
    }

    quizQuestionLabel->setStyleSheet(
        "color: #56B6C2; background-color: #0E161B; font-size: 13px; font-weight: bold; padding: 16px; border: 2px solid #56B6C2; border-radius: 10px;"
        );
    quizQuestionLabel->setText("CHALLENGE COMPLETE: BST CLEANLY RESTORED");

    statusOutput->setStyleSheet(
        "background-color: #0F1914; border: 1px solid #238636; border-radius: 8px; color: #3FB950; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 55px;"
        );
    statusOutput->setText(">> Success: Validation Pass! BST pipeline synchronized perfectly.");

    currentQuizState = QuizState::PRE_START;

    if (hasNoChildren) {
        if (userSelectedTarget == root) {
            root = nullptr;
        } else {
            QList<Quiz_Node*> pQueue = {root};
            while(!pQueue.isEmpty()) {
                Quiz_Node* curr = pQueue.takeFirst();
                if (!curr) continue;
                if (curr->left == userSelectedTarget) { curr->left = nullptr; break; }
                if (curr->right == userSelectedTarget) { curr->right = nullptr; break; }
                pQueue.append(curr->left); pQueue.append(curr->right);
            }
        }
        delete userSelectedTarget;
    } else {
        userSelectedTarget->key = correctReplacement->key;
        Quiz_Node* replacementChild = (correctReplacement->left) ? correctReplacement->left : correctReplacement->right;

        QList<Quiz_Node*> pQueue = {root};
        while(!pQueue.isEmpty()) {
            Quiz_Node* curr = pQueue.takeFirst();
            if (!curr) continue;

            if (curr->left == correctReplacement) {
                curr->left = replacementChild;
                break;
            }
            if (curr->right == correctReplacement) {
                curr->right = replacementChild;
                break;
            }
            pQueue.append(curr->left);
            pQueue.append(curr->right);
        }
        delete correctReplacement;
    }

    userSelectedNodes.clear();
    executeDeleteButton->setEnabled(false);
    executeDeleteButton->setStyleSheet("QPushButton { background-color: #21262D; color: #8B949E; border-radius: 6px; padding: 0px 16px; font-weight: bold; border: 1px solid #30363D; height: 36px; }");

    currentFocusNode = root;
    updateVisuals();
}

// ==========================================
// 6. 무조건 가시성 시각화 렌더러
// ==========================================
void TREE_BST_QUIZ::updateVisuals() {
    scene->clear();
    if (!root) return;

    drawNode(root, 0, 0.0, 160.0);

    QRectF bounds = scene->itemsBoundingRect();
    scene->setSceneRect(bounds.adjusted(-80, -40, 80, 80));
}

void TREE_BST_QUIZ::drawNode(Quiz_Node* node, int level, double x_pos, double x_offset) {
    if (!node) return;
    double y_pos = level * LEVEL_HEIGHT;

    QPen pen(QColor("#D29922"), 2);
    QBrush brush(QColor("#161B22"));
    QColor textColor(QColor("#E6EDF3"));

    if (node == currentFocusNode && userSelectedNodes.isEmpty()) {
        brush = QBrush(QColor("#1F6FEB")); pen = QPen(QColor("#58A6FF"), 2); textColor = QColor("#FFFFFF");
    }

    if (userSelectedNodes.contains(node)) {
        int index = userSelectedNodes.indexOf(node);
        if (index == 0) {
            brush = QBrush(QColor("#FF7B72")); pen = QPen(QColor("#F85149"), 2); textColor = QColor("#0D1117");
        }
        else if (index == 1) {
            brush = QBrush(QColor("#3FB950")); pen = QPen(QColor("#238636"), 2); textColor = QColor("#FFFFFF");
        }
    }

    auto rectItem = new QGraphicsRectItem(x_pos - NODE_WIDTH/2, y_pos, NODE_WIDTH, NODE_HEIGHT);
    rectItem->setPen(pen); rectItem->setBrush(brush);
    rectItem->setData(0, QVariant::fromValue(static_cast<void*>(node)));
    scene->addItem(rectItem);

    auto textItem = new QGraphicsTextItem(QString::number(node->key));
    textItem->setDefaultTextColor(textColor);

    QFont font = textItem->font(); font.setBold(true); font.setPointSize(10);
    textItem->setFont(font);
    textItem->setPos(x_pos - textItem->boundingRect().width()/2, y_pos + (NODE_HEIGHT - textItem->boundingRect().height())/2);
    textItem->setData(0, QVariant::fromValue(static_cast<void*>(node)));
    scene->addItem(textItem);

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