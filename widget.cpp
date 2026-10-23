#include "widget.h"
#include "ui_widget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QColorDialog>
#include <QCloseEvent>
#include <QImage>
#include <QFileInfo>
#include <QStringList>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <stdexcept>
#include <vector>

// لازم يتضمن بعد هيدرز Qt (لأنه بيعمل #define pixel و ll)
#include "Image_Class.h"

namespace {

const QString kAppName = "Nova Studio";   // غيّر الاسم زي ما تحب

inline unsigned char clampByte(int v)
{
    return static_cast<unsigned char>(v < 0 ? 0 : (v > 255 ? 255 : v));
}

inline unsigned char clampByteF(float v)
{
    return clampByte(static_cast<int>(v));
}

inline unsigned char *at(Image &img, int x, int y)
{
    return img.imageData + (static_cast<size_t>(y) * img.width + x) * 3;
}

QPixmap toPixmap(const Image &img)
{
    QImage q(img.imageData, img.width, img.height, img.width * 3, QImage::Format_RGB888);
    return QPixmap::fromImage(q.copy());
}

void showOn(QLabel *label, const QPixmap &pix)
{
    if (pix.isNull()) {
        label->clear();
        return;
    }
    label->setPixmap(pix.scaled(label->size() - QSize(4, 4),
                                Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

bool askInt(QWidget *parent, const QString &title, const QString &label,
            int value, int mn, int mx, int &out)
{
    bool ok = false;
    out = QInputDialog::getInt(parent, title, label, value, mn, mx, 1, &ok);
    return ok;
}

// ====================== Filters ======================

void invertFilter(Image &img)
{
    size_t total = static_cast<size_t>(img.width) * img.height * 3;
    for (size_t i = 0; i < total; ++i)
        img.imageData[i] = 255 - img.imageData[i];
}

void darkenFilter(Image &img)
{
    size_t total = static_cast<size_t>(img.width) * img.height * 3;
    for (size_t i = 0; i < total; ++i)
        img.imageData[i] = static_cast<unsigned char>(img.imageData[i] * 0.5);
}

void lightenFilter(Image &img)
{
    size_t total = static_cast<size_t>(img.width) * img.height * 3;
    for (size_t i = 0; i < total; ++i)
        img.imageData[i] = clampByte(static_cast<int>(img.imageData[i] * 1.5));
}

void blurFilter(Image &img, int passes)
{
    for (int p = 0; p < passes; ++p) {
        Image src(img);
        for (int y = 0; y < img.height; ++y) {
            for (int x = 0; x < img.width; ++x) {
                for (int c = 0; c < 3; ++c) {
                    int sum = 0;
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            int nx = std::clamp(x + dx, 0, img.width - 1);
                            int ny = std::clamp(y + dy, 0, img.height - 1);
                            sum += at(src, nx, ny)[c];
                        }
                    }
                    at(img, x, y)[c] = static_cast<unsigned char>(sum / 9);
                }
            }
        }
    }
}

void infraredFilter(Image &img)
{
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            unsigned char *p = at(img, x, y);
            p[0] = 255;
            p[1] = 255 - p[1];
            p[2] = 255 - p[2];
        }
    }
}

void grayScaleFilter(Image &img)
{
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            unsigned char *p = at(img, x, y);
            unsigned char avg = static_cast<unsigned char>((p[0] + p[1] + p[2]) / 3);
            p[0] = p[1] = p[2] = avg;
        }
    }
}

void blackAndWhiteFilter(Image &img)
{
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            unsigned char *p = at(img, x, y);
            int gray = (p[0] + p[1] + p[2]) / 3;
            unsigned char v = gray >= 128 ? 255 : 0;
            p[0] = p[1] = p[2] = v;
        }
    }
}

void flipHorizontal(Image &img)
{
    for (int y = 0; y < img.height; ++y)
        for (int x = 0; x < img.width / 2; ++x)
            for (int c = 0; c < 3; ++c)
                std::swap(at(img, x, y)[c], at(img, img.width - 1 - x, y)[c]);
}

void flipVertical(Image &img)
{
    for (int y = 0; y < img.height / 2; ++y)
        for (int x = 0; x < img.width; ++x)
            for (int c = 0; c < 3; ++c)
                std::swap(at(img, x, y)[c], at(img, x, img.height - 1 - y)[c]);
}

