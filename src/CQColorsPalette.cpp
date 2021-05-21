#include <CQColorsPalette.h>
#include <CCubeHelix.h>
#ifdef CQCOLORS_TCL
#include <CQTclUtil.h>
#endif
#include <CMathUtil.h>
#include <QLinearGradient>
#include <QPainter>

#include <iostream>

CQColorsPalette::
CQColorsPalette()
{
  init();
}

void
CQColorsPalette::
init()
{
  initFunctions();
}

#if 0
CQColorsPalette::
CQColorsPalette(const CQColorsPalette &palette) :
 QObject()
{
  *this = palette;
}
#endif

CQColorsPalette::
~CQColorsPalette()
{
  delete cubeHelix_;

#ifdef CQCOLORS_TCL
  delete qtcl_;
#endif
}

#if 0
CQColorsPalette &
CQColorsPalette::
operator=(const CQColorsPalette &palette)
{
  assign(palette);

  return *this;
}
#endif

void
CQColorsPalette::
assign(const CQColorsPalette &palette)
{
  // keep original name and desc

  // Color Calculation Type and Model Type
  colorType_  = palette.colorType_;
  colorModel_ = palette.colorModel_;

  // Color Model
  rModel_        = palette.rModel_;
  gModel_        = palette.gModel_;
  bModel_        = palette.bModel_;
  gray_          = palette.gray_;
  redNegative_   = palette.redNegative_;
  greenNegative_ = palette.greenNegative_;
  blueNegative_  = palette.blueNegative_;
  redMin_        = palette.redMin_;
  redMax_        = palette.redMax_;
  greenMin_      = palette.greenMin_;
  greenMax_      = palette.greenMax_;
  blueMin_       = palette.blueMin_;
  blueMax_       = palette.blueMax_;

  // Functions
  rf_ = palette.rf_;
  gf_ = palette.gf_;
  bf_ = palette.bf_;

  // CubeHelix
  if (palette.cubeHelix_) {
    cubeHelix_ = new CCubeHelix;

    *cubeHelix_ = *palette.cubeHelix_;
  }
  else {
    delete cubeHelix_;

    cubeHelix_ = nullptr;
  }

  cubeNegative_ = palette.cubeNegative_;

  //---

  // Defined
  definedColors_      = palette.definedColors_;
  definedValueColors_ = palette.definedValueColors_;

  definedMin_ = palette.definedMin_;
  definedMax_ = palette.definedMax_;

  definedDistinct_ = palette.definedDistinct_;
  definedInverted_ = palette.definedInverted_;

  // Misc
  defaultNumColors_ = palette.defaultNumColors_;

#if 0
  gamma_= palette.gamma_;
#endif

#ifdef CQCOLORS_TCL
  // Tcl
  delete qtcl_;

  qtcl_ = nullptr;
#endif

  //---

  init();

  //---

  emit colorsChanged();

  gradientImageDirty_ = true;
}

CQColorsPalette *
CQColorsPalette::
dup() const
{
  auto *palette = new CQColorsPalette;

  palette->assign(*this);

  return palette;
}

//---

#ifdef CQCOLORS_TCL
CQTcl *
CQColorsPalette::
qtcl() const
{
  if (! qtcl_) {
    auto *th = const_cast<CQColorsPalette *>(this);

    th->qtcl_ = new CQTcl;

    th->qtcl_->createVar("pi", M_PI);
  }

  return qtcl_;
}
#endif

//---

void
CQColorsPalette::
initFunctions()
{
  setRedFunction  ("$gray");
  setGreenFunction("$gray");
  setBlueFunction ("$gray");
}

//---

void
CQColorsPalette::
setRedFunction(const std::string &fn)
{
  rf_.fn = fn;
}

void
CQColorsPalette::
setGreenFunction(const std::string &fn)
{
  gf_.fn = fn;
}

void
CQColorsPalette::
setBlueFunction(const std::string &fn)
{
  bf_.fn = fn;
}

void
CQColorsPalette::
setFunctions(const std::string &rf, const std::string &gf, const std::string &bf)
{
  colorType_ = ColorType::FUNCTIONS;

  setRedFunction  (rf);
  setGreenFunction(gf);
  setBlueFunction (bf);
}

//---

void
CQColorsPalette::
setCubeHelix(double start, double cycles, double saturation)
{
  colorType_ = ColorType::CUBEHELIX;

  setCbStart     (start);
  setCbCycles    (cycles);
  setCbSaturation(saturation);
}

