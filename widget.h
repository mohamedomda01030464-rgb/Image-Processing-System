#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QString>
#include <functional>
#include <memory>

class Image; // Image_Class.h بيتضمن في widget.cpp بس
class QComboBox;
class QPushButton;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onLoadClicked();
    void onSaveClicked();
    void onDeleteFilterClicked();   // بيرجّع الصورة الأصلية
    void onClearImageClicked();     // بيمسح الصورة عشان تحمّل واحدة جديدة
    void onMultiToggled(bool on);   // وضع الفلاتر المتعددة
    void onFilterChosen(int index);

private:
    void applyFilter(const std::function<bool(Image &)> &filter);
    bool maybeSave();
    bool doSave(const QString &path);
    void showPreviews(bool rebuild = true);
    void updateUiState();
    void setupModernUi();

    Ui::Widget *ui;
    QComboBox *filterCombo_ = nullptr;           // قايمة الفلاتر
    QPushButton *clearButton_ = nullptr;         // مسح الصورة
    QPushButton *multiButton_ = nullptr;         // وضع الفلاتر المتعددة

    std::unique_ptr<Image> image_;               // بعد التعديل
    std::unique_ptr<Image> baseImage_;           // الصورة الأصلية بعد التحميل
    QPixmap originalPixmap_;                      // قبل التعديل
    QPixmap afterPixmap_;                         // نسخة جاهزة للعرض (عشان الـ resize يكون سريع)
    QString currentPath_;
    bool modified_ = false;                       // فيه تعديلات ما اتحفظتش
    bool filtered_ = false;                       // الصورة عليها فلتر
    bool multiMode_ = false;                      // وضع الفلاتر المتعددة شغّال
};

#endif // WIDGET_H