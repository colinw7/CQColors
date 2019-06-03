#include <CQColorsEditCanvas.h>
#include <CQColors.h>
#include <CQColorsPalette.h>
#include <CQUtil.h>

#include <QLabel>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QPainter>

namespace Util {
  inline double rgbToGray(double r, double g, double b) {
    return r*0.3 + g*0.59 + b*0.11;
  }

  QColor bwColor(const QColor &c) {
    int g = qGray(c.red(), c.green(), c.blue());

    return (g > 128 ? QColor(0,0,0) : QColor(255, 255, 255));
  }
}

//------

CQColorsEditCanvas::
CQColorsEditCanvas(QWidget *parent, CQColorsPalette *palette) :
 QFrame(parent), palette_(palette)
{
  init();
}

CQColorsEditCanvas::
CQColorsEditCanvas(CQColorsPalette *palette, QWidget *parent) :
 QFrame(parent), palette_(palette)
{
  init();
}

CQColorsEditCanvas::
~CQColorsEditCanvas()
{
}

void
CQColorsEditCanvas::
init()
{
  setObjectName("palette");

  setMouseTracking(true);

  tipText_ = CQUtil::makeLabelWidget<QLabel>(this, "", "tipText");

  tipText_->setAutoFillBackground(true);
}

void
CQColorsEditCanvas::
setPalette(CQColorsPalette *palette)
{
  if (palette_)
    disconnect(palette_, SIGNAL(colorsChanged()), this, SLOT(updateSlot()));

  palette_ = palette;

  if (palette_)
    connect(palette_, SIGNAL(colorsChanged()), this, SLOT(updateSlot()));

  update();
}

void
CQColorsEditCanvas::
enterEvent(QEvent *)
{
}

void
CQColorsEditCanvas::
leaveEvent(QEvent *)
{
  hideTipText();
}

void
CQColorsEditCanvas::
mousePressEvent(QMouseEvent *me)
{
  CQColorsPalette *pal = this->palette();

  if (! pal)
    return;

  if (me->button() == Qt::LeftButton) {
    mouseData_.pressed  = true;
    mouseData_.pressPos = pixelToWindow(me->pos());
    mouseData_.movePos  = mouseData_.pressPos;

    if (pal->colorType() == CQColorsPalette::ColorType::DEFINED) {
      NearestData nearestData;

      nearestDefinedColor(mouseData_.movePos, nearestData);

      if (nearestData.i != nearestData_.i || nearestData.c != nearestData_.c) {
        nearestData_ = nearestData;

        update();
      }
    }
  }
}

void
CQColorsEditCanvas::
mouseMoveEvent(QMouseEvent *me)
{
  CQColorsPalette *pal = this->palette();

  if (! pal)
    return;

  mouseData_.movePos = pixelToWindow(me->pos());

  //---

  if (mouseData_.movePos.x() >= 0.0 && mouseData_.movePos.x() <= 1.0) {
    QColor bg = pal->getColor(mouseData_.movePos.x());
    QColor fg = Util::bwColor(bg);

    tipText_->setText(QString("%1,%2").arg(mouseData_.movePos.x()).arg(mouseData_.movePos.y()));

    QPalette palette = tipText_->palette();

    palette.setColor(tipText_->backgroundRole(), bg);
    palette.setColor(tipText_->foregroundRole(), fg);

    tipText_->setPalette(palette);

    showTipText();
  }
  else {
    hideTipText();
  }

  //---

  if (pal->colorType() == CQColorsPalette::ColorType::DEFINED) {
    if (mouseData_.pressed) {
      double dy = mouseData_.movePos.y() - mouseData_.pressPos.y();

      moveNearestDefinedColor(nearestData_, dy);

      update();
    }
    else {
      NearestData nearestData;

      nearestDefinedColor(mouseData_.movePos, nearestData);

      if (nearestData.i != nearestData_.i || nearestData.c != nearestData_.c) {
        nearestData_ = nearestData;

        update();
      }
    }
  }
}

void
CQColorsEditCanvas::
mouseReleaseEvent(QMouseEvent *me)
{
  if (me->button() == Qt::LeftButton) {
    mouseData_.pressed = false;
  }
}

