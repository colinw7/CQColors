#ifndef CQColorsEditCanvas_H
#define CQColorsEditCanvas_H

#include <QFrame>

class CQColorsPalette;
class QPainter;
class QLabel;

//! \brief Gradient Plot
class CQColorsEditCanvas : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool      showPoints   READ isShowPoints   WRITE setShowPoints  )
  Q_PROPERTY(bool      showLines    READ isShowLines    WRITE setShowLines   )
  Q_PROPERTY(bool      showGrayLine READ isShowGrayLine WRITE setShowGrayLine)
  Q_PROPERTY(bool      showBars     READ isShowBars     WRITE setShowBars    )
  Q_PROPERTY(bool      showColorBar READ isShowColorBar WRITE setShowColorBar)
  Q_PROPERTY(bool      gray         READ isGray         WRITE setGray        )
  Q_PROPERTY(ShowValue showValue    READ showValue      WRITE setShowValue   )
  Q_PROPERTY(QColor    nearestColor READ nearestColor   WRITE setNearestColor)

  Q_ENUMS(ShowValue)

 public:
  struct Margin {
    double left   = 0.1;
    double bottom = 0.1;
    double right  = 0.2;
    double top    = 0.1;
  };

  enum class ShowValue {
    RED,
    GREEN,
    BLUE,
    HUE,
    SATURATION,
    VALUE,
    GRAY
  };

 public:
  CQColorsEditCanvas(QWidget *parent, CQColorsPalette *palette=nullptr);
  CQColorsEditCanvas(CQColorsPalette *palette, QWidget *parent=nullptr);

 ~CQColorsEditCanvas();

  //---

  CQColorsPalette *palette() { return palette_; }
  void setPalette(CQColorsPalette *pal);

  bool isShowPoints  () const { return showPoints_  ; }
  bool isShowLines   () const { return showLines_   ; }
  bool isShowGrayLine() const { return showGrayLine_; }
  bool isShowBars    () const { return showBars_    ; }
  bool isShowColorBar() const { return showColorBar_; }
  bool isGray        () const { return gray_        ; }

  ShowValue showValue() const { return showValue_; }
  void setShowValue(const ShowValue &s);

  const QColor &nearestColor() const { return nearestColor_; }
  void setNearestColor(const QColor &v) { nearestColor_ = v; }

  //---

  void enterEvent(QEvent *) override;
  void leaveEvent(QEvent *) override;

  void mousePressEvent  (QMouseEvent *) override;
  void mouseMoveEvent   (QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;

  void contextMenuEvent(QContextMenuEvent *e) override;

  void paintEvent(QPaintEvent *) override;

  //---

  QSize sizeHint() const override;

  QSize minimumSizeHint() const override;

 private:
  struct NearestData {
    int    i = -1;
    double x = 0.0;
    double d = 0.0;
    int    c = 0;
    QColor color;
  };

 private:
  void init();

  void showTipText();
  void hideTipText();

  void drawAxis(QPainter *painter);

  void drawLine(QPainter *painter, double x1, double y1, double x2, double y2, const QPen &pen);

  void drawSymbol(QPainter *painter, double x, double y, const QPen &pen);

  void nearestDefinedColor(const QPointF &p, NearestData &nearestData);

  void moveNearestDefinedColor(const NearestData &nearestData, double dy);

  QPointF pixelToWindow(const QPoint &p);

  void windowToPixel(double wx, double wy, double &px, double &py) const;
  void pixelToWindow(double px, double py, double &wx, double &wy) const;

 public slots:
  void setShowPoints  (bool b);
  void setShowLines   (bool b);
  void setShowGrayLine(bool b);
  void setShowBars    (bool b);
  void setShowColorBar(bool b);
  void setGray        (bool b);

  void setShowHueValue();
  void setShowSaturationValue();
  void setShowValueValue();
  void setShowRedValue();
  void setShowGreenValue();
  void setShowBlueValue();
  void setShowGrayValue();

 private slots:
  void updateSlot();

 signals:
  void colorsChanged();

 private:
  struct MouseData {
    bool    pressed { false };
    QPointF pressPos;
    QPointF movePos;
  };

  CQColorsPalette* palette_       { nullptr };         //!< palette to edit
  Margin           margin_;                            //!< canvas margin
  QLabel*          tipText_       { nullptr };         //!< tip text widget
  MouseData        mouseData_;                         //!< mouse state data
  NearestData      nearestData_;                       //!< nearest data
  bool             showPoints_    { true };            //!< show points on plot
  bool             showLines_     { true };            //!< show lines on plot
  bool             showGrayLine_  { true };            //!< show gray line on plot
  bool             showBars_      { false };           //!< show bars on plot
  bool             showColorBar_  { true };            //!< show color bar
  bool             gray_          { false };           //!< is gray
  ShowValue        showValue_     { ShowValue::GRAY }; //!< show value gray
  QColor           nearestColor_  { Qt::cyan };        //!< nearest color
};

#endif
