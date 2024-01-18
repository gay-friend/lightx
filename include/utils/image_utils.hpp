#pragma once
#include <opencv2/opencv.hpp>
#include <QImage>

inline cv::Mat qimage_to_mat(const QImage &qimage)
{
    int format = CV_8UC3;
    switch (qimage.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        format = CV_8UC4;
        break;
    case QImage::Format_RGB888:
        format = CV_8UC3;
        break;
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        format = CV_8UC1;
        break;

    default:
        format = CV_8UC3;
        break;
    }
    return cv::Mat(qimage.height(), qimage.width(), format, const_cast<uchar *>(qimage.bits()), qimage.bytesPerLine());
}

inline QImage mat_to_qimage(const cv::Mat &mat, bool convert_to_rgb = false)
{
    auto format = QImage::Format_RGB888;
    switch (mat.type())
    {
    case CV_8UC1:
        format = QImage::Format_Grayscale8;
        break;
    case CV_8UC3:
        format = QImage::Format_RGB888;
        break;
    case CV_8UC4:
        format = QImage::Format_ARGB32;
        break;

    default:
        break;
    }
    if (convert_to_rgb && mat.type() == CV_8UC3)
    {
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
    }
    QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), format);
    return image;
}