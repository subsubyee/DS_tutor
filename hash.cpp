#include "hash.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

// ==========================================
// 1. 수학적 조건 검증 및 크기 자동 결정 (PPT 알고리즘)
// ==========================================
bool HASH::is_prime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

bool HASH::is_4kplus3(int n) {
    return (n % 4 == 3);
}

int HASH::calculate_optimal_size(int target_nmax) {
    int start_size = static_cast<int>(target_nmax * 1.33) + 1;
    while (true) {
        if (is_prime(start_size) && is_4kplus3(start_size)) {
            return start_size;
        }
        start_size++;
    }
}

// ==========================================
// 2. 생성자 및 소멸자
// ==========================================
HASH::HASH(QMainWindow* _parent) : QObject(_parent) {
    parent = _parent;
    nmax = 0;
    max = 0;
    current_len = 0;
    hashCells = nullptr;
}

HASH::~HASH() {
    if (hashCells) {
        delete[] hashCells;
    }
}

// ==========================================
// 3. UI 레이아웃 구성 (STACK 코드 기반 완벽 통일)
// ==========================================
QWidget* HASH::setting() {
    // --- 메인 컨테이너 및 전역 배경 설정 (Modern Obsidian Dark Theme) ---
    QWidget *containerWidget = new QWidget(parent);
    containerWidget->setStyleSheet(
        "QWidget { background-color: #0B0E14; color: #CDD6F4; font-family: 'Segoe UI', -apple-system, sans-serif; }"
        );

    QHBoxLayout *mainLayout = new QHBoxLayout(containerWidget);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(28);

    // ==========================================
    // [Left Area] VISUALIZATION (고정형 해시 테이블 슬롯 레이아웃)
    // ==========================================
    QWidget *leftWidget = new QWidget(containerWidget);
    QVBoxLayout *leftVLayout = new QVBoxLayout(leftWidget);
    leftVLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *visTitle = new QLabel("● VISUALIZATION\n\nHash Table Grid Slots", leftWidget);
    visTitle->setStyleSheet("font-size: 13px; font-weight: bold; color: #58A6FF; letter-spacing: 1px; margin-bottom: 8px;");
    leftVLayout->addWidget(visTitle);

    // 해시 슬롯 전용 부드러운 스크롤 영역
    scrollArea = new QScrollArea(leftWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea { border: none; background-color: transparent; }"
        "QScrollBar:vertical { background: #161B22; width: 8px; margin-left: 2px; }"
        "QScrollBar::handle:vertical { background: #30363D; border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: #8B949E; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { background: none; height: 0px; }"
        );

    hashContainer = new QWidget();
    hashContainer->setStyleSheet("background-color: #11141A; border-radius: 12px; border: 1px solid #21262D;");

    // 컴팩트한 그리드(4열 배치) 레이아웃 구성
    hashGrid = new QGridLayout(hashContainer);
    hashGrid->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    hashGrid->setContentsMargins(20, 20, 20, 20);
    hashGrid->setSpacing(12);

    scrollArea->setWidget(hashContainer);
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

    QLabel *controlsTitle = new QLabel("● CONTROLS\n\nHash System Dashboard", rightContentWidget);
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
                         "QLineEdit:focus { border: 1px solid #58A6FF; }"
                         "QLineEdit:disabled { background-color: #11141A; color: #484F58; border-color: #21262D; }";
    QString btnStyle    = "QPushButton { background-color: #238636; color: #FFFFFF; border-radius: 6px; padding: 6px 16px; font-weight: bold; border: none; }"
                       "QPushButton:hover { background-color: #2EA043; }"
                       "QPushButton:pressed { background-color: #248039; }"
                       "QPushButton:disabled { background-color: #21262D; color: #484F58; }";
    QString setupBtnStyle = "QPushButton { background-color: #1F6FEB; color: #FFFFFF; border-radius: 6px; padding: 6px 16px; font-weight: bold; border: none; }"
                            "QPushButton:hover { background-color: #388BFD; }"
                            "QPushButton:pressed { background-color: #0C57D0; }";

    // --- Card 1: Setup Capacity ---
    QWidget *setupCard = new QWidget(rightContentWidget); setupCard->setAttribute(Qt::WA_StyledBackground, true); setupCard->setStyleSheet(cardStyle);
    QVBoxLayout *setupL = new QVBoxLayout(setupCard); QHBoxLayout *setupTop = new QHBoxLayout();
    QLabel *sTitle = new QLabel("initialize()", setupCard); sTitle->setStyleSheet(titleStyle);
    QLabel *sDesc = new QLabel("Set dynamic optimal size", setupCard); sDesc->setStyleSheet(descStyle);
    QLabel *sComp = new QLabel("Step 1", setupCard); sComp->setStyleSheet(compStyle);
    setupTop->addWidget(sTitle); setupTop->addWidget(sDesc); setupTop->addStretch(); setupTop->addWidget(sComp);
    QHBoxLayout *setupBot = new QHBoxLayout(); sizeInput = new QLineEdit(setupCard); sizeInput->setPlaceholderText("Target capacity (e.g. 7)"); sizeInput->setStyleSheet(inputStyle);
    setSizeButton = new QPushButton("Init Table", setupCard); setSizeButton->setStyleSheet(setupBtnStyle);
    setupBot->addWidget(sizeInput); setupBot->addWidget(setSizeButton); setupL->addLayout(setupTop); setupL->addLayout(setupBot);
    gridLayout->addWidget(setupCard, 0, 0);

    // --- Card 2: Insert Element ---
    QWidget *insertCard = new QWidget(rightContentWidget); insertCard->setAttribute(Qt::WA_StyledBackground, true); insertCard->setStyleSheet(cardStyle);
    QVBoxLayout *insertL = new QVBoxLayout(insertCard); QHBoxLayout *insertTop = new QHBoxLayout();
    QLabel *iTitle = new QLabel("insert()", insertCard); iTitle->setStyleSheet(titleStyle);
    QLabel *iDesc = new QLabel("Store key via double hash", insertCard); iDesc->setStyleSheet(descStyle);
    QLabel *iComp = new QLabel("O(1) Avg", insertCard); iComp->setStyleSheet(compStyle);
    insertTop->addWidget(iTitle); insertTop->addWidget(iDesc); insertTop->addStretch(); insertTop->addWidget(iComp);
    QHBoxLayout *insertBot = new QHBoxLayout(); valueInput = new QLineEdit(insertCard); valueInput->setPlaceholderText("Enter non-negative key..."); valueInput->setStyleSheet(inputStyle); valueInput->setDisabled(true);
    insertButton = new QPushButton("Insert", insertCard); insertButton->setStyleSheet(btnStyle); insertButton->setDisabled(true);
    insertBot->addWidget(valueInput); insertBot->addWidget(insertButton); insertL->addLayout(insertTop); insertL->addLayout(insertBot);
    gridLayout->addWidget(insertCard, 0, 1);

    // --- Card 3: Delete Element ---
    QWidget *deleteCard = new QWidget(rightContentWidget); deleteCard->setAttribute(Qt::WA_StyledBackground, true); deleteCard->setStyleSheet(cardStyle);
    QVBoxLayout *deleteL = new QVBoxLayout(deleteCard); QHBoxLayout *deleteTop = new QHBoxLayout();
    QLabel *dTitle = new QLabel("delete()", deleteCard); dTitle->setStyleSheet(titleStyle);
    QLabel *dDesc = new QLabel("Soft-delete key (Dummy)", deleteCard); dDesc->setStyleSheet(descStyle);
    QLabel *dComp = new QLabel("O(1) Avg", deleteCard); dComp->setStyleSheet(compStyle);
    deleteTop->addWidget(dTitle); deleteTop->addWidget(dDesc); deleteTop->addStretch(); deleteTop->addWidget(dComp);
    QHBoxLayout *deleteBot = new QHBoxLayout(); deleteInput = new QLineEdit(deleteCard); deleteInput->setPlaceholderText("Enter key to remove..."); deleteInput->setStyleSheet(inputStyle); deleteInput->setDisabled(true);
    deleteButton = new QPushButton("Delete", deleteCard); deleteButton->setStyleSheet(
        "QPushButton { background-color: #DA3637; color: #FFFFFF; border-radius: 6px; padding: 6px 16px; font-weight: bold; border: none; }"
        "QPushButton:hover { background-color: #F85149; }"
        "QPushButton:pressed { background-color: #B82526; }"
        "QPushButton:disabled { background-color: #21262D; color: #484F58; }"
        ); deleteButton->setDisabled(true);
    deleteBot->addWidget(deleteInput); deleteBot->addWidget(deleteButton); deleteL->addLayout(deleteTop); deleteL->addLayout(deleteBot);
    gridLayout->addWidget(deleteCard, 1, 0);

    rightLayout->addLayout(gridLayout);

    // --- Bottom: Operation Log Terminal ---
    QLabel *logTitle = new QLabel(">_ Integrated System Live Log", rightContentWidget);
    logTitle->setStyleSheet("color: #8B949E; font-size: 11px; font-family: monospace; font-weight: bold; margin-top: 6px;");
    rightLayout->addWidget(logTitle);

    statusOutput = new QLabel("System Ready. Please setup hash table size first.", rightContentWidget);
    statusOutput->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    statusOutput->setWordWrap(true);
    statusOutput->setStyleSheet(
        "background-color: #0D1117; border: 1px solid #21262D; border-radius: 8px; "
        "color: #58A6FF; font-family: 'Consolas', monospace; font-size: 12px; padding: 12px; min-height: 140px;"
        );
    rightLayout->addWidget(statusOutput);

    rightScrollArea->setWidget(rightContentWidget);

    // ==========================================
    // 레이아웃 조립 및 이벤트 시그널 연결
    // ==========================================
    mainLayout->addWidget(leftWidget, 1);
    mainLayout->addWidget(rightScrollArea);

    connect(setSizeButton, &QPushButton::clicked, this, &HASH::setupHashSize);
    connect(insertButton, &QPushButton::clicked, this, &HASH::insertValue);
    connect(deleteButton, &QPushButton::clicked, this, &HASH::deleteValue);

    return containerWidget;
}