void rotateImage(Image &img, int angle)
{
    int w = img.width, h = img.height;
    if (angle == 180) {
        Image r(w, h);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                for (int c = 0; c < 3; ++c)
                    at(r, w - 1 - x, h - 1 - y)[c] = at(img, x, y)[c];
        img = r;
    } else if (angle == 90 || angle == 270) {
        Image r(h, w);
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                for (int c = 0; c < 3; ++c) {
                    if (angle == 90)
                        at(r, h - 1 - y, x)[c] = at(img, x, y)[c];
                    else
                        at(r, y, w - 1 - x)[c] = at(img, x, y)[c];
                }
        img = r;
    }
}

void resizeImage(Image &img, int newW, int newH)
{
    Image r(newW, newH);
    float facW = static_cast<float>(img.width) / newW;
    float facH = static_cast<float>(img.height) / newH;
    for (int y = 0; y < newH; ++y) {
        for (int x = 0; x < newW; ++x) {
            int sx = std::min(static_cast<int>(x * facW), img.width - 1);
            int sy = std::min(static_cast<int>(y * facH), img.height - 1);
            for (int c = 0; c < 3; ++c)
                at(r, x, y)[c] = at(img, sx, sy)[c];
        }
    }
    img = r;
}

void cropImage(Image &img, int x0, int y0, int w, int h)
{
    Image r(w, h);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            for (int c = 0; c < 3; ++c)
                at(r, x, y)[c] = at(img, x0 + x, y0 + y)[c];
    img = r;
}

void wanoPurpleFilter(Image &img)
{
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            unsigned char *p = at(img, x, y);
            int r = std::min(255, static_cast<int>(p[0] * 1.1));
            int g = static_cast<int>(p[1] * 0.5);
            int b = std::min(255, static_cast<int>(p[2] * 1.1));
            p[0] = static_cast<unsigned char>(r);
            p[1] = static_cast<unsigned char>(g);
            p[2] = static_cast<unsigned char>(b);
        }
    }
}

void detectEdgesFilter(Image &img)
{
    const int w = img.width, h = img.height;
    const int threshold = 90;

    std::vector<int> gray(static_cast<size_t>(w) * h);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            unsigned char *p = at(img, x, y);
            gray[static_cast<size_t>(y) * w + x] = (p[0] + p[1] + p[2]) / 3;
        }

    const int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    const int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    Image result(w, h);
    std::fill(result.imageData, result.imageData + static_cast<size_t>(w) * h * 3, 255);

    for (int y = 1; y < h - 1; ++y) {
        for (int x = 1; x < w - 1; ++x) {
            int sumx = 0, sumy = 0;
            for (int m = -1; m <= 1; ++m)
                for (int n = -1; n <= 1; ++n) {
                    int g = gray[static_cast<size_t>(y + n) * w + (x + m)];
                    sumx += g * gx[m + 1][n + 1];
                    sumy += g * gy[m + 1][n + 1];
                }
            int mag = std::abs(sumx) + std::abs(sumy);
            unsigned char v = mag > threshold ? 0 : 255;
            unsigned char *p = at(result, x, y);
            p[0] = p[1] = p[2] = v;
        }
    }
    img = result;
}

// ---- Cinema (Old TV) helpers ----
void jitterRows(Image &img, int maxShift, int rate)
{
    Image copy(img);
    for (int y = 0; y < img.height; ++y) {
        if (std::rand() % rate != 0) continue;
        int off = std::rand() % (2 * maxShift + 1) - maxShift;
        for (int x = 0; x < img.width; ++x) {
            int sx = std::clamp(x - off, 0, img.width - 1);
            for (int c = 0; c < 3; ++c)
                at(img, x, y)[c] = at(copy, sx, y)[c];
        }
    }
}

void rgbShift(Image &img, int shift)
{
    Image copy(img);
    for (int y = 0; y < img.height; ++y)
        for (int x = 0; x < img.width; ++x) {
            int xr = std::clamp(x - shift, 0, img.width - 1);
            int xb = std::clamp(x + shift, 0, img.width - 1);
            at(img, x, y)[0] = at(copy, xr, y)[0];
            at(img, x, y)[2] = at(copy, xb, y)[2];
        }
}