double
CQColorsPalette::
cbStart() const
{
  return cubeHelix()->start();
}

void
CQColorsPalette::
setCbStart(double r)
{
  cubeHelix()->setStart(r);
}

double
CQColorsPalette::
cbCycles() const
{
  return cubeHelix()->cycles();
}

void
CQColorsPalette::
setCbCycles(double r)
{
  cubeHelix()->setCycles(r);
}

double
CQColorsPalette::
cbSaturation() const
{
  return cubeHelix()->saturation();
}

void
CQColorsPalette::
setCbSaturation(double r)
{
  cubeHelix()->setSaturation(r);
}

bool
CQColorsPalette::
isCubeNegative() const
{
  return cubeNegative_;
}

void
CQColorsPalette::
setCubeNegative(bool b)
{
  cubeNegative_ = b;
}

CCubeHelix *
CQColorsPalette::
cubeHelix() const
{
  if (! cubeHelix_) {
    auto *th = const_cast<CQColorsPalette *>(this);

    th->cubeHelix_ = new CCubeHelix;
  }

  return cubeHelix_;
}

//---

void
CQColorsPalette::
setRgbModel(int r, int g, int b)
{
  colorType_ = ColorType::MODEL;

  rModel_ = r;
  gModel_ = g;
  bModel_ = b;
}

bool
CQColorsPalette::
isDefinedColor(double v) const
{
  return (definedValueColors_.find(v) != definedValueColors_.end());
}

void
CQColorsPalette::
addDefinedColor(double v, const QColor &c)
{
  assert(! isDefinedColor(v));

  definedColors_.push_back(DefinedColor(v, c));

  definedValueColors_[v] = c;

  definedMin_ = definedValueColors_. begin()->first;
  definedMax_ = definedValueColors_.rbegin()->first;
}

void
CQColorsPalette::
resetDefinedColors()
{
  definedColors_     .clear();
  definedValueColors_.clear();

  definedMin_ = 0.0;
  definedMax_ = 0.0;
}

void
CQColorsPalette::
setDefinedColor(int i, const QColor &c)
{
  assert(i >= 0 && i < numDefinedColors());

  DefinedColor &dc = definedColors_[i];

  dc.c = c;

  definedValueColors_[dc.v] = c;
}

void
CQColorsPalette::
setDefinedColors(const ColorMap &cmap)
{
  setColorType(ColorType::DEFINED);

  resetDefinedColors();

  for (const auto &c : cmap)
    addDefinedColor(c.first, c.second);

  emit colorsChanged();

  gradientImageDirty_ = true;
}

void
CQColorsPalette::
setDefinedColors(const DefinedColors &colors)
{
  setColorType(ColorType::DEFINED);

  resetDefinedColors();

  for (const auto &c : colors)
    addDefinedColor(c.v, c.c);

  emit colorsChanged();

  gradientImageDirty_ = true;
}

double
CQColorsPalette::
mapDefinedColorX(double x) const
{
  double d = definedMax_ - definedMin_;

  return (d > 0.0 ? (x - definedMin_)/d : x);
}

double
CQColorsPalette::
unmapDefinedColorX(double x) const
{
  double d = definedMax_ - definedMin_;

  return x*d + definedMin_;
}

void
CQColorsPalette::
setDistinct(bool b)
{
  definedDistinct_ = b;

  emit colorsChanged();

  gradientImageDirty_ = true;
}

void
CQColorsPalette::
setInverted(bool b)
{
  definedInverted_ = b;

  emit colorsChanged();

  gradientImageDirty_ = true;
}

//---

QColor
CQColorsPalette::
getColor(int i, int n, WrapMode wrapMode) const
{
  if      (colorType() == ColorType::DEFINED) {
    int nc = definedColors_.size();

    if (nc <= 0)
      return QColor();

    if (isInverted())
      i = n - 1 - i;

    if      (wrapMode == WrapMode::REPEAT) {
      while (i < 0)
        i += nc;

      while (i >= nc)
        i -= nc;
    }
    else if (wrapMode == WrapMode::REFLECT) {
      bool reflect = false;

      while (i < 0) {
        i += nc;

        reflect = ! reflect;
      }

      while (i >= nc) {
        i -= nc;

        reflect = ! reflect;
      }

      if (reflect)
        i = nc - 1 - i;
    }
    else {
      if (i < 0 || i >= nc)
        return QColor();
    }

    return definedColors_[i].c;
  }
  else {
    if (n < 0)
      n = defaultNumColors_; // TODO: default value

    if (n <= 0)
      return QColor();

    double r = (n > 1 ? 1.0*i/(n - 1) : 0.0);

    return getColor(r);
  }
}

