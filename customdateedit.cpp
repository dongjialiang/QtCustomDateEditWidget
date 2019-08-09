#include "customdateedit.h"

CustomDateEdit::CustomDateEdit(QWidget * parent) :
    QDateEdit (parent) {
    setDate(QDate::currentDate());
    setCalendarPopup(true); // 设置日期编辑为弹出式
    calendarWidget()->setLocale(QLocale(QLocale::Chinese)); // 设置程序的区域为中文使用地区
    calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader); // 去除日历左边的列头
    calendarWidget()->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames); // 只显示周的一个字
    calendarWidget()->setFirstDayOfWeek(Qt::Monday); // 设置每周由周一开始
    calendarWidget()->setNavigationBarVisible(false);
    QTextCharFormat dateCharFormat; // 日期文本样式格式
    dateCharFormat.setBackground(Qt::white); // 背景色
    QTextCharFormat headerCharFormat;
    headerCharFormat.setBackground(Qt::white);
    headerCharFormat.setForeground(Qt::gray);
    calendarWidget()->setHeaderTextFormat(headerCharFormat); // 设置"周几"字体的样式
    /* 星期(周)的每一天的样式 */
    for (int dayOfWeek = Qt::Monday; dayOfWeek <= Qt::Sunday; dayOfWeek++) {
        calendarWidget()->setWeekdayTextFormat(Qt::DayOfWeek(dayOfWeek), dateCharFormat);
    }
    QFile dateEditFile(":/style/dateEditStyle.qss"); // 取样式文件
    if (dateEditFile.open(QFile::ReadOnly)) { // 文件可以只读方式打开
        QString styleSheet = QLatin1String(dateEditFile.readAll()); // 获取样式文件内容
        setStyleSheet(styleSheet); // 设置样式表
        dateEditFile.close(); // 关闭文件
    }
    initHeaderWidget();
}
CustomDateEdit::~CustomDateEdit() {}
/* 初始化日期导航条 */
void CustomDateEdit::initHeaderWidget() {
    QWidget * topWidget = new QWidget(this);
    topWidget->setObjectName("CalendarHeaderWidget");
    topWidget->setFixedHeight(40);
    topWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout * hboxLayout = new QHBoxLayout(this);
    hboxLayout->setContentsMargins(12, 0, 12, 0);
    hboxLayout->setSpacing(4);

    prevYearButton = new QPushButton(this); // 上一年按钮
    prevYearButton->setObjectName("CalendarPrevYearButton");
    prevYearButton->setFixedSize(16, 16);
    prevYearButton->setToolTip("上一年");
    nextYearButton = new QPushButton(this); // 下一年按钮
    nextYearButton->setObjectName("CalendarNextYearButton");
    nextYearButton->setFixedSize(16, 16);
    nextYearButton->setToolTip("下一年");
    prevMonthButton = new QPushButton(this); // 上个月按钮
    prevMonthButton->setObjectName("CalendarPrevMonthButton");
    prevMonthButton->setFixedSize(16, 16);
    prevMonthButton->setToolTip("上一个月");
    nextMonthButton = new QPushButton(this); // 下个月按钮
    nextMonthButton->setObjectName("CalendarNextMonthButton");
    nextMonthButton->setFixedSize(16, 16);
    nextMonthButton->setToolTip("下一个月");
    yearButton = new QPushButton(this); // 年份按钮
    yearButton->setObjectName("yearButton");
    monthButton = new QPushButton(this); // 月份按钮
    monthButton->setObjectName("monthButton");
    setDateLabelText(QDate::currentDate().year(), QDate::currentDate().month()); // 设置初始的日期文本

    hboxLayout->addWidget(prevYearButton);
    hboxLayout->addWidget(prevMonthButton);
    hboxLayout->addStretch();
    hboxLayout->addWidget(yearButton);
    hboxLayout->addWidget(monthButton);
    hboxLayout->addStretch();
    hboxLayout->addWidget(nextMonthButton);
    hboxLayout->addWidget(nextYearButton);
    topWidget->setLayout(hboxLayout);

    vBodyLayout = qobject_cast<QVBoxLayout *>(calendarWidget()->layout()); // 获取日历控件的布局
    vBodyLayout->insertWidget(0, topWidget);

    monthMenu = new QWidget(this); // 月份菜单
    monthMenu->setObjectName("MonthMenu");
    vBodyLayout->insertWidget(1, monthMenu);
    monthMenu->hide();
    monthGridLayout = new QGridLayout(this); // 月份菜单的栅格布局
    monthMenu->setLayout(monthGridLayout);
    monthGridLayout->setSpacing(0);
    monthMenu->installEventFilter(this);

    yearMenu = new QWidget(this); // 年份菜单
    yearMenu->setObjectName("YearMenu");
    vBodyLayout->insertWidget(2, yearMenu);
    yearMenu->hide();
    yearGridLayout = new QGridLayout(this); // 年份菜单的栅格布局
    yearMenu->setLayout(yearGridLayout);
    yearGridLayout->setSpacing(0);
    yearMenu->installEventFilter(this);

    connect(prevYearButton, &QPushButton::clicked, [this](){
        selectedYear(-1);
    }); // 上一年按钮的信号槽
    connect(nextYearButton, &QPushButton::clicked, [this](){
        selectedYear(1);
    }); // 下一年按钮的信号槽
    connect(prevMonthButton, &QPushButton::clicked, [this](){
        int year = calendarWidget()->selectedDate().year();
        int month = calendarWidget()->selectedDate().month();
        selectedMonth(year, month - 1);
    }); // 上一月按钮的信号槽
    connect(nextMonthButton, &QPushButton::clicked, [this](){
        int year = calendarWidget()->selectedDate().year();
        int month = calendarWidget()->selectedDate().month();
        selectedMonth(year, month + 1);
    }); // 下一月按钮的信号槽
    connect(calendarWidget(), &QCalendarWidget::currentPageChanged, [this](int year, int month) {
        setDateLabelText(year, month);
        menuChanged();
    }); // 当日期改变时设置日期标签的文字
    connect(calendarWidget(), &QCalendarWidget::currentPageChanged,
            [this](int year, int month){
        selectedMonth(year, month);
    }); // 月份改变时立即作用到日期的信号槽
    connect(monthButton, &QPushButton::clicked, [this]() {
        if (monthMenu->isHidden()) {
            monthMenuPopup();
        } else {
            changeMenu(menuContent::day);
        }
    }); // 月份按钮的信号槽
    connect(yearButton, &QPushButton::clicked, [this]() {
        if (yearMenu->isHidden()) {
            yearMenuPopup();
        } else if (!yearMenu->isHidden()) {
            changeMenu(menuContent::day);
        }
    }); // 年份按钮的信号槽
}
/* 设置日期标签的文本 */
void CustomDateEdit::setDateLabelText(int year, int month) {
    yearButton->setText(QStringLiteral("%1年").arg(year));
    monthButton->setText(QStringLiteral("%1月").arg(month));
}
/* 选择月份 */
void CustomDateEdit::selectedMonth(int year, int month) {
    if (year >= 1752 && year <= 9999) {
        QDate seletedDate = calendarWidget()->selectedDate(); // 当前选中的月份
        QDate validDate = calendarWidget()->selectedDate().addMonths(month - seletedDate.month()); // 按月份加减时间,返回有效时间
        setDate(validDate);
    }
}
/* 选择年份 */
void CustomDateEdit::selectedYear(int yearCount) {
    QDate validDate = calendarWidget()->selectedDate().addYears(yearCount); // 按年份加减时间,返回有效日期
    setDate(validDate);
}
/* 展示月份菜单 */
void CustomDateEdit::monthMenuPopup() {
    changeMenu(menuContent::month);
    QList<QPushButton *> monthList;
    int currentMonth = calendarWidget()->selectedDate().month();
    int initializationMonth = currentMonth - 12;
    int monthTableCount = 0;
    int monthCount = initializationMonth;
    for (int monthTableRow = 0; monthTableRow < 4; monthTableRow++) {
        for (int monthTableColumn = 0; monthTableColumn < 4; monthTableColumn++) {
            monthList << new QPushButton(this);
            monthCount = (monthCount <= 0)
                    ? (monthCount + 12) : ((++monthCount) > 12)
                      ? (monthCount % 12) : (monthCount);
            monthList[monthTableCount]->setText(QStringLiteral("%1月").arg(monthCount));
            if (monthTableCount > 2 && monthTableCount < 13) {
                monthList[monthTableCount]->setObjectName("monthListEnabled");
            } else if (monthTableCount <= 2 || monthTableCount >= 13) {
                monthList[monthTableCount]->setObjectName("monthListDisabled");
            }
            monthGridLayout->addWidget(monthList[monthTableCount], monthTableRow, monthTableColumn);
            connect(monthList[monthTableCount], &QPushButton::clicked, [=]() {
                int year = calendarWidget()->selectedDate().year();
                selectedMonth(year, monthCount);
                changeMenu(menuContent::day);
            });
            monthTableCount++;
        }
    }
}
/* 展示年份菜单 */
void CustomDateEdit::yearMenuPopup() {
    changeMenu(menuContent::year);
    QList<QPushButton *> yearList;
    int currentYear = calendarWidget()->selectedDate().year();
    int initializationYear = currentYear - 12;
    int yearTableCount = 0;
    int yearCount = initializationYear;
    for (int yearTableRow = 0; yearTableRow < 4; yearTableRow++) {
        for (int yearTableColumn = 0; yearTableColumn < 4; yearTableColumn++) {
            yearList << new QPushButton(this);
            yearList[yearTableCount]->setText(QStringLiteral("%1年").arg(yearCount));
            yearCount = 1 + yearCount;
            if (yearTableCount <= 2 || yearTableCount >= 13) {
                yearList[yearTableCount]->setObjectName("yearListDisabled");
            } else if (yearTableCount > 2 && yearTableCount < 13) {
                yearList[yearTableCount]->setObjectName("yearListEnabled");
            }
            yearGridLayout->addWidget(yearList[yearTableCount], yearTableRow, yearTableColumn);
            connect(yearList[yearTableCount], &QPushButton::clicked, [=]() {
                QString yearText = yearList[yearTableCount]->text();
                int year = yearText.mid(0, yearText.length() - 1).toInt();
                int currentYear = calendarWidget()->selectedDate().year();
                selectedYear(year - currentYear);
                changeMenu(menuContent::day);
            });
            yearTableCount++;
        }
    }
}
/* 菜单发生变化时展示菜单的新内容 */
void CustomDateEdit::menuChanged() {
    if (!yearMenu->isHidden()) {
        yearMenuPopup();
    } else if (!monthMenu->isHidden()) {
        monthMenuPopup();
    }
}
/* 切换菜单 */
void CustomDateEdit::changeMenu(int sequence) {
    switch (sequence) {
    case 0: yearMenu->show();
        vBodyLayout->itemAt(4)->widget()->hide();
        monthMenu->hide();
        yearButton->setStyleSheet("border: 1px solid gray;");
        monthButton->setStyleSheet("border: 0;");
        break;
    case 1: monthMenu->show();
        vBodyLayout->itemAt(4)->widget()->hide();
        yearMenu->hide();
        monthButton->setStyleSheet("border: 1px solid gray;");
        yearButton->setStyleSheet("border: 0;");
        break;
    case 2: vBodyLayout->itemAt(4)->widget()->show();
        monthMenu->hide();
        yearMenu->hide();
        monthButton->setStyleSheet("border: 0;");
        yearButton->setStyleSheet("border: 0;");
        break;
    }
}
/* 过滤鼠标滚轮事件 */
bool CustomDateEdit::eventFilter(QObject *widget, QEvent *event) {
    if (widget == yearMenu) {
            if (event->type() == QEvent::Wheel) {
                QWheelEvent * wheelEvent = static_cast<QWheelEvent *>(event);
                int angelDeltaY = wheelEvent->angleDelta().y();
                if (angelDeltaY > 0) {
                    selectedYear(-4);
                    menuChanged();
                } else if (angelDeltaY < 0) {
                    selectedYear(4);
                    menuChanged();
                }
            }
        } else if (widget == monthMenu) {
            if (event->type() == QEvent::Wheel) {
                QWheelEvent * wheelEvent = static_cast<QWheelEvent *>(event);
                int angelDeltaY = wheelEvent->angleDelta().y();
                int year = calendarWidget()->selectedDate().year();
                int month = calendarWidget()->selectedDate().month();
                if (angelDeltaY > 0) {
                    selectedMonth(year, month - 4);
                    menuChanged();
                } else if (angelDeltaY < 0) {
                    selectedMonth(year, month + 4);
                    menuChanged();
                }
            }
        }
    return true;
}
