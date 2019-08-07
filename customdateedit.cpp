#include "customdateedit.h"

CustomDateEdit::CustomDateEdit(QWidget * parent) :
    QDateEdit (parent) {
    setDate(QDate::currentDate());
    setCalendarPopup(true); // 设置日期编辑为弹出式
    calendarWidget()->setMinimumDate(QDate(2017, 7, 15)); // 设置日历最小日期,不影响日期编辑控件的最小日期
    // setMinimumDate(QDate(2017, 7, 15)); // 设置日期编辑控件的最小日期
    calendarWidget()->setMaximumDate(QDate::currentDate()); // 设置日历最大日期为当前日期,不影响日期编辑控件的最大日期
    // setMaximumDate(QDate::currentDate()); // 设置日期编辑控件的最大日期
    int minimumYear = calendarWidget()->minimumDate().year(); // 最小日期年份
    int maximumYear = QDate::currentDate().year();  // 当前年份

    yearMenu = new QMenu(this);
    yearButton = calendarWidget()->findChild<QToolButton *>(QLatin1String("qt_calendar_yearbutton")); // 年份按钮
    yearButton->setPopupMode(QToolButton::MenuButtonPopup); // 防止yearButton点击后触发"qt_calendar_yearedit"(QSpinBox)控件
    QList<QAction *> listAction = yearMenu->actions(); // 以数组形式存储动作
    /* 循环处理每个动作 */
    for (int year = minimumYear; year <= maximumYear; year++) {
        int yearCount = year - minimumYear;
        listAction << new QAction(QString::number(year), this);
        connect(listAction.at(yearCount), SIGNAL(triggered()),
                SLOT(selectedYear())); // 动作数组的每个动作选择年份的槽函数
    }
    yearMenu->addActions(listAction); // 添加动作数组到菜单
    yearButton->setMenu(yearMenu);
    connect(yearButton, &QToolButton::clicked,
            [this]{
        yearButton->showMenu();
    }); // 点击年份按钮时弹出年份菜单的信号槽
    connect(calendarWidget(), &QCalendarWidget::currentPageChanged,
            [this](int year, int month){
        selectedMonth(year, month);
    }); // 月份改变时立即作用到日期的信号槽

    QTextCharFormat dateTextFormat; // 日期文本样式格式
    dateTextFormat.setBackground(QColor(20, 20, 20, 153)); // 背景色
    dateTextFormat.setForeground(Qt::white); // 作用到字体颜色
    /* 星期(周)的每一天的样式 */
    for (int dayOfWeek = Qt::Monday; dayOfWeek <= Qt::Sunday; dayOfWeek++) {
        calendarWidget()->setWeekdayTextFormat(Qt::DayOfWeek(dayOfWeek), dateTextFormat);
    }
    QFile dateEditFile(":/style/dateEditStyle.qss"); // 取样式文件
    if (dateEditFile.open(QFile::ReadOnly)) { // 文件可以只读方式打开
        QString styleSheet = QLatin1String(dateEditFile.readAll()); // 获取样式文件内容
        setStyleSheet(styleSheet); // 设置样式表
        dateEditFile.close(); // 关闭文件
    }
}
CustomDateEdit::~CustomDateEdit() {}
/* 选择年份 */
void CustomDateEdit::selectedYear() {
    int year = qobject_cast<QAction *>(sender())->text().toInt(); // 点击的信号发射器的文本就是选中的年份
    int month = calendarWidget()->selectedDate().month(); // 当前选中的月份
    int day = calendarWidget()->selectedDate().day(); // 当前选中的天
    setDate(QDate(year, month, day));
}
/* 选择月份 */
void CustomDateEdit::selectedMonth(int year, int month) {
    QDate seletedDate = calendarWidget()->selectedDate(); // 当前选中的月份
    int day = seletedDate.addMonths(month - seletedDate.month()) // 按月份加减时间
            .day(); // 获取的月份中如果不存在该天会变成该月的最后一天
    setDate(QDate(year, month, day));
}
