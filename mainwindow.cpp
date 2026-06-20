#include "mainwindow.h"
#include "./ui_stack.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>

#define n 10

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // --- 창 타이틀 및 베이스 스킨 설정 ---
    this->setWindowTitle("DS Tutor - High Performance Core");

    // 윈도우 메인 프레임 자체를 순수 검은색(#000000)으로 채워 비율 외 영역을 차단합니다.
    this->setStyleSheet("QMainWindow { background-color: #000000; }");

    // 🌟 [모바일 대응 수정] PC 하한선 고정(1100x750)을 해제하고, 모바일 해상도까지 수축 가능하도록 최소 크기 하향
    this->resize(1200, 750);
    this->setMinimumSize(320, 480);

    // 각 모듈 동적 생성 및 매핑 (기존 유지)
    stack = new STACK(n, this);
    queue = new QUEUE(n, this);
    deque = new DEQUE(n, this);
    linked_stack = new LINKED_STACK(this);
    circular_linked = new CIRCULAR_LINKED(this);
    doubly_linked = new DOUBLY_LINKED(this);
    tree_bst = new TREE_BST(this);
    tree_bst_quiz = new TREE_BST_QUIZ(this);
    tree_avl = new TREE_AVL(this);
    tree_avl_insert_quiz = new TREE_AVL_INSERT_QUIZ(this);
    tree_avl_delete_quiz = new TREE_AVL_DELETE_QUIZ(this);
    hash = new HASH(this);
    tree_sort = new TREE_SORT(this);
    bubble_sort = new BUBBLE_SORT(this);
    insertion_sort = new INSERTION_SORT(this);
    selection_sort = new SELECTION_SORT(this);
    shell_sort = new SHELL_SORT(this);
    merge_sort = new MERGE_SORT(this);
    quick_sort = new QUICK_SORT(this);
    radix_sort = new RADIX_SORT(this);

    // ==========================================
    // 기본 centralWidget을 불러와 다크 테마 테두리 및 스타일 부여
    // ==========================================
    QWidget *cWidget = this->centralWidget();
    cWidget->setStyleSheet(
        "QWidget {"
        "   background-color: #0D1117;"
        "   color: #E6EDF3;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        );

    // 🌟 [모바일 대응 수정] 여백과 간격을 미세하게 압축하여 작은 화면 영역 확보
    QHBoxLayout *mainLayout = new QHBoxLayout(cWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(12);

    // ==========================================
    // [Left Side] 네비게이션 사이드바 (가변 레이아웃 개편)
    // ==========================================
    QScrollArea *sideScroll = new QScrollArea(this);
    sideScroll->setWidgetResizable(true);

    // 🌟 [모바일 대응 수정] setFixedWidth(240) 고정을 제거하고 반응형 크기 적용
    // 화면이 PC만큼 넓을 때는 최대 240px을 유지하고, 모바일처럼 좁아지면 최소 100px까지 유연하게 수축합니다.
    sideScroll->setMaximumWidth(240);
    sideScroll->setMinimumWidth(100);

    sideScroll->setStyleSheet(
        "QScrollArea { border: none; background-color: #161B22; border-radius: 8px; }"
        "QScrollBar:vertical { background: #161B22; width: 6px; }"
        "QScrollBar::handle:vertical { background: #30363D; border-radius: 3px; }"
        );

    QWidget *sideBarWidget = new QWidget();

    // 개별 버튼 스타일은 updateSidebarHighlight에서 제어하므로 기본 배경만 지정합니다.
    sideBarWidget->setStyleSheet("QWidget { background-color: #161B22; }");

    QVBoxLayout *navLayout = new QVBoxLayout(sideBarWidget);
    navLayout->setContentsMargins(8, 12, 8, 12);
    navLayout->setSpacing(8);

    // 네비게이션 버튼 생성 (기존 유지)
    QPushButton *btnStack              = new QPushButton("Stack", sideBarWidget);
    QPushButton *btnQueue              = new QPushButton("Queue", sideBarWidget);
    QPushButton *btnDeque              = new QPushButton("Deque", sideBarWidget);
    QPushButton *btnLinkedStack        = new QPushButton("Linked Stack", sideBarWidget);
    QPushButton *btnCircularLinked     = new QPushButton("Linked Circular Queue", sideBarWidget);
    QPushButton *btnDoublyLinked       = new QPushButton("Doubly Linked List", sideBarWidget);
    QPushButton *btnTreeBST            = new QPushButton("Binary Search Tree", sideBarWidget);
    QPushButton *btnTreeBSTQuiz        = new QPushButton("BST Quiz", sideBarWidget);
    QPushButton *btnTreeAVL            = new QPushButton("AVL Tree", sideBarWidget);
    QPushButton *btnTreeAVLInsertQuiz  = new QPushButton("AVL Insert Quiz", sideBarWidget);
    QPushButton *btnTreeAVLDeleteQuiz  = new QPushButton("AVL Delete Quiz", sideBarWidget);
    QPushButton *btnHash               = new QPushButton("Hash Table", sideBarWidget);
    QPushButton *btnTreeSort           = new QPushButton("Binary Tree Sort", sideBarWidget);
    QPushButton *btnBubbleSort         = new QPushButton("Bubble Sort", sideBarWidget);
    QPushButton *btnInsertionSort      = new QPushButton("Insertion Sort", sideBarWidget);
    QPushButton *btnSelectionSort      = new QPushButton("Selection Sort", sideBarWidget);
    QPushButton *btnShellSort          = new QPushButton("Shell Sort", sideBarWidget);
    QPushButton *btnMergeSort          = new QPushButton("Merge Sort", sideBarWidget);
    QPushButton *btnQuickSort          = new QPushButton("Quick Sort", sideBarWidget);
    QPushButton *btnRadixSort          = new QPushButton("Radix Sort", sideBarWidget);

    // 🌟 [하이라이트 기능 추가] 일괄 관리 및 스타일 순회를 위해 리스트에 저장
    sidebarButtons = {
        btnStack, btnQueue, btnDeque, btnLinkedStack, btnCircularLinked, btnDoublyLinked,
        btnTreeBST, btnTreeBSTQuiz, btnTreeAVL, btnTreeAVLInsertQuiz, btnTreeAVLDeleteQuiz,
        btnHash, btnTreeSort, btnBubbleSort, btnInsertionSort, btnSelectionSort,
        btnShellSort, btnMergeSort, btnQuickSort, btnRadixSort
    };

    navLayout->addWidget(btnStack);          navLayout->addWidget(btnQueue);
    navLayout->addWidget(btnDeque);          navLayout->addWidget(btnLinkedStack);
    navLayout->addWidget(btnCircularLinked); navLayout->addWidget(btnDoublyLinked);
    navLayout->addWidget(btnTreeBST);        navLayout->addWidget(btnTreeBSTQuiz);
    navLayout->addWidget(btnTreeAVL);        navLayout->addWidget(btnTreeAVLInsertQuiz);
    navLayout->addWidget(btnTreeAVLDeleteQuiz); navLayout->addWidget(btnHash);
    navLayout->addWidget(btnTreeSort);       navLayout->addWidget(btnBubbleSort);
    navLayout->addWidget(btnInsertionSort);  navLayout->addWidget(btnSelectionSort);
    navLayout->addWidget(btnShellSort);      navLayout->addWidget(btnMergeSort);
    navLayout->addWidget(btnQuickSort);      navLayout->addWidget(btnRadixSort);
    navLayout->addStretch();

    sideScroll->setWidget(sideBarWidget);

    // ==========================================
    // [Right Side] QStackedWidget 본문 배치
    // ==========================================
    QStackedWidget *stackedWidget = new QStackedWidget(this);
    stackedWidget->setStyleSheet(
        "QStackedWidget {"
        "   background-color: #0D1117;"
        "   border: 1px solid #21262D;"
        "   border-radius: 8px;"
        "}"
        );

    // 각 모듈 세팅 화면 등록 (기존 유지)
    stackedWidget->addWidget(stack->setting());                // Index 0
    stackedWidget->addWidget(queue->setting());                // Index 1
    stackedWidget->addWidget(deque->setting());                // Index 2
    stackedWidget->addWidget(linked_stack->setting());         // Index 3
    stackedWidget->addWidget(circular_linked->setting());      // Index 4
    stackedWidget->addWidget(doubly_linked->setting());        // Index 5
    stackedWidget->addWidget(tree_bst->setting());             // Index 6
    stackedWidget->addWidget(tree_bst_quiz->setting());        // Index 7
    stackedWidget->addWidget(tree_avl->setting());             // Index 8
    stackedWidget->addWidget(tree_avl_insert_quiz->setting()); // Index 9
    stackedWidget->addWidget(tree_avl_delete_quiz->setting()); // Index 10
    stackedWidget->addWidget(hash->setting());                 // Index 11
    stackedWidget->addWidget(tree_sort->setting());            // Index 12
    stackedWidget->addWidget(bubble_sort->setting());          // Index 13
    stackedWidget->addWidget(insertion_sort->setting());       // Index 14
    stackedWidget->addWidget(selection_sort->setting());       // Index 15
    stackedWidget->addWidget(shell_sort->setting());           // Index 16
    stackedWidget->addWidget(merge_sort->setting());           // Index 17
    stackedWidget->addWidget(quick_sort->setting());           // Index 18
    stackedWidget->addWidget(radix_sort->setting());           // Index 19

    // 🌟 조립 (사이드바는 가변 배정, 본문 스택 위젯이 남은 모든 공간을 비율로 차지)
    mainLayout->addWidget(sideScroll);
    mainLayout->addWidget(stackedWidget, 1);

    // ==========================================
    // 화면 전환 라우팅 및 하이라이트 연결 🌟
    // ==========================================
    connect(btnStack, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(0); updateSidebarHighlight(btnStack); });
    connect(btnQueue, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(1); updateSidebarHighlight(btnQueue); });
    connect(btnDeque, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(2); updateSidebarHighlight(btnDeque); });
    connect(btnLinkedStack, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(3); updateSidebarHighlight(btnLinkedStack); });
    connect(btnCircularLinked, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(4); updateSidebarHighlight(btnCircularLinked); });
    connect(btnDoublyLinked, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(5); updateSidebarHighlight(btnDoublyLinked); });
    connect(btnTreeBST, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(6); updateSidebarHighlight(btnTreeBST); });
    connect(btnTreeBSTQuiz, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(7); updateSidebarHighlight(btnTreeBSTQuiz); });
    connect(btnTreeAVL, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(8); updateSidebarHighlight(btnTreeAVL); });
    connect(btnTreeAVLInsertQuiz, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(9); updateSidebarHighlight(btnTreeAVLInsertQuiz); });
    connect(btnTreeAVLDeleteQuiz, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(10); updateSidebarHighlight(btnTreeAVLDeleteQuiz); });
    connect(btnHash, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(11); updateSidebarHighlight(btnHash); });
    connect(btnTreeSort, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(12); updateSidebarHighlight(btnTreeSort); });
    connect(btnBubbleSort, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(13); updateSidebarHighlight(btnBubbleSort); });
    connect(btnInsertionSort, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(14); updateSidebarHighlight(btnInsertionSort); });
    connect(btnSelectionSort, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(15); updateSidebarHighlight(btnSelectionSort); });
    connect(btnShellSort, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(16); updateSidebarHighlight(btnShellSort); });
    connect(btnMergeSort, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(17); updateSidebarHighlight(btnMergeSort); });
    connect(btnQuickSort, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(18); updateSidebarHighlight(btnQuickSort); });
    connect(btnRadixSort, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(19); updateSidebarHighlight(btnRadixSort); });

    // 프로그램 최초 구동 시 Stack 메뉴가 선택된 상태로 시작하도록 초기화 🌟
    stackedWidget->setCurrentIndex(0);
    updateSidebarHighlight(btnStack);
}