QColor
CQColorsPalette::
getColor(double x, bool scale, bool invert) const
{
  if      (colorType() == ColorType::DEFINED) {
    if (definedColors_.empty()) {
      QColor c1(0,0,0);
      QColor c2(255,255,255);

      if      (colorModel() == ColorModel::RGB)
        return interpRGB(c1, c2, x);
      else if (colorModel() == ColorModel::HSV)
        return interpHSV(c1, c2, x);
      else
        return interpRGB(c1, c2, x);
    }

    if (scale)
      x = mapDefinedColorX(x);

    if (invert)
      x = 1.0 - x;

    if (isInverted())
      x = 1.0 - x;

    auto p = definedValueColors_.begin();

    auto x1 = mapDefinedColorX((*p).first);
    auto c1 = (*p).second;

    if (x <= x1) return c1;

    for (++p; p != definedValueColors_.end(); ++p) {
      auto x2 = mapDefinedColorX((*p).first);
      auto c2 = (*p).second;

      if (x <= x2) {
        double m = (x - x1)/(x2 - x1);

        if      (colorModel() == ColorModel::RGB)
          return interpRGB(c1, c2, m);
        else if (colorModel() == ColorModel::HSV)
          return interpHSV(c1, c2, m);
        else
          return interpRGB(c1, c2, m);
      }

      x1 = x2;
      c1 = c2;
    }

    return c1;
  }
  else if (colorType() == ColorType::MODEL) {
    if (isGray()) {
      double g = CMathUtil::clamp(x, 0.0, 1.0);

      if (isRedNegative() || isGreenNegative() || isBlueNegative())
        g = 1.0 - g;

      return QColor(255*g, 255*g, 255*g);
    }

    //---

    double x1 = CMathUtil::clamp(x, 0.0, 1.0);

    double r = CMathUtil::clamp(interpModel(redModel  (), x1), 0.0, 1.0);
    double g = CMathUtil::clamp(interpModel(greenModel(), x1), 0.0, 1.0);
    double b = CMathUtil::clamp(interpModel(blueModel (), x1), 0.0, 1.0);

    if (isRedNegative  ()) r = 1.0 - r;
    if (isGreenNegative()) g = 1.0 - g;
    if (isBlueNegative ()) b = 1.0 - b;

    r = CMathUtil::map(r, 0.0, 1.0, redMin  (), redMax  ());
    g = CMathUtil::map(g, 0.0, 1.0, greenMin(), greenMax());
    b = CMathUtil::map(b, 0.0, 1.0, blueMin (), blueMax ());

    QColor c;

    if      (colorModel() == ColorModel::RGB)
      c = QColor::fromRgbF(r, g, b);
    else if (colorModel() == ColorModel::HSV)
      c = QColor::fromHsvF(r, g, b);
    else
      c = QColor::fromRgbF(r, g, b);

    return c;
  }
  else if (colorType() == ColorType::FUNCTIONS) {
    double r = 0.0, g = 0.0, b = 0.0;

#ifdef CQCOLORS_TCL
    auto *qtcl = this->qtcl();

    qtcl->createVar("gray", x);

    QVariant res;
    bool     ok;

    if (qtcl->evalExpr(rf_.fn.c_str(), res))
      r = CQTclUtil::toReal(res, ok);

    if (qtcl->evalExpr(gf_.fn.c_str(), res))
      g = CQTclUtil::toReal(res, ok);

    if (qtcl->evalExpr(bf_.fn.c_str(), res))
      b = CQTclUtil::toReal(res, ok);
#endif

    //---

    QColor c;

    r = CMathUtil::clamp(r, 0.0, 1.0);
    g = CMathUtil::clamp(g, 0.0, 1.0);
    b = CMathUtil::clamp(b, 0.0, 1.0);

    if      (colorModel() == ColorModel::RGB)
      c = QColor::fromRgbF(r, g, b);
    else if (colorModel() == ColorModel::HSV)
      c = QColor::fromHsvF(r, g, b);
    else
      c = QColor::fromRgbF(r, g, b);

    return c;
  }
  else if (colorType() == ColorType::CUBEHELIX) {
    return QColor(cubeHelix()->interp(x, isCubeNegative()));
  }
  else {
    return QColor(0, 0, 0);
  }
}

