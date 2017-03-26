#ifndef OCRDIALOG_H
#define OCRDIALOG_H

#include <QDialog>
#include <QString>

#include "opencv2/core/core.hpp"

class QLabel;
class QTableView;

struct fontLetter{
    // Binary mask of kept parts of the character
    cv::Mat mask;
    std::string label;
    float confidence;

    // Bounding box of the character in the image
    cv::Rect rect;

    // Baseline given by Tesseract
    int baseline;

    // Baseline computed by our own
    int baseline2;

    // Test
    int GT_baseline;

    // Binary threshold
    double binarization_step = -1;

    // Smoothed image
    bool checked;
};

class OCR : public QObject
{

public:
    explicit OCR(QObject *parent = 0);
    ~OCR();

    void setParameters(const QString &tessdataParentDir, const QString &lang);
    
    void init(const QImage &ori, const QImage &bin);
    void setOriginalImage(const QImage &img);
    void setBinarizedImage(const QImage &img);
    const QImage getResultImage() {return m_originalImg;}
    //bool eventFilter(QObject *watched, QEvent *event);
    QString saveFont();
    void saveFont(const QString &filename);

    std::vector<fontLetter> getFinalFont() const;

    /*
    l_uint32 pixAtGet(PIX* pix, int x, int y){
        l_int32 wpl    = pix->wpl;
        l_uint32* data = pix->data;
        l_uint32* line = data + y * wpl;
        l_uint32 value = GET_DATA_BYTE(line, x);
        return value;
    }
    */

private:
    void process();

protected:
    void updateAlphabet();
    void computeBaselines();
    void rebinarizeCurrentLetter();
    void writeFont(const QString &filename, const std::vector<fontLetter> &finalFont) const;

    cv::Mat getLetterViewFromMask(const cv::Mat &original, const cv::Mat &mask) const;

    cv::Mat getImageFromMask(const cv::Mat &original, const cv::Mat &mask, int background_value = 237) const;
    QColor getConfidenceColor(float conf) const;
    std::vector<int> getSimilarLetters(const fontLetter &fl) const;
    
private:
    QString m_tessdataParentDir;
    QString m_language;

    QTableView *m_table;
    QImage m_originalImg;
    QImage m_binarizedImg;

    std::vector<fontLetter> m_font;
    std::vector<int> m_similarList;
    std::vector<std::pair<int, int>> m_alphabet;
    std::vector<cv::Vec4i> m_baselines;
    fontLetter m_currentLetter;
    int m_currentIndex;
    int m_maxNumberOfSymbols = 5;

    std::vector<fontLetter> m_validatedFont;
};

#endif // OCRDIALOG_H
