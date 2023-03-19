#ifndef CQColorsEditDefinedColors_H
#define CQColorsEditDefinedColors_H

#include <CQColorsPalette.h>
#include <QTableWidget>

//! \brief gradient palette defined colors table
class CQColorsEditDefinedColors : public QTableWidget {
  Q_OBJECT

 public:
  struct RealColor {
    RealColor(double r1, const QColor &c1) :
     r(r1), c(c1) {
    }

    double r { 0.0 };
    QColor c { 0, 0, 0 };
  };

  using RealColors = std::vector<RealColor>;

 public:
  CQColorsEditDefinedColors(QWidget *parent=nullptr);

  void updateColors(CQColorsPalette *palette);

  size_t numRealColors() const { return realColors_.size(); }

  const RealColor &realColor(int i) const;

  void setRealColor(int i, const RealColor &c);

 signals:
  void colorsChanged();

 private:
  RealColors realColors_;
};

#endif