void contrastSaturation(Image &img, float contrast, float saturation)
{
    for (int y = 0; y < img.height; ++y)
        for (int x = 0; x < img.width; ++x) {
            unsigned char *p = at(img, x, y);
            float r = (p[0] - 128) * contrast + 128;
            float g = (p[1] - 128) * contrast + 128;
            float b = (p[2] - 128) * contrast + 128;
            float gray = 0.299f * r + 0.587f * g + 0.114f * b;
            p[0] = clampByteF(gray + (r - gray) * saturation);
            p[1] = clampByteF(gray + (g - gray) * saturation);
            p[2] = clampByteF(gray + (b - gray) * saturation);
        }
}

void scanlines(Image &img, int step, float darkness)
{
    for (int y = 0; y < img.height; y += step)
        for (int x = 0; x < img.width; ++x)
            for (int c = 0; c < 3; ++c)
                at(img, x, y)[c] = clampByteF(at(img, x, y)[c] * darkness);
}

void glitchBar(Image &img, int barHeight)
{
    for (int y = 0; y < barHeight && y < img.height; ++y)
        for (int x = 0; x < img.width; ++x) {
            unsigned char *p = at(img, x, y);
            p[0] = clampByteF(p[0] * 0.4f + 255 * 0.6f);
            p[1] = clampByteF(p[1] * 0.4f + 120 * 0.6f);
            p[2] = clampByteF(p[2] * 0.4f + 255 * 0.6f);
        }
}

void addNoise(Image &img, int strength)
{
    for (int y = 0; y < img.height; ++y)
        for (int x = 0; x < img.width; ++x) {
            int n = std::rand() % (2 * strength + 1) - strength;
            unsigned char *p = at(img, x, y);
            for (int c = 0; c < 3; ++c)
                p[c] = clampByte(p[c] + n);
        }
}

void cinemaFilter(Image &img)
{
    jitterRows(img, 12, 25);
    rgbShift(img, 4);
    contrastSaturation(img, 1.2f, 1.3f);
    scanlines(img, 3, 0.6f);
    glitchBar(img, std::max(2, img.height / 100));
    addNoise(img, 8);
}

void simpleFrame(Image &img, int size, int r, int g, int b)
{
    for (int y = 0; y < img.height; ++y)
        for (int x = 0; x < img.width; ++x)
            if (x < size || y < size || x >= img.width - size || y >= img.height - size) {
                unsigned char *p = at(img, x, y);
                p[0] = clampByte(r);
                p[1] = clampByte(g);
                p[2] = clampByte(b);
            }
}

void fancyFrame(Image &img, int outer, int stripe)
{
    int start = outer / 2 - stripe / 2;
    int end = outer / 2 + stripe / 2;
    int w = img.width, h = img.height;

    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            if (x < outer || y < outer || x >= w - outer || y >= h - outer) {
                unsigned char *p = at(img, x, y);
                bool inStripe = (x >= start && x < end) || (x >= w - end && x < w - start)
                                || (y >= start && y < end) || (y >= h - end && y < h - start);
                if (inStripe) { p[0] = 255; p[1] = 255; p[2] = 255; }
                else          { p[0] = 0;   p[1] = 100; p[2] = 255; }
            }
}

// ---- Oil Painting ----
void oilPaintFilter(Image &img, int radius, int levels)
{
    Image src(img);
    const int w = img.width, h = img.height;
    std::vector<int> cnt(levels), sr(levels), sg(levels), sb(levels);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            std::fill(cnt.begin(), cnt.end(), 0);
            std::fill(sr.begin(), sr.end(), 0);
            std::fill(sg.begin(), sg.end(), 0);
            std::fill(sb.begin(), sb.end(), 0);

            for (int dy = -radius; dy <= radius; ++dy) {
                int ny = std::clamp(y + dy, 0, h - 1);
                for (int dx = -radius; dx <= radius; ++dx) {
                    int nx = std::clamp(x + dx, 0, w - 1);
                    unsigned char *p = at(src, nx, ny);
                    int bin = ((p[0] + p[1] + p[2]) / 3) * (levels - 1) / 255;
                    cnt[bin]++;
                    sr[bin] += p[0];
                    sg[bin] += p[1];
                    sb[bin] += p[2];
                }
            }

            int best = 0;
            for (int i = 1; i < levels; ++i)
                if (cnt[i] > cnt[best]) best = i;

            unsigned char *out = at(img, x, y);
            out[0] = static_cast<unsigned char>(sr[best] / cnt[best]);
            out[1] = static_cast<unsigned char>(sg[best] / cnt[best]);
            out[2] = static_cast<unsigned char>(sb[best] / cnt[best]);
        }
    }
}