double
CQColorsPalette::
interpModel(int ind, double x)
{
  if (ind < 0)
    return interpModel(-ind, 1 - x);

  switch (ind) {
    case  0: return 0;
    case  1: return 0.5;
    case  2: return 1;
    case  3: return x;
    case  4: return pow(x, 2);
    case  5: return pow(x, 3);
    case  6: return pow(x, 4);
    case  7: return sqrt(x);
    case  8: return sqrt(sqrt(x));
    case  9: return sin(CMathUtil::Deg2Rad(90*x));
    case 10: return cos(CMathUtil::Deg2Rad(90*x));
    case 11: return fabs(x - 0.5);
    case 12: return pow(2*x - 1, 2);
    case 13: return sin(CMathUtil::Deg2Rad(180*x));
    case 14: return fabs(cos(CMathUtil::Deg2Rad(180*x)));
    case 15: return sin(CMathUtil::Deg2Rad(360*x));
    case 16: return cos(CMathUtil::Deg2Rad(360*x));
    case 17: return fabs(sin(CMathUtil::Deg2Rad(360*x)));
    case 18: return fabs(cos(CMathUtil::Deg2Rad(360*x)));
    case 19: return fabs(sin(CMathUtil::Deg2Rad(720*x)));
    case 20: return fabs(cos(CMathUtil::Deg2Rad(720*x)));
    case 21: return 3*x;
    case 22: return 3*x - 1;
    case 23: return 3*x - 2;
    case 24: return fabs(3*x - 1);
    case 25: return fabs(3*x - 2);
    case 26: return (3*x - 1)/2;
    case 27: return (3*x - 2)/2;
    case 28: return fabs((3*x - 1)/2);
    case 29: return fabs((3*x - 2)/2);
    case 30: return x/0.32 - 0.78125;
    case 31: return 2*x - 0.84;
    case 32: {
      if      (x < 0.00) return 0;
      else if (x < 0.25) return 4*x;
      else if (x < 0.42) return 1;
      else if (x < 0.92) return -2*x + 1.84;
      else if (x < 1.00) return x/0.08 - 11.5;
      else               return 1;
    }
    case 33: return fabs(2*x - 0.5);
    case 34: return 2*x;
    case 35: return 2*x - 0.5;
    case 36: return 2*x - 1;
    default: return x;
  }
}

std::string
CQColorsPalette::
modelName(int ind)
{
  if (ind < 0)
    return "Inverted " + modelName(-ind);

  switch (ind) {
    case  0: return "0";
    case  1: return "0.5";
    case  2: return "1";
    case  3: return "x";
    case  4: return "x\u00B2";
    case  5: return "x\u00B3";
    case  6: return "x\u2074";
    case  7: return "\u221Ax";
    case  8: return "\u221A\u221Ax";
    case  9: return "sin(90*x)";
    case 10: return "cos(90*x)";
    case 11: return "\u007Cx - 0.5\u007C";
    case 12: return "(2*x - 1)\u00B2";
    case 13: return "sin(180*x)";
    case 14: return "\u007Ccos(180*x)\u007C";
    case 15: return "sin(360*x)";
    case 16: return "cos(360*x)";
    case 17: return "\u007Csin(360*x)\u007C";
    case 18: return "\u007Ccos(360*x)\u007C";
    case 19: return "\u007Csin(720*x)\u007C";
    case 20: return "\u007Ccos(720*x)\u007C";
    case 21: return "3*x";
    case 22: return "3*x - 1";
    case 23: return "3*x - 2";
    case 24: return "\u007C3*x - 1\u007C";
    case 25: return "\u007C3*x - 2\u007C";
    case 26: return "(3*x - 1)/2";
    case 27: return "(3*x - 2)/2";
    case 28: return "\u007C(3*x - 1)/2\u007C";
    case 29: return "\u007C(3*x - 2)/2\u007C";
    case 30: return "x/0.32 - 0.78125";
    case 31: return "2*x - 0.84";
    case 32: return "0,4*x,1,-2*x + 1.84,x/0.08 - 11.5,1";
    case 33: return "\u007C2*x - 0.5\u007C";
    case 34: return "2*x";
    case 35: return "2*x - 0.5";
    case 36: return "2*x - 1";
    default: return "x";
  }
}