void
CQColorsEditCanvas::
contextMenuEvent(QContextMenuEvent *e)
{
  QMenu *menu = CQUtil::makeWidget<QMenu>("menu");

  //---

  auto addSubMenu = [](QMenu *menu, const QString &name) {
    QMenu *subMenu = new QMenu(name, menu);

    menu->addMenu(subMenu);

    return subMenu;
  };

  auto addCheckAction = [&](QMenu *menu, const QString &name, bool checked, const char *slotName) {
    QAction *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(checked);

    connect(action, SIGNAL(triggered(bool)), this, slotName);

    menu->addAction(action);

    return action;
  };

  //---

  addCheckAction(menu, "Lines" , isShowLines (), SLOT(setShowLines (bool)));
  addCheckAction(menu, "Bars"  , isShowBars  (), SLOT(setShowBars  (bool)));
  addCheckAction(menu, "Points", isShowPoints(), SLOT(setShowPoints(bool)));

  //---

  addCheckAction(menu, "Color Bar", isShowColorBar(), SLOT(setShowColorBar(bool)));

  //---

  CQColorsPalette *pal = this->palette();

  if (pal) {
    QMenu *valueMenu = addSubMenu(menu, "Bar Value");

    if (pal->colorModel() == CQColorsPalette::ColorModel::HSV) {
      addCheckAction(valueMenu, "Hue"       , showValue() == ShowValue::HUE,
                     SLOT(setShowHueValue(bool)));
      addCheckAction(valueMenu, "Saturation", showValue() == ShowValue::SATURATION,
                     SLOT(setShowSaturationValue(bool)));
      addCheckAction(valueMenu, "Value"     , showValue() == ShowValue::VALUE,
                     SLOT(setShowValueValue(bool)));
    }
    else {
      addCheckAction(valueMenu, "Red"  , showValue() == ShowValue::RED,
                     SLOT(setShowRedValue(bool)));
      addCheckAction(valueMenu, "Green", showValue() == ShowValue::GREEN,
                     SLOT(setShowGreenValue(bool)));
      addCheckAction(valueMenu, "Blue" , showValue() == ShowValue::BLUE,
                     SLOT(setShowBlueValue(bool)));
    }

    addCheckAction(valueMenu, "Gray", showValue() == ShowValue::GRAY,
                   SLOT(setShowGrayValue(bool)));
  }

  //---

  (void) menu->exec(e->globalPos());

  delete menu;
}

void
CQColorsEditCanvas::
setShowLines(bool b)
{
  showLines_ = b;

  update();
}

void
CQColorsEditCanvas::
setShowBars(bool b)
{
  showBars_ = b;

  update();
}

void
CQColorsEditCanvas::
setShowPoints(bool b)
{
  showPoints_ = b;

  update();
}

void
CQColorsEditCanvas::
setShowColorBar(bool b)
{
  showColorBar_ = b;

  update();
}

void
CQColorsEditCanvas::
setGray(bool b)
{
  gray_ = b;

  update();
}

void
CQColorsEditCanvas::
setShowHueValue(bool b)
{
  showValue_ = (b ? ShowValue::HUE : ShowValue::GRAY);

  update();
}

void
CQColorsEditCanvas::
setShowSaturationValue(bool b)
{
  showValue_ = (b ? ShowValue::SATURATION : ShowValue::GRAY);

  update();
}

void
CQColorsEditCanvas::
setShowValueValue(bool b)
{
  showValue_ = (b ? ShowValue::VALUE : ShowValue::GRAY);

  update();
}

void
CQColorsEditCanvas::
setShowRedValue(bool b)
{
  showValue_ = (b ? ShowValue::RED : ShowValue::GRAY);

  update();
}

void
CQColorsEditCanvas::
setShowGreenValue(bool b)
{
  showValue_ = (b ? ShowValue::GREEN : ShowValue::GRAY);

  update();
}

void
CQColorsEditCanvas::
setShowBlueValue(bool b)
{
  showValue_ = (b ? ShowValue::BLUE : ShowValue::GRAY);

  update();
}

