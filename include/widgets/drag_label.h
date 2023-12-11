#pragma once

#include <QLabel>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>

/// @brief 可拖动标签
class DragLabel : public QLabel
{
    Q_OBJECT
public:
    /// @brief 构造函数
    /// @param text 内容
    /// @param parent 父窗体
    DragLabel(const std::string &text, QWidget *parent = nullptr);
    /// @brief 鼠标移动事件
    /// @param e 事件源
    void mouseMoveEvent(QMouseEvent *e);
};