// ---- Skew (horizontal shear) ----
// الزاوية الموجبة بتميّل الصورة يمين، والسالبة شمال
void skewImage(Image &img, int angleDeg)
{
    const double pi = 3.14159265358979323846;
    const double t = std::tan(angleDeg * pi / 180.0);
    const int w = img.width, h = img.height;
    const int extra = static_cast<int>(std::ceil(std::abs(t) * (h - 1)));
    const int newW = w + extra;

    Image r(newW, h);
    std::fill(r.imageData, r.imageData + static_cast<size_t>(newW) * h * 3, 255); // خلفية بيضا

    for (int y = 0; y < h; ++y) {
        double shift = (t >= 0) ? std::abs(t) * (h - 1 - y)
                                : std::abs(t) * y;
        int s = static_cast<int>(std::lround(shift));
        for (int x = 0; x < w; ++x)
            for (int c = 0; c < 3; ++c)
                at(r, x + s, y)[c] = at(img, x, y)[c];
    }
    img = r;
}

// ---- Land Of Wano ----
void landOfWanoFilter(Image &img)
{
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            unsigned char *p = at(img, x, y);
            int r = static_cast<int>(p[0] * 1.2);
            int g = static_cast<int>(p[1] * 1.1);
            int b = static_cast<int>(p[2] * 0.9);
            p[0] = static_cast<unsigned char>(std::min(255, r));
            p[1] = static_cast<unsigned char>(std::min(255, g));
            p[2] = static_cast<unsigned char>(std::min(255, b));
        }
    }
}

// ---- Merge Images ----
// بيرجع false لو المستخدم لغى أو حصل خطأ (والصورة بتفضل زي ما هي)
bool mergeImagesFilter(Image &img, QWidget *parent)
{
    QString path = QFileDialog::getOpenFileName(parent, "اختر الصورة التانية", QString(),
                                                "Images (*.png *.jpg *.jpeg *.bmp *.tga)");
    if (path.isEmpty()) return false;

    Image b;
    try {
        b.loadNewImage(path.toStdString());
    } catch (const std::exception &e) {
        QMessageBox::warning(parent, "Error", QString("Couldn't load image:\n") + e.what());
        return false;
    }

    Image a(img);
    int newW = a.width;
    int newH = a.height;

    if (a.width != b.width || a.height != b.height) {
        const QStringList options = {
            "1- تكبير الصورتين لأكبر عرض وارتفاع",
            "2- دمج المنطقة المشتركة بس",
            "3- حط الصورة التانية فوق الأولى في مكان تختاره"
        };
        bool ok = false;
        QString sel = QInputDialog::getItem(parent, "Merge Images",
                                            "الصورتين مقاسهم مختلف، اختار:",
                                            options, 0, false, &ok);
        if (!ok) return false;
        int choice = static_cast<int>(options.indexOf(sel)) + 1;

        if (choice == 3) {
            int x, y;
            if (!askInt(parent, "Merge Images",
                        "X اللي تبدأ منه الصورة التانية (0 = الحافة الشمال):",
                        0, -(b.width - 1), a.width - 1, x)) return false;
            if (!askInt(parent, "Merge Images",
                        "Y اللي تبدأ منه الصورة التانية (0 = الحافة فوق):",
                        0, -(b.height - 1), a.height - 1, y)) return false;

            int startX = std::max(0, x);
            int startY = std::max(0, y);
            int endX = std::min(a.width, x + b.width);
            int endY = std::min(a.height, y + b.height);

            if (startX >= endX || startY >= endY) {
                QMessageBox::information(parent, "Merge Images",
                                         "الصورتين مش بيتقاطعوا! الدمج اتلغى.");
                return false;
            }

            Image result(a);
            for (int j = startY; j < endY; ++j)
                for (int i = startX; i < endX; ++i)
                    for (int c = 0; c < 3; ++c)
                        at(result, i, j)[c] = static_cast<unsigned char>(
                            (at(a, i, j)[c] + at(b, i - x, j - y)[c]) / 2);
            img = result;
            return true;
        }

        if (choice == 1) {
            newW = std::max(a.width, b.width);
            newH = std::max(a.height, b.height);
            resizeImage(a, newW, newH);
            resizeImage(b, newW, newH);
        } else {
            newW = std::min(a.width, b.width);
            newH = std::min(a.height, b.height);
        }
    }

    Image result(newW, newH);
    for (int j = 0; j < newH; ++j)
        for (int i = 0; i < newW; ++i)
            for (int c = 0; c < 3; ++c)
                at(result, i, j)[c] = static_cast<unsigned char>(
                    (at(a, i, j)[c] + at(b, i, j)[c]) / 2);
    img = result;
    return true;
}