void
CQColorsEditCanvas::
setShowGrayValue(bool b)
{
  showValue_ = (b ? ShowValue::GRAY : ShowValue::GRAY);

  update();
}

void
CQColorsEditCanvas::
nearestDefinedColor(const QPointF &p, NearestData &nearestData)
{
  double mx = p.x();
  double my = p.y();

  nearestData.i     = -1;
  nearestData.d     = 0.0;
  nearestData.c     = 0;
  nearestData.color = QColor();

  CQColorsPalette *pal = this->palette();

  if (! pal)
    return;

  int i = 0;

  for (const auto &c : pal->definedValueColors()) {
    double x = pal->mapDefinedColorX(c.first);

    const QColor &c1 = c.second;

    if (! isGray()) {
      double y[3];

      if (pal->colorModel() == CQColorsPalette::ColorModel::HSV) {
        y[0] = c1.hueF       ();
        y[1] = c1.saturationF();
        y[2] = c1.valueF     ();
      }
      else {
        y[0] = c1.redF  ();
        y[1] = c1.greenF();
        y[2] = c1.blueF ();
      }

      for (int j = 0; j < 3; ++j) {
        double d = std::hypot(mx - x, my - y[j]);

        if (nearestData.i < 0 || d < nearestData.d) {
          nearestData.i     = i;
          nearestData.x     = x;
          nearestData.d     = d;
          nearestData.c     = j;
          nearestData.color = c1;
        }
      }
    }
    else {
      double g = Util::rgbToGray(c1.redF(), c1.greenF(), c1.blueF());

      double d = std::hypot(mx - x, my - g);

      if (nearestData.i < 0 || d < nearestData.d) {
        nearestData.i     = i;
        nearestData.x     = x;
        nearestData.d     = d;
        nearestData.c     = 0;
        nearestData.color = c1;
      }
    }

    ++i;
  }
}

void
CQColorsEditCanvas::
moveNearestDefinedColor(const NearestData &nearestData, double dy)
{
  CQColorsPalette *pal = this->palette();

  if (! pal)
    return;

  QColor newColor = nearestData.color;

  if (pal->colorModel() == CQColorsPalette::ColorModel::HSV) {
    double h = newColor.hueF       ();
    double s = newColor.saturationF();
    double v = newColor.valueF     ();

    if (! isGray()) {
      if      (nearestData.c == 0) h += dy;
      else if (nearestData.c == 1) s += dy;
      else if (nearestData.c == 2) v += dy;
    }
    else {
      h += dy;
      s += dy;
      v += dy;
    }

    h = CMathUtil::clamp(h, 0.0, 1.0);
    s = CMathUtil::clamp(s, 0.0, 1.0);
    v = CMathUtil::clamp(v, 0.0, 1.0);

    newColor.setHsvF(h, s, v);
  }
  else {
    double r = newColor.redF  ();
    double g = newColor.greenF();
    double b = newColor.blueF ();

    if (! isGray()) {
      if      (nearestData.c == 0) r += dy;
      else if (nearestData.c == 1) g += dy;
      else if (nearestData.c == 2) b += dy;
    }
    else {
      r += dy;
      g += dy;
      b += dy;
    }

    r = CMathUtil::clamp(r, 0.0, 1.0);
    g = CMathUtil::clamp(g, 0.0, 1.0);
    b = CMathUtil::clamp(b, 0.0, 1.0);

    newColor.setRgbF(r, g, b);
  }

  //--

  pal->setDefinedColor(nearestData.i, newColor);

  emit colorsChanged();
}

void
CQColorsEditCanvas::
showTipText()
{
  tipText_->setVisible(true);

  tipText_->resize(tipText_->sizeHint());

  tipText_->move(width() - tipText_->width(), height() - tipText_->height());

  tipText_->raise();
}

void
CQColorsEditCanvas::
hideTipText()
{
  tipText_->setVisible(false);
}

void
CQColorsEditCanvas::
updateSlot()
{
  update();
}