// ==========================================
// 4. 입력 개수 기반 크기 세팅 & 빈 해시 표시
// ==========================================
void HASH::setupHashSize() {
    QString rawInput = sizeInput->text().trimmed();
    if (rawInput.isEmpty()) {
        statusOutput->setText(">> Error: Enter target capacity first.");
        return;
    }

    int inputN = rawInput.toInt();
    if (inputN <= 0) {
        statusOutput->setText(">> Error: Please enter a positive integer.");
        return;
    }

    // 기존 동적 슬롯 객체 클리어 후 재구성
    if (hashCells) {
        for(int i = 0; i < max; ++i) {
            delete hashCells[i];
        }
        delete[] hashCells;
    }

    nmax = inputN;
    max = calculate_optimal_size(nmax);
    current_len = 0;

    hashTable.clear();
    hashTable.resize(max, "None");

    hashCells = new QLabel*[max];

    // [수정] 한 화면 크기에 맞게 한 줄당 3개 배치로 변경
    int columns = 3;

    for (int i = 0; i < max; ++i) {
        hashCells[i] = new QLabel(hashContainer);
        hashCells[i]->setAlignment(Qt::AlignCenter);
        hashCells[i]->setFixedSize(180, 70); // STACK과 완전히 동일한 노드 규격 고정

        // 행(i / 3)과 열(i % 3) 계산으로 Grid 배치
        hashGrid->addWidget(hashCells[i], i / columns, i % columns);
    }

    valueInput->setEnabled(true);
    insertButton->setEnabled(true);
    deleteInput->setEnabled(true);
    deleteButton->setEnabled(true);
    sizeInput->clear();

    statusOutput->setText(QString(">> Success: Hash Table Initialized.\n ├─ Target Capacity: %1\n ├─ Optimal Size (M): %2\n └─ Status: Ready for Operations.").arg(nmax).arg(max));

    update();
}

