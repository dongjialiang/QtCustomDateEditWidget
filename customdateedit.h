#ifndef CUSTOMDATEEDIT_H
#define CUSTOMDATEEDIT_H

#include <QDateEdit>
#include <QToolButton>
#include <QMenu>
#include <QFile>
#include <QTextCharFormat>
#include <QCalendarWidget>

class CustomDateEdit : public QDateEdit {
    Q_OBJECT
public:
    explicit CustomDateEdit(QWidget * parent = nullptr);
    ~CustomDateEdit();
private slots:
    void selectedYear(); // 选择年份的槽函数
    void selectedMonth(int year, int month); // 选择月份的槽函数
private:
    QToolButton * yearButton; // 年份按钮
    QMenu * yearMenu; // 年份菜单
};

#endif // CUSTOMDATEEDIT_H