void
CQColorsEditCanvas::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  //---

  // draw background
  painter.fillRect(rect(), Qt::white);

  //---

  // draw axes
  drawAxis(&painter);

  //---

  CQColorsPalette *pal = this->palette();

  if (! pal)
    return;

  //---

  double px1, py1, px2, py2;

  windowToPixel(0.0, 0.0, px1, py1);
  windowToPixel(1.0, 1.0, px2, py2);

  //---

  // set pens
  QPen redPen  (Qt::red  ); redPen   .setWidth(0);
  QPen greenPen(Qt::green); greenPen .setWidth(0);
  QPen bluePen (Qt::blue ); bluePen  .setWidth(0);
  QPen blackPen(Qt::black); blackPen .setWidth(0);

  QPen nearestPen(nearestColor()); nearestPen.setWidth(2);

  //---

  // draw bars
  if (isShowBars()) {
    using XValues = std::vector<double>;

    int nx = pal->numDefinedColors();

    XValues xvalues;

    xvalues.resize(nx);

    for (int i = 0; i < nx; ++i) {
      double x = pal->mapDefinedColorX(pal->definedColorValue(i));

      xvalues[i] = x;
    }

    double dxs = 1.0;
    double dxe = 1.0;

    if (nx > 1) {
      dxs = xvalues[2] - xvalues[1];
      dxe = xvalues[nx - 1] - xvalues[nx - 2];
    }

    //---

    for (int i = 0; i < nx; ++i) {
      double xm = xvalues[i];

      double x1, x2;

      if (i == 0)
        x1 = xm - dxs/2;
      else
        x1 = (xvalues[i - 1] + xm)/2.0;

      if (i == nx - 1)
        x2 = xm + dxe/2;
      else
        x2 = (xm + xvalues[i + 1])/2.0;

      //---

      QColor c = pal->definedColor(i);

      double px1, px2, py1;

      windowToPixel(x1, 0, px1, py1);
      windowToPixel(x2, 0, px2, py1);

      double r2 = 0.0, g2 = 0.0, b2 = 0.0, m2 = 0.0;

      if (pal->colorModel() == CQColorsPalette::ColorModel::HSV) {
        r2 = c.hueF       ();
        g2 = c.saturationF();
        b2 = c.valueF     ();
        m2 = c.valueF     ();
      }
      else {
        r2 = c.redF  ();
        g2 = c.greenF();
        b2 = c.blueF ();
        m2 = Util::rgbToGray(r2, g2, b2);
      }

      double px, py2;

      if      (showValue() == ShowValue::RED || showValue() == ShowValue::HUE)
        windowToPixel(0, r2, px, py2);
      else if (showValue() == ShowValue::GREEN || showValue() == ShowValue::SATURATION)
        windowToPixel(0, g2, px, py2);
      else if (showValue() == ShowValue::BLUE || showValue() == ShowValue::VALUE)
        windowToPixel(0, b2, px, py2);
      else
        windowToPixel(0, m2, px, py2);

      QRectF r(px1, py1, px2 - px1, py2 - py1);

      painter.fillRect(r, c);
    }
  }

  //---

  // draw lines
  if (isShowLines()) {
    QPainterPath redPath, greenPath, bluePath, blackPath;

    bool   first = true;
  //double r1 = 0.0, g1 = 0.0, b1 = 0.0, m1 = 0.0, x1 = 0.0;

    // get rgb (red, green, blue, gray), or hsv (hue, saturation, value, gray) paths
    for (double x = px1; x <= px2; x += 1.0) {
      double wx, wy;

      pixelToWindow(x, 0, wx, wy);

      QColor c = pal->getColor(wx);

      double x2 = wx;

      double r2 = 0.0, g2 = 0.0, b2 = 0.0, m2 = 0.0;

      if (pal->colorModel() == CQColorsPalette::ColorModel::HSV) {
        r2 = c.hueF       ();
        g2 = c.saturationF();
        b2 = c.valueF     ();
        m2 = c.valueF     ();
      }
      else {
        r2 = c.redF  ();
        g2 = c.greenF();
        b2 = c.blueF ();
        m2 = Util::rgbToGray(r2, g2, b2);
      }

      double px, py;

      if (first) {
        if (! isGray()) {
          windowToPixel(x2, r2, px, py); redPath  .moveTo(px, py);
          windowToPixel(x2, g2, px, py); greenPath.moveTo(px, py);
          windowToPixel(x2, b2, px, py); bluePath .moveTo(px, py);
        }

        windowToPixel(x2, m2, px, py); blackPath.moveTo(px, py);
      }
      else {
        if (! isGray()) {
          windowToPixel(x2, r2, px, py); redPath  .lineTo(px, py);
          windowToPixel(x2, g2, px, py); greenPath.lineTo(px, py);
          windowToPixel(x2, b2, px, py); bluePath .lineTo(px, py);
        }

        windowToPixel(x2, m2, px, py); blackPath.lineTo(px, py);
      }

  //  x1 = x2; r1 = r2; g1 = g2; b1 = b2; m1 = m2;

      first = false;
    }

    // draw paths
    if (! isGray()) {
      painter.strokePath(redPath  , redPen  );
      painter.strokePath(greenPath, greenPen);
      painter.strokePath(bluePath , bluePen );
    }

    painter.strokePath(blackPath, blackPen);
  }

  //---

  // draw color bar
  if (isShowColorBar()) {
    double xp1 = 1.05;
    double xp2 = 1.15;

    double pxp1, pxp2;

    windowToPixel(xp1, 0.0, pxp1, py1);
    windowToPixel(xp2, 1.0, pxp2, py2);

    // draw gradient
    if (! pal->isDistinct()) {
      for (double y = py2; y <= py1; y += 1.0) {
        double wx, wy;

        pixelToWindow(0, y, wx, wy);

        QColor c = pal->getColor(wy);

        QPen pen(c); pen.setWidth(0);

        painter.setPen(pen);

        painter.drawLine(QPointF(pxp1, y), QPointF(pxp2, y));
      }
    }
    else {
      int nc = pal->numDefinedColors();

      double dy = (nc > 0 ? (py1 - py2)/nc : 0.0);

      double y = py1;

      for (int i = 0; i < nc; ++i, y -= dy) {
        QColor c = pal->definedColor(i);

        QBrush brush(c);

        painter.setBrush(brush);

        painter.fillRect(QRectF(pxp1, y - dy, pxp2 - pxp1, dy), brush);
      }
    }

    // draw border
    painter.setPen(blackPen);

    painter.drawLine(QPointF(pxp1, py1), QPointF(pxp2, py1));
    painter.drawLine(QPointF(pxp2, py1), QPointF(pxp2, py2));
    painter.drawLine(QPointF(pxp2, py2), QPointF(pxp1, py2));
    painter.drawLine(QPointF(pxp1, py2), QPointF(pxp1, py1));

    // draw nearest
    if (nearestData_.i >= 0) {
      double px, py;

      windowToPixel(0.0, nearestData_.x, px, py);

      painter.setPen(nearestPen);

      painter.drawLine(QPointF(pxp1, py), QPointF(pxp2, py));
    }
  }

  //---

  // draw color points
  if (isShowPoints()) {
    if (pal->colorType() == CQColorsPalette::ColorType::DEFINED) {
      int i = 0;

      for (const auto &c : pal->definedValueColors()) {
        double x  = pal->mapDefinedColorX(c.first);
        QColor c1 = c.second;

        if (pal->colorModel() == CQColorsPalette::ColorModel::HSV) {
          drawSymbol(&painter, x, c1.hueF       (),
                     (nearestData_.i == i && nearestData_.c == 0 ? nearestPen : redPen  ));
          drawSymbol(&painter, x, c1.saturationF(),
                     (nearestData_.i == i && nearestData_.c == 1 ? nearestPen : greenPen));
          drawSymbol(&painter, x, c1.valueF     (),
                     (nearestData_.i == i && nearestData_.c == 2 ? nearestPen : bluePen ));
        }
        else {
          drawSymbol(&painter, x, c1.redF  (),
                     (nearestData_.i == i && nearestData_.c == 0 ? nearestPen : redPen  ));
          drawSymbol(&painter, x, c1.greenF(),
                     (nearestData_.i == i && nearestData_.c == 1 ? nearestPen : greenPen));
          drawSymbol(&painter, x, c1.blueF (),
                     (nearestData_.i == i && nearestData_.c == 2 ? nearestPen : bluePen ));
        }

        ++i;
      }
    }
  }
}