// ====================== قايمة الفلاتر ======================
// كل عنصر: اسم يظهر في القايمة + الدالة اللي بتتنفذ (parent للـ dialogs)

struct FilterItem {
    QString name;
    std::function<bool(Image &, QWidget *)> run;
};

const std::vector<FilterItem> &filterList()
{
    static const std::vector<FilterItem> list = {
        {"Invert",      [](Image &i, QWidget *) { invertFilter(i); return true; }},
        {"Darken",      [](Image &i, QWidget *) { darkenFilter(i); return true; }},
        {"Lighten",     [](Image &i, QWidget *) { lightenFilter(i); return true; }},

        {"Blur", [](Image &i, QWidget *p) {
             int passes;
             if (!askInt(p, "Blur", "درجة الـ Blur (عدد المرات):", 1, 1, 30, passes)) return false;
             blurFilter(i, passes);
             return true;
         }},

        {"Infrared",        [](Image &i, QWidget *) { infraredFilter(i); return true; }},
        {"GrayScale",       [](Image &i, QWidget *) { grayScaleFilter(i); return true; }},
        {"Black And White", [](Image &i, QWidget *) { blackAndWhiteFilter(i); return true; }},

        {"Flip Horizontal", [](Image &i, QWidget *) { flipHorizontal(i); return true; }},
        {"Flip Vertical",   [](Image &i, QWidget *) { flipVertical(i); return true; }},

        {"Rotate", [](Image &i, QWidget *p) {
             const QStringList options = {"90", "180", "270"};
             bool ok = false;
             QString sel = QInputDialog::getItem(p, "Rotate", "اختر زاوية الدوران (بالدرجات):",
                                                 options, 0, false, &ok);
             if (!ok) return false;
             rotateImage(i, sel.toInt());
             return true;
         }},

        {"Resize", [](Image &i, QWidget *p) {
             int w, h;
             if (!askInt(p, "Resize", "العرض الجديد (Width):", i.width, 1, 10000, w)) return false;
             if (!askInt(p, "Resize", "الارتفاع الجديد (Height):", i.height, 1, 10000, h)) return false;
             resizeImage(i, w, h);
             return true;
         }},

        {"Crop", [](Image &i, QWidget *p) {
             int x, y, w, h;
             if (!askInt(p, "Crop", QString("نقطة البداية X (العرض الحالي %1):").arg(i.width),
                         0, 0, i.width - 1, x)) return false;
             if (!askInt(p, "Crop", QString("نقطة البداية Y (الارتفاع الحالي %1):").arg(i.height),
                         0, 0, i.height - 1, y)) return false;
             if (!askInt(p, "Crop", "عرض القص (W):", i.width - x, 1, i.width - x, w)) return false;
             if (!askInt(p, "Crop", "ارتفاع القص (H):", i.height - y, 1, i.height - y, h)) return false;
             cropImage(i, x, y, w, h);
             return true;
         }},

        {"Wano Purple",  [](Image &i, QWidget *) { wanoPurpleFilter(i); return true; }},

        {"Oil Painting", [](Image &i, QWidget *p) {
             int radius, levels;
             if (!askInt(p, "Oil Painting", "حجم الفرشاة (radius):", 3, 1, 10, radius)) return false;
             if (!askInt(p, "Oil Painting", "عدد مستويات اللون (أقل = شكل رسم أكتر):", 20, 4, 50, levels)) return false;
             oilPaintFilter(i, radius, levels);
             return true;
         }},

        {"Skew", [](Image &i, QWidget *p) {
             int angle;
             if (!askInt(p, "Skew", "الزاوية بالدرجات (السالب بيميّل شمال):", 20, -60, 60, angle)) return false;
             if (angle == 0) return false;
             skewImage(i, angle);
             return true;
         }},

        {"Detect Edges", [](Image &i, QWidget *) { detectEdgesFilter(i); return true; }},
        {"Cinema (Old TV)", [](Image &i, QWidget *) { cinemaFilter(i); return true; }},

        {"Simple Frame", [](Image &i, QWidget *p) {
             int maxSize = std::max(1, std::min(i.width, i.height) / 2);
             int size;
             if (!askInt(p, "Simple Frame", "سُمك الإطار:", std::min(20, maxSize), 1, maxSize, size))
                 return false;
             QColor color = QColorDialog::getColor(Qt::blue, p, "اختر لون الإطار");
             if (!color.isValid()) return false;
             simpleFrame(i, size, color.red(), color.green(), color.blue());
             return true;
         }},

        {"Fancy Frame", [](Image &i, QWidget *p) {
             int maxSize = std::max(2, std::min(i.width, i.height) / 2);
             int outer, stripe;
             if (!askInt(p, "Fancy Frame", "السُمك الخارجي:", std::min(20, maxSize), 2, maxSize, outer))
                 return false;
             if (!askInt(p, "Fancy Frame", "سُمك الخط الداخلي:", std::min(3, outer), 1, outer, stripe))
                 return false;
             fancyFrame(i, outer, stripe);
             return true;
         }},

        // ---- الفلاتر الجديدة (في آخر القايمة عشان الترتيب القديم ما يتغيّرش) ----
        {"Land Of Wano", [](Image &i, QWidget *) { landOfWanoFilter(i); return true; }},

        {"Merge Images", [](Image &i, QWidget *p) { return mergeImagesFilter(i, p); }},
    };
    return list;
}

} // namespace