namespace {

bool fileToLines(const QString &filename, QStringList &lines) {
  auto open = [&](FILE* &fp, const QString &filename) -> bool {
    fp = fopen(filename.toStdString().c_str(), "r");
    if (! fp) return false;

    return true;
  };

  auto readLine = [](FILE *fp, QString &line) {
    line = "";

    if (feof(fp)) return false;

    int c = fgetc(fp);

    if (c == EOF)
      return false;

    while (! feof(fp) && c != '\n') {
      line += c;

      c = fgetc(fp);
    }

    return true;
  };

  auto close = [](FILE* &fp) {
    if (fp)
      fclose(fp);

    fp = 0;
  };

  //---

  FILE *fp = nullptr;

  if (! open(fp, filename))
    return false;

  QString line;

  while (readLine(fp, line))
    lines.push_back(line);

  close(fp);

  //---

  return true;
}

}

bool
CQColorsPalette::
readFile(const std::string &filename)
{
  QStringList lines;

  if (! fileToLines(filename.c_str(), lines)) {
    std::cerr << "can't read file \"" << filename << "\"" << std::endl;
    return false;
  }

  if (! readFileLines(lines))
    return false;

  return true;
}

bool
CQColorsPalette::
readFileLines(const QStringList &lines)
{
  auto addWords = [](const QString &str, std::vector<QString> &words) {
    auto strs = str.split(" ", QString::SkipEmptyParts);

    for (int i = 0; i < strs.length(); ++i)
      words.push_back(strs[i]);
  };

  //---

  setColorType(ColorType::DEFINED);

  resetDefinedColors();

  int i = 0;

  for (const auto &line : lines) {
    if (line.length() == 0) continue;

    std::vector<QString> words;

    addWords(line, words);

    double x = i;

    int j = 0;

    if (words.size() >= 4) {
      bool ok;

      x = words[0].toDouble(&ok);

      ++j;
    }

    double r, g, b;

    if (words.size() >= 3) {
      bool ok;

      r = words[j + 0].toDouble(&ok);
      g = words[j + 1].toDouble(&ok);
      b = words[j + 2].toDouble(&ok);
    }
    else
      continue;

    addDefinedColor(x, QColor(255*r, 255*g, 255*b));

    ++i;
  }

  return true;
}

bool
CQColorsPalette::
saveFile(const std::string &filename)
{
  auto *fp = fopen(filename.c_str(), "w");
  if (! fp) return false;

  for (int i = 0; i < numDefinedColors(); ++i) {
    double x = definedColorValue(i);

    const auto &c = definedColor(i);

    fprintf(fp, "%lf %lf %lf %lf\n", x, c.redF(), c.greenF(), c.blueF());
  }

  fclose(fp);

  return true;
}

void
CQColorsPalette::
unset()
{
  colorType_  = ColorType::MODEL;
  colorModel_ = ColorModel::RGB;

  // Model
  rModel_        = 7;
  gModel_        = 5;
  bModel_        = 15;
  gray_          = false;
  redNegative_   = false;
  greenNegative_ = false;
  blueNegative_  = false;

  // Defined
  resetDefinedColors();

  // Functions
  initFunctions();

  // CubeHelix
  if (cubeHelix_)
    cubeHelix_->reset();

  // Gamma
#if 0
  gamma_ = 1.5;
#endif
}

QImage
CQColorsPalette::
getGradientImage(const QSize &size)
{
  if (gradientImage_.isNull() || gradientImage_.size() != size) {
    gradientImage_ = QImage(size, QImage::Format_ARGB32_Premultiplied);

    gradientImageDirty_ = true;
  }

  if (gradientImageDirty_) {
    gradientImage_.fill(QColor(0,0,0,0));

    int w = size.width ();
    int h = size.height();

    if (w > 1) {
      QPainter painter(&gradientImage_);

      for (int i = 0; i < w; ++i) {
        double x = i/(w - 1.0);

        auto c = getColor(x);

        QPen pen(c);

        painter.setPen(pen);

        painter.drawLine(i, 0, i, h - 1);
      }

      painter.end();
    }

    gradientImageDirty_ = false;
  }

  return gradientImage_;
}