MainWindow::~MainWindow()
{
    delete stack;              delete queue;                delete deque;
    delete linked_stack;       delete circular_linked;      delete doubly_linked;
    delete tree_bst;           delete tree_bst_quiz;        delete tree_avl;
    delete tree_avl_insert_quiz; delete tree_avl_delete_quiz; delete hash;
    delete tree_sort;          delete bubble_sort;          delete insertion_sort;
    delete selection_sort;     delete shell_sort;           delete merge_sort;
    delete quick_sort;         delete radix_sort;
    delete ui;
}

// ==========================================
// 실시간 해상도 비율 유지 제어 로직 (반응형 유연화 모드)
// ==========================================
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    // 유지하고자 하는 기본 타겟 해상도 비율 (1200 : 750 = 1.6)
    const double targetAspect = 1200.0 / 750.0;

    int currentWidth = this->width();
    int currentHeight = this->height();

    int finalW = currentWidth;
    int finalH = currentHeight;

    // 종횡비 변환 계산 (기존 유지)
    if (currentWidth / (double)currentHeight > targetAspect) {
        finalW = qRound(currentHeight * targetAspect);
    } else {
        finalH = qRound(currentWidth / targetAspect);
    }

    // 🌟 [모바일 대응 수정] 강제 하한 가드선(finalH < 750) 제거
    if (finalH < 300) {
        finalH = 300;
        finalW = qRound(300 * targetAspect);
    }

    // 내장 centralWidget() 크기 및 위치 가드 커스텀 수동 배치
    if (this->centralWidget()) {
        this->centralWidget()->setFixedSize(finalW, finalH);
        this->centralWidget()->move((currentWidth - finalW) / 2, (currentHeight - finalH) / 2);
    }
}