// ====================== Widget ======================

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // ندوّر على القايمة مهما كان اسمها، ولو مفيش قايمة نعملها
    filterCombo_ = findChild<QComboBox *>("filterComboBox");
    if (!filterCombo_)
        filterCombo_ = findChild<QComboBox *>();
    if (!filterCombo_) {
        filterCombo_ = new QComboBox(this);
        filterCombo_->setGeometry(710, 220, 190, 30);
    }

    filterCombo_->clear();
    filterCombo_->addItem("Choose Filter");
    for (const auto &f : filterList())
        filterCombo_->addItem(f.name);
    filterCombo_->setMaxVisibleItems(15);
    filterCombo_->setEnabled(true);
    filterCombo_->show();
    filterCombo_->raise();

    connect(filterCombo_, QOverload<int>::of(&QComboBox::activated),
            this, &Widget::onFilterChosen);

    connect(ui->loadButton, &QPushButton::clicked, this, &Widget::onLoadClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &Widget::onSaveClicked);

    // زرار "undo" القديم بقى "Delete Filter": بيرجّع الصورة الأصلية
    ui->undoButton->setText("Delete Filter");
    connect(ui->undoButton, &QPushButton::clicked, this, &Widget::onDeleteFilterClicked);

    // زرار مسح الصورة عشان تحمّل واحدة جديدة
    clearButton_ = new QPushButton("Clear_Img", this);
    connect(clearButton_, &QPushButton::clicked, this, &Widget::onClearImageClicked);

    // زرار وضع الفلاتر المتعددة (شغّال/مقفول)
    multiButton_ = new QPushButton("Multi Filters: OFF", this);
    multiButton_->setCheckable(true);
    connect(multiButton_, &QPushButton::toggled, this, &Widget::onMultiToggled);

    setupModernUi();   // الـ Layouts + الألوان + الاسم الجديد
    updateUiState();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setupModernUi()
{
    // نلاقي العنوان وكلمتين Before / After وكلمتين الكابشن العربي من نصهم
    QLabel *title = nullptr, *capBefore = nullptr, *capAfter = nullptr;
    QLabel *tagBefore = nullptr, *tagAfter = nullptr;
    const auto labels = findChildren<QLabel *>();
    for (QLabel *l : labels) {
        const QString t = l->text();
        const QString tt = t.trimmed();
        if (tt.compare("Before", Qt::CaseInsensitive) == 0)      tagBefore = l;
        else if (tt.compare("After", Qt::CaseInsensitive) == 0)  tagAfter = l;
        else if (t.contains("Filter_project"))                   title = l;
        else if (t.contains(QString::fromUtf8("قبل")))          capBefore = l;
        else if (t.contains(QString::fromUtf8("بعد")))          capAfter = l;
    }
    if (!title)     title     = new QLabel(this);
    if (!tagBefore) tagBefore = new QLabel("Before", this);
    if (!tagAfter)  tagAfter  = new QLabel("After", this);

    // نشيل أي كلام عادي، ونسيب Before و After بس
    if (capBefore) capBefore->hide();
    if (capAfter)  capAfter->hide();

    title->setText(kAppName);
    title->setObjectName("appTitle");
    title->setAlignment(Qt::AlignCenter);
    setWindowTitle(kAppName);

    // إطارات الصور: تتمدد مع الشاشة والصورة تتوسط جواها
    for (QLabel *l : {ui->beforeImageLabel, ui->afterImageLabel}) {
        l->setObjectName("imgFrame");
        l->setAlignment(Qt::AlignCenter);
        l->setScaledContents(false);
        l->setMinimumSize(320, 400);
        l->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        l->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }
    for (QLabel *tag : {tagBefore, tagAfter}) {
        tag->setObjectName("tagText");
        tag->setAlignment(Qt::AlignCenter);
        tag->setMinimumSize(0, 0);
        tag->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }

    // لو الشاشة كان عليها layout قديم نشيله (العناصر نفسها بتفضل موجودة)
    if (layout())
        delete layout();

    auto *col1 = new QVBoxLayout;
    col1->setSpacing(10);
    col1->addWidget(ui->beforeImageLabel, 1);
    col1->addWidget(tagBefore, 0);

    auto *col2 = new QVBoxLayout;
    col2->setSpacing(10);
    col2->addWidget(ui->afterImageLabel, 1);
    col2->addWidget(tagAfter, 0);

    // عمود الأزرار: Load / Clear / Multi / القايمة / Delete Filter / Save (آخر واحد)
    auto *side = new QVBoxLayout;
    side->setSpacing(18);
    side->addStretch(1);
    for (QWidget *w : QList<QWidget *>{ui->loadButton, clearButton_, multiButton_,
                                       filterCombo_, ui->undoButton, ui->saveButton}) {
        w->setMinimumSize(200, 44);
        w->setMaximumSize(260, 44);
        side->addWidget(w);
    }
    side->addStretch(1);

    auto *content = new QHBoxLayout;
    content->setSpacing(28);
    content->addLayout(col1, 4);
    content->addLayout(col2, 4);
    content->addLayout(side, 1);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(30, 20, 30, 30);
    root->setSpacing(16);
    root->addWidget(title, 0, Qt::AlignHCenter);
    root->addLayout(content, 1);

    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(R"CSS(
        Widget {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                        stop:0 #141e30, stop:1 #243b55);
        }
        QLabel { color: #f1f5f9; font-size: 16px; }
        QLabel#appTitle {
            font-size: 34px; font-weight: bold; font-style: italic;
            color: #7cc4ff;
        }
        QLabel#tagText {
            color: #7cc4ff; font-size: 16px; font-weight: bold;
        }
        QLabel#imgFrame {
            background: rgba(255,255,255,0.06);
            border: 2px solid #4facfe;
            border-radius: 14px;
        }
        QPushButton {
            background: #4facfe; color: #0b1220; border: none;
            border-radius: 10px; padding: 8px 18px;
            font-size: 14px; font-weight: bold;
        }
        QPushButton:hover    { background: #7cc4ff; }
        QPushButton:pressed  { background: #2f8de0; }
        QPushButton:checked  { background: #34d399; color: #06241a; }
        QPushButton:disabled { background: #3a4a5e; color: #7b8794; }
        QComboBox {
            background: rgba(255,255,255,0.10); color: white;
            border: 1px solid #4facfe; border-radius: 10px; padding: 6px 12px;
        }
        QComboBox QAbstractItemView {
            background: #1b2a41; color: white;
            selection-background-color: #4facfe;
        }
    )CSS");

    resize(1200, 760);
}

void Widget::updateUiState()
{
    bool has = (image_ != nullptr);
    // القايمة والـ Multi شغالين دايماً، والباقي بيتقفل من غير صورة
    ui->saveButton->setEnabled(has);
    ui->undoButton->setEnabled(has && filtered_);   // Delete Filter
    clearButton_->setEnabled(has);

    QString title = kAppName;
    if (has) {
        title += " - " + (currentPath_.isEmpty() ? QString("Untitled")
                                                 : QFileInfo(currentPath_).fileName());
        if (modified_) title += " *";
    }
    setWindowTitle(title);
}

void Widget::showPreviews(bool rebuild)
{
    if (rebuild)
        afterPixmap_ = image_ ? toPixmap(*image_) : QPixmap();

    showOn(ui->beforeImageLabel, originalPixmap_);
    showOn(ui->afterImageLabel, afterPixmap_);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    showPreviews(false);   // بس نعيد scale، من غير ما نحول الصورة تاني
}

void Widget::applyFilter(const std::function<bool(Image &)> &filter)
{
    if (!image_ || !baseImage_) return;

    // الوضع العادي: الفلتر بيتطبق على الصورة الأصلية (بيمسح اللي قبله)
    // وضع الفلاتر المتعددة: الفلتر بيتطبق فوق اللي اتعمل
    auto work = std::make_unique<Image>(multiMode_ ? *image_ : *baseImage_);
    if (!filter(*work)) return;      // المستخدم لغى

    image_ = std::move(work);
    modified_ = true;
    filtered_ = true;
    showPreviews();
    updateUiState();
}

void Widget::onFilterChosen(int index)
{
    // نرجّع القايمة لأول عنصر عشان تقدر تختار نفس الفلتر تاني
    auto resetCombo = [this]() { filterCombo_->setCurrentIndex(0); };

    if (index <= 0) { resetCombo(); return; }

    if (!image_) {
        QMessageBox::information(this, kAppName, "حمّل صورة الأول عشان تطبّق عليها فلتر.");
        resetCombo();
        return;
    }

    const auto &list = filterList();
    size_t idx = static_cast<size_t>(index - 1);
    if (idx < list.size()) {
        const auto &item = list[idx];
        applyFilter([this, &item](Image &i) { return item.run(i, this); });
    }
    resetCombo();
}

void Widget::onMultiToggled(bool on)
{
    multiMode_ = on;
    multiButton_->setText(on ? "Multi Filters: ON" : "Multi Filters: OFF");
}

void Widget::onDeleteFilterClicked()
{
    if (!baseImage_) return;
    image_ = std::make_unique<Image>(*baseImage_);   // نرجع للصورة الأصلية
    modified_ = false;
    filtered_ = false;
    showPreviews();
    updateUiState();
}

void Widget::onClearImageClicked()
{
    image_.reset();
    baseImage_.reset();
    originalPixmap_ = QPixmap();
    afterPixmap_ = QPixmap();
    currentPath_.clear();
    modified_ = false;
    filtered_ = false;
    showPreviews(false);   // البيكسماب فاضية، فالإطارات هتتمسح
    updateUiState();
}

bool Widget::maybeSave()
{
    if (!image_ || !modified_) return true;

    auto ret = QMessageBox::question(this, "تعديلات غير محفوظة",
                                     "الصورة فيها تعديلات ما اتحفظتش.\nتحب تحفظها الأول؟",
                                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                     QMessageBox::Save);
    if (ret == QMessageBox::Cancel) return false;
    if (ret == QMessageBox::Save) {
        onSaveClicked();
        return !modified_;   // لو الحفظ اتلغى هيفضل modified
    }
    return true;
}

void Widget::onLoadClicked()
{
    // تحميل صورة جديدة بيمسح القديمة تلقائي (من غير سؤال)
    QString path = QFileDialog::getOpenFileName(this, "اختر صورة", QString(),
                                                "Images (*.png *.jpg *.jpeg *.bmp *.tga)");
    if (path.isEmpty()) return;

    try {
        auto newImage = std::make_unique<Image>();
        newImage->loadNewImage(path.toStdString());
        image_ = std::move(newImage);
        baseImage_ = std::make_unique<Image>(*image_);
        originalPixmap_ = toPixmap(*image_);
        currentPath_ = path;
        modified_ = false;
        filtered_ = false;
        showPreviews();
        updateUiState();
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error", QString("Couldn't load image:\n") + e.what());
    }
}

bool Widget::doSave(const QString &path)
{
    QFileInfo info(path);
    QString suffix = info.suffix().toLower();
    QStringList valid = {"png", "jpg", "jpeg", "bmp", "tga"};

    QString finalPath;
    if (!valid.contains(suffix))
        finalPath = info.absoluteFilePath() + ".png";
    else // Image_Class بيقبل الامتداد lowercase بس
        finalPath = info.absolutePath() + "/" + info.completeBaseName() + "." + suffix;

    try {
        image_->saveImage(finalPath.toStdString());
        currentPath_ = finalPath;
        modified_ = false;
        updateUiState();
        return true;
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error", QString("Error saving image:\n") + e.what());
        return false;
    }
}

void Widget::onSaveClicked()
{
    if (!image_) return;
    QString path = QFileDialog::getSaveFileName(this, "حفظ الصورة", currentPath_,
                                                "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp);;TGA (*.tga)");
    if (path.isEmpty()) return;
    doSave(path);
}

void Widget::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}