void
CQColorsEditCanvas::
drawAxis(QPainter *painter)
{
  QPen blackPen(Qt::black); blackPen.setWidth(0);

  double px1, py1, px2, py2;

  windowToPixel(0.0, 0.0, px1, py1);
  windowToPixel(1.0, 1.0, px2, py2);

  drawLine(painter, 0, 0, 1, 0, blackPen);
  drawLine(painter, 0, 0, 0, 1, blackPen);

  painter->setPen(blackPen);

  painter->drawLine(QPointF(px1, py1), QPointF(px1, py1 + 4));
  painter->drawLine(QPointF(px2, py1), QPointF(px2, py1 + 4));

  painter->drawLine(QPointF(px1, py1), QPointF(px1 - 4, py1));
  painter->drawLine(QPointF(px1, py2), QPointF(px1 - 4, py2));

  QFontMetricsF fm(font());

  double tw  = fm.width("X.X");
  double dty = (fm.ascent() - fm.descent())/2;

  painter->drawText(QPointF(px1 - tw - 4, py1 + dty), "0.0");
  painter->drawText(QPointF(px1 - tw - 4, py2 + dty), "1.0");
  painter->drawText(QPointF(px2 - tw/2, py1 + fm.height()), "1.0");

  painter->drawText(QPointF((px1 + px2 - tw)/2, py1 + fm.height()), "X");

  //---

  CQColorsPalette *pal = this->palette();

  QString yLabel;

  if (pal && pal->colorModel() == CQColorsPalette::ColorModel::HSV)
    yLabel = "HSV Value";
  else
    yLabel = "RGB Value";

  double tw1 = fm.width(yLabel);

  painter->save();

  QTransform t = painter->transform();

  t.translate(px1 - 8, (py1 + py2 + tw1)/2);
  t.rotate(-90);

  painter->setTransform(t);

  if (pal && pal->colorModel() == CQColorsPalette::ColorModel::HSV)
    painter->drawText(QPointF(0, 0), "HSV Value");
  else
    painter->drawText(QPointF(0, 0), "RGB Value");

  painter->restore();
}

void
CQColorsEditCanvas::
drawLine(QPainter *painter, double x1, double y1, double x2, double y2, const QPen &pen)
{
  painter->setPen(pen);

  double px1, py1, px2, py2;

  windowToPixel(x1, y1, px1, py1);
  windowToPixel(x2, y2, px2, py2);

  painter->drawLine(QPointF(px1, py1), QPointF(px2, py2));
}

void
CQColorsEditCanvas::
drawSymbol(QPainter *painter, double x, double y, const QPen &pen)
{
  painter->setPen(pen);

  double px, py;

  windowToPixel(x, y, px, py);

  painter->drawLine(QPointF(px - 4, py), QPointF(px + 4, py));
  painter->drawLine(QPointF(px, py - 4), QPointF(px, py + 4));
}

void
CQColorsEditCanvas::
windowToPixel(double wx, double wy, double &px, double &py) const
{
  px = CMathUtil::map(wx, -margin_.left  , 1 + margin_.right, 0, width () - 1);
  py = CMathUtil::map(wy, -margin_.bottom, 1 + margin_.top  , height() - 1, 0);
}

QPointF
CQColorsEditCanvas::
pixelToWindow(const QPoint &p)
{
  double wx, wy;

  pixelToWindow(p.x(), p.y(), wx, wy);

  return QPointF(wx, wy);
}

void
CQColorsEditCanvas::
pixelToWindow(double px, double py, double &wx, double &wy) const
{
  wx = CMathUtil::map(px, 0, width () - 1, -margin_.left  , 1 + margin_.right);
  wy = CMathUtil::map(py, height() - 1, 0, -margin_.bottom, 1 + margin_.top  );
}

QSize
CQColorsEditCanvas::
sizeHint() const
{
  return QSize(600, 600);
}