// ==========================================
// 🌟 실시간 메뉴 하이라이트 제어 함수 구현
// ==========================================
void MainWindow::updateSidebarHighlight(QPushButton* selectedButton)
{
    for (QPushButton* btn : sidebarButtons) {
        if (btn == selectedButton) {
            // [선택된 상태] GitHub 테마 감성의 깔끔한 블루 포인트 스타일 적용
            btn->setStyleSheet(
                "QPushButton {"
                "   background-color: #1F6FEB;"
                "   color: #FFFFFF;"
                "   border: 1px solid #58A6FF;"
                "   border-radius: 6px;"
                "   padding: 10px 12px;"
                "   font-size: 12px;"
                "   font-weight: bold;"
                "   text-align: left;"
                "}"
                );
        } else {
            // [선택되지 않은 상태] 기존에 작성하신 일반 비활성 다크 모드 스타일 유지
            btn->setStyleSheet(
                "QPushButton {"
                "   background-color: #21262D;"
                "   color: #C9D1D9;"
                "   border: 1px solid #30363D;"
                "   border-radius: 6px;"
                "   padding: 10px 12px;"
                "   font-size: 12px;"
                "   font-weight: bold;"
                "   text-align: left;"
                "}"
                "QPushButton:hover {"
                "   background-color: #30363D;"
                "   color: #FFFFFF;"
                "   border-color: #8B949E;"
                "}"
                "QPushButton:pressed {"
                "   background-color: #0D1117;"
                "}"
                );
        }
    }
}