// ==========================================
// 7. 시각화 셀 스타일 드로잉 제어 (STACK 스타일 및 규격 완전 결합)
// ==========================================
void HASH::update() {
    if (max <= 0 || !hashCells) return;

    // 만약 update() 내부에서도 columns나 좌표 계산 로직이 들어간다면
    // 여기도 3으로 맞춰줍니다. (현재 제공된 코드 기준으로는 스타일 유지만 하므로 안전합니다.)
    for (int i = 0; i < max; ++i) {
        hashCells[i]->setTextFormat(Qt::RichText);

        if (hashTable[i] == "Deleted") {
            // [소프트 파스텔 크롬 레드] 삭제 데이터 상태 스타일 피드백
            hashCells[i]->setText(QString("Index [ %1 ]<br><span style='font-size: 13pt; color: #F87171;'><b>Deleted</b></span>").arg(i, 2, 10, QChar('0')));
            hashCells[i]->setStyleSheet(
                "QLabel {"
                "   border: 2px dashed #EF4444;"
                "   border-radius: 8px;"
                "   background-color: #451A1A;"
                "   color: #F87171;"
                "   font-weight: bold;"
                "}"
                );
        }
        else if (hashTable[i] != "None") {
            // [골드/엠버 다크스펙트럼 계열] STACK의 활성화 노드 색감 매칭 기법
            hashCells[i]->setText(
                QString("Bucket (Idx: %1)<br><span style='font-size: 16pt;'>%2</span>")
                    .arg(QString::number(i).rightJustified(2, '0'))
                    .arg(hashTable[i])
                );
            hashCells[i]->setStyleSheet(
                "QLabel {"
                "   border-radius: 8px;"
                "   background-color: #D29922;"
                "   color: #0D1117;"
                "   font-weight: bold;"
                "}"
                );
        }
        else {
            // [통일] 비어 있는 슬롯 (STACK의 Empty 노드 점선 대시 스타일링 일치)
            hashCells[i]->setText(QString("[ %1 ]<br><span style='font-size: 10pt;'>Empty</span>").arg(i, 2, 10, QChar('0')));
            hashCells[i]->setStyleSheet(
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
// 5. 해시 삽입 알고리즘
// ==========================================
void HASH::insertValue() {
    QString rawValue = valueInput->text().trimmed();
    if (rawValue.isEmpty()) {
        statusOutput->setText(">> Error: Enter a value to insert.");
        return;
    }

    bool isInt;
    int numericKey = rawValue.toInt(&isInt);
    if (!isInt || numericKey < 0) {
        statusOutput->setText(">> Error: Only non-negative integers are allowed.");
        return;
    }

    if (current_len >= max) {
        statusOutput->setText(">> Rejected: Insertion Failed. Hash Table is strictly full!");
        return;
    }

    int ip = numericKey % max;
    int q = numericKey / max;
    int offset = q;
    if (offset % max == 0) {
        offset = 9967;
    }

    int current_index = ip;
    int pass = 0;
    QString logMsg = QString(">> Insert Start: Key %1\n ├─ Home Address: %2 (Offset: %3)").arg(numericKey).arg(ip).arg(offset);

    while (pass < max) {
        if (hashTable[current_index] == "None" || hashTable[current_index] == "Deleted") {
            QString prevStatus = hashTable[current_index];
            hashTable[current_index] = QString::number(numericKey);
            current_len++;

            if (prevStatus == "Deleted") {
                logMsg += QString("\n └─ [Recycled] Dummy bucket at Index [%1] re-used.").arg(current_index);
            } else {
                logMsg += QString("\n └─ [Success] Stored safely at Index [%1]. (Collisions: %2)").arg(current_index).arg(pass);
            }

            statusOutput->setText(logMsg);
            valueInput->clear();
            update();
            return;
        }

        logMsg += QString("\n ├─ Collision at Index [%1] (Occupied by '%2')").arg(current_index).arg(hashTable[current_index]);
        current_index = (current_index + offset) % max;
        pass++;
    }

    statusOutput->setText(">> Error: Insertion Failed. Loop limit exceeded.");
}

// ==========================================
// 6. 해시 삭제 및 더미화 알고리즘
// ==========================================
void HASH::deleteValue() {
    QString rawValue = deleteInput->text().trimmed();
    if (rawValue.isEmpty()) {
        statusOutput->setText(">> Error: Enter a value to delete.");
        return;
    }

    bool isInt;
    int numericKey = rawValue.toInt(&isInt);
    if (!isInt || numericKey < 0) {
        statusOutput->setText(">> Error: Invalid target key.");
        return;
    }

    int ip = numericKey % max;
    int q = numericKey / max;
    int offset = q;
    if (offset % max == 0) {
        offset = 9967;
    }

    int current_index = ip;
    int pass = 0;
    QString logMsg = QString(">> Delete Start: Target Key %1\n ├─ Home Address: %2").arg(numericKey).arg(ip);

    while (pass < max) {
        if (hashTable[current_index] == "None") {
            break;
        }

        if (hashTable[current_index] == QString::number(numericKey)) {
            hashTable[current_index] = "Deleted";
            current_len--;

            logMsg += QString("\n └─ [Success] Deleted from Index [%1]. Left 'Deleted' dummy.").arg(current_index);
            statusOutput->setText(logMsg);
            deleteInput->clear();
            update();
            return;
        }

        if (hashTable[current_index] == "Deleted") {
            logMsg += QString("\n ├─ Passed Dummy 'Deleted' at Index [%1]").arg(current_index);
        } else {
            logMsg += QString("\n ├─ Mismatch at Index [%1] (Value: '%2')").arg(current_index).arg(hashTable[current_index]);
        }

        current_index = (current_index + offset) % max;
        pass++;
    }

    logMsg += QString("\n └─ [Failed] Key %1 does not exist in the table.").arg(numericKey);
    statusOutput->setText(logMsg);
    deleteInput->clear();
}