void
CQColorsPalette::
setLinearGradient(QLinearGradient &lg, double a, double xmin, double xmax, bool enabled) const
{
  auto grayColor = [&](const QColor &c) {
    int g = qGray(c.red(), c.green(), c.blue());

    return QColor(g, g, g);
  };

  if (! enabled)
    a *= 0.8;

  int    n = 64;
  double d = (xmax - xmin)/(n - 1);

  double x = xmin;

  for (int i = 0; i < n; ++i) {
    auto c = getColor(x);

    if (! enabled)
      c = grayColor(c);

    c.setAlphaF(a);

    double x1 = 1.0*i/(n - 1);

    lg.setColorAt(x1, c);

    x += d;
  }
}

#if 0
void
CQColorsPalette::
show(std::ostream &os) const
{
  os << "palette is " << (isGray() ? "GRAY" : "COLOR") << std::endl;

  if (! isGray()) {
    if      (colorType() == ColorType::MODEL)
      os << "rgb color mapping by rgbformulae are " <<
            redModel() << "," << greenModel() << "," << blueModel() << std::endl;
    else if (colorType() == ColorType::DEFINED)
      os << "color mapping by defined gradient" << std::endl;
    else if (colorType() == ColorType::FUNCTIONS)
      os << "color mapping is done by user defined functions " <<
            "A-formula: " << redFunction() <<
            "B-formula: " << greenFunction() <<
            "C-formula: " << blueFunction() << std::endl;
    else if (colorType() == ColorType::CUBEHELIX)
      os << "Cubehelix color palette: start " << cubeHelix()->start() <<
            " cycles " << cubeHelix()->cycles() <<
            " saturation " << cubeHelix()->saturation() << std::endl;
  }

  if      (colorType() == ColorType::MODEL)
    os << "figure is " << ((isRedNegative() || isGreenNegative() || isBlueNegative()) ?
                            "NEGATIVE" : "POSITIVE") << std::endl;
  else if (colorType() == ColorType::CUBEHELIX)
    os << "figure is " << (isCubeNegative() ? "NEGATIVE" : "POSITIVE") << std::endl;

  os << "Color-Model: ";
  if      (colorModel() == ColorModel::RGB) os << "RGB";
  else if (colorModel() == ColorModel::HSV) os << "HSV";
  else if (colorModel() == ColorModel::CMY) os << "CMY";
  else if (colorModel() == ColorModel::YIQ) os << "YIQ";
  else if (colorModel() == ColorModel::XYZ) os << "XYZ";
  os << std::endl;

#if 0
  os << "gamma is " << gamma_ << std::endl;
#endif
}

void
CQColorsPalette::
showGradient(std::ostream &os) const
{
  if (colorType() != ColorType::DEFINED) {
    int i = 0;

    for (const auto &cc : colors()) {
      const auto &c = cc.second;

      os << i << ". gray=" << cc.first << ", ";

      if      (c.spec() == QColor::Rgb) {
        os << "(r,g,b)=(" << c.redF() << "," << c.greenF() << "," << c.blueF() <<
              "), " << c.name().toStdString() << " = " <<
              c.red() << " " << c.green() << " " << c.blue() << std::endl;
      }
      else if (c.spec() == QColor::Hsv) {
        // TODO
        os << "(h,s,v)=(...)";
      }

      ++i;
    }
  }
  else
    os << "color mapping *not* done by defined gradient." << std::endl;
}

void
CQColorsPalette::
showRGBFormulae(std::ostream &os) const
{
  os << "set palette rgbformulae " <<
        redModel() << "," << greenModel() << "," << blueModel() << std::endl;
}

void
CQColorsPalette::
showPaletteValues(std::ostream &os, int n, bool is_float, bool is_int)
{
  if (n <= 1) n = 128;

  os << "Color palette with " << n << " discrete colors." << std::endl;

  double x = 0.0;
  double d = 1.0/(n - 1);

  for (int i = 0; i < n; ++i, x += d) {
    auto c = getColor(x);

    os << "  ";

    if      (c.spec() == QColor::Rgb) {
      if      (is_float) {
        os << c.redF() << " " << c.greenF() << " " << c.blueF() << std::endl;
      }
      else if (is_int) {
        os << c.red() << " " << c.green() << " " << c.blue() << std::endl;
      }
      else {
        os << i << ". gray=" << x << ", (r,g,b)=(" <<
              c.redF() << "," << c.greenF() << "," << c.blueF() <<
              "), " << c.name().toStdString() << " = " <<
              c.red() << " " << c.green() << " " << c.blue() << std::endl;
      }
    }
    else if (c.spec() == QColor::Hsv) {
      // TODO
    }
  }
}
#endif
