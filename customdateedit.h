#ifndef CUSTOMDATEEDIT_H
#define CUSTOMDATEEDIT_H

#include <QDateEdit>
#include <QToolButton>
#include <QMenu>
#include <QFile>
#include <QTextCharFormat>
#include <QCalendarWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QWheelEvent>

class CustomDateEdit : public QDateEdit {
    Q_OBJECT
public:
    explicit CustomDateEdit(QWidget * parent = nullptr);
    ~CustomDateEdit();
private slots:
    void selectedMonth(int year, int month); // 选择月份的槽函数
    void initHeaderWidget(); // 初始化日历导航条
    void setDateLabelText(int year, int month);
    void selectedYear(int yearCount); // 选择年份的槽函数
    void monthMenuPopup(); // 展示月份菜单的槽函数
    void yearMenuPopup(); // 展示年份菜单的槽函数
    void menuChanged(); // 菜单内容改变的槽函数
    void changeMenu(int sequence); // 切换菜单的槽函数
private:
    QPushButton * prevYearButton; // 上一年按钮
    QPushButton * nextYearButton; // 下一年按钮
    QPushButton * prevMonthButton; // 上一月按钮
    QPushButton * nextMonthButton; // 下一月按钮
    QPushButton * yearButton; // 年份按钮
    QPushButton * monthButton; // 月份按钮
    QVBoxLayout * vBodyLayout; // 获取的父类日历控件的布局
    QGridLayout * monthGridLayout; // 月份菜单的栅格布局
    QGridLayout * yearGridLayout; // 年份菜单的栅格布局
    QWidget * monthMenu; // 月份菜单
    QWidget * yearMenu; // 年份菜单
    enum menuContent { year, month, day }; // 表示菜单种类的枚举值
protected:
    bool eventFilter(QObject * widget, QEvent * event); // 鼠标滚轮的过滤事件
};

#endif // CUSTOMDATEEDIT_H
