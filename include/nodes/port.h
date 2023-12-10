#pragma once

#include <QMap>
#include <QPainter>
#include <QGraphicsItem>
#include <any>

/// @brief 端口
class Port
{
public:
    /// @brief 端口类型
    enum Type
    {
        Input,      // 输入
        InputForce, // 强制输入
        Output      // 输出
    };
    // 端口数据类型
    enum DataType
    {
        Int,
        Bool,
        String,
        Float,
        Bit,
        Vector2,
        Vector3,
        Color,
        Image,
    };
    /// @brief 构造函数
    Port();
    /// @brief 构造函数
    /// @param id 端口ID
    /// @param name 端口名
    /// @param type 端口类型
    /// @param data_type 数据类型
    /// @param Data 数据
    Port(uint id, const std::string &name = "Port", Type type = Input, DataType data_type = Int, QVariant data = "");
    /// @brief 析构函数
    virtual ~Port();
    /// @brief 更新端口
    void update();
    /// @brief 端口类型校验
    /// @param target_type 目标类型
    /// @return bool
    bool port_type_check(Type target_type);
    bool set_port_value(Port *port);

    /// @brief 端口存放的数据
    QVariant data;
    /// @brief 端口类型
    Type type;
    /// @brief 端口数据类型
    DataType data_type;
    /// @brief 端口ID
    uint id = 0;
    /// @brief 端口名称
    std::string name{"Port"};
    /// @brief 端口边界
    QRectF rect;
    /// @brief 端口文字边界
    QRectF text_rect;
    /// @brief 端口名字文字边界
    QRectF name_text_rect;
    /// @brief 端口文字对齐
    Qt::AlignmentFlag text_align;
    /// @brief 端口颜色
    QColor color;
    /// @brief 是否连接
    bool is_connected = false;

    /// @brief 端口数据类型颜色表
    inline static std::map<DataType, QColor> PORT_COLOR_MAP{
        {Int, QColor(106, 34, 136)},
        {Bool, QColor(200, 50, 170)},
        {String, QColor(56, 136, 34)},
        {Bit, QColor(160, 160, 100)},
        {Float, QColor(34, 136, 204)},
        {Vector2, QColor(136, 84, 68)},
        {Vector3, QColor(136, 34, 68)},
        {Color, QColor(85, 34, 136)},
        {Image, QColor(100, 200, 200)}};
    /// @brief 端口数据类型名字表
    inline static std::map<DataType, const std::string &> PORT_DATA_TYPE_NAME_MAP{
        {Int, "整数"},
        {Bool, "布尔值"},
        {String, "字符串"},
        {Float, "小数"},
        {Bit, "位"},
        {Vector2, "2维向量"},
        {Vector3, "3维向量"},
        {Color, "颜色"},
        {Image, "图片"}};
    /// @brief 端口数据类型转换表
    inline static std::map<DataType, DataType> PORT_DATA_CONVERTION_MAP{
        // int可以转换的类型
        {Int, Float},
        {Int, String},
        {Int, Bool},
        // float可转换类型
        {Float, Int},
        {Float, String},
        {Float, Bool},
        // bool 可转换类型
        {Bool, Int},
        {Bool, String},
        {Bool, Float},
        // string 可转换类型
        {String, Int},
        {String, Bool},
        {String, Float}};
};