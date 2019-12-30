#ifndef CQColorsPalette_H
#define CQColorsPalette_H

#include <QObject>
#include <QColor>
#include <QStringList>
#include <QImage>

#include <string>
#include <map>
#include <memory>
#include <cmath>
#include <cassert>

class CCubeHelix;
class CQTcl;
class QLinearGradient;

//---

//! \brief gradient palette
class CQColorsPalette : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString    name       READ name       WRITE setName      )
  Q_PROPERTY(QString    desc       READ desc       WRITE setDesc      )
  Q_PROPERTY(ColorType  colorType  READ colorType  WRITE setColorType )
  Q_PROPERTY(ColorModel colorModel READ colorModel WRITE setColorModel)

  Q_ENUMS(ColorType)
  Q_ENUMS(ColorModel)

 public:
  enum class ColorType {
    NONE,
    MODEL,
    DEFINED,
    FUNCTIONS,
    CUBEHELIX
  };

  enum class ColorModel {
    NONE,
    RGB,
    HSV,
    CMY,
    YIQ,
    XYZ,
  };

  enum class WrapMode {
    NONE,
    REPEAT,
    REFLECT
  };

  using ColorMap = std::map<double,QColor>;

  struct DefinedColor {
    double v { -1.0 };
    QColor c;

    DefinedColor(double v, const QColor &c) :
     v(v), c(c) {
    }
  };

  using DefinedColors = std::vector<DefinedColor>;

 public:
  static ColorType stringToColorType(const QString &str) {
    if      (str == "model"    ) return ColorType::MODEL;
    else if (str == "defined"  ) return ColorType::DEFINED;
    else if (str == "functions") return ColorType::FUNCTIONS;
    else if (str == "cubehelix") return ColorType::CUBEHELIX;

    return ColorType::NONE;
  }

  static QString colorTypeToString(ColorType type) {
    switch (type) {
      case ColorType::MODEL    : return "model";
      case ColorType::DEFINED  : return "defined";
      case ColorType::FUNCTIONS: return "functions";
      case ColorType::CUBEHELIX: return "cubehelix";
      default                  : return "";
    }
  }

  static ColorModel stringToColorModel(const QString &str) {
    if      (str == "rgb") return ColorModel::RGB;
    else if (str == "hsv") return ColorModel::HSV;
    else if (str == "cmy") return ColorModel::CMY;
    else if (str == "yiq") return ColorModel::YIQ;
    else if (str == "xyz") return ColorModel::XYZ;

    return ColorModel::NONE;
  }

  static QString colorModelToString(ColorModel model) {
    switch (model) {
      case ColorModel::RGB: return "rgb";
      case ColorModel::HSV: return "hsv";
      case ColorModel::CMY: return "cmy";
      case ColorModel::YIQ: return "yiq";
      case ColorModel::XYZ: return "xyz";
      default             : return "";
    }
  }

 public:
  CQColorsPalette();

  //CQColorsPalette(const CQColorsPalette &palette);

  virtual ~CQColorsPalette();

  //CQColorsPalette &operator=(const CQColorsPalette &palette);

  //---

  CQColorsPalette *dup() const;

  void assign(const CQColorsPalette &palette);

  //---

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  const QString &desc() const { return desc_; }
  void setDesc(const QString &s) { desc_ = s; }

  //---

  //! get/set color calculation type
  ColorType colorType() const { return colorType_; }
  void setColorType(ColorType t) { colorType_ = t; }

  //! get/set color model
  ColorModel colorModel() const { return colorModel_; }
  void setColorModel(ColorModel m) { colorModel_ = m; }

  //---

#if 0
  //! get/set gamma correction : NOT USED yet
  double gamma() const { return gamma_; }
  void setGamma(double g) { gamma_ = g; }
#endif

  //---

  // model
  void setRgbModel(int r, int g, int b);

  int redModel() const { return rModel_; }
  void setRedModel(int r) { rModel_ = r; }

  int greenModel() const { return gModel_; }
  void setGreenModel(int r) { gModel_ = r; }

  int blueModel() const { return bModel_; }
  void setBlueModel(int r) { bModel_ = r; }

  bool isGray() const { return gray_; }
  void setGray(bool b) { gray_ = b; }

  bool isRedNegative() const { return redNegative_; }
  void setRedNegative(bool b) { redNegative_ = b; }

  bool isGreenNegative() const { return greenNegative_; }
  void setGreenNegative(bool b) { greenNegative_ = b; }

  bool isBlueNegative() const { return blueNegative_; }
  void setBlueNegative(bool b) { blueNegative_ = b; }

  void setRedMin(double r) { redMin_ = std::min(std::max(r, 0.0), 1.0); }
  double redMin() const { return redMin_; }
  void setRedMax(double r) { redMax_ = std::min(std::max(r, 0.0), 1.0); }
  double redMax() const { return redMax_; }

  void setGreenMin(double r) { greenMin_ = std::min(std::max(r, 0.0), 1.0); }
  double greenMin() const { return greenMin_; }
  void setGreenMax(double r) { greenMax_ = std::min(std::max(r, 0.0), 1.0); }
  double greenMax() const { return greenMax_; }

  void setBlueMin(double r) { blueMin_ = std::min(std::max(r, 0.0), 1.0); }
  double blueMin() const { return blueMin_; }
  void setBlueMax(double r) { blueMax_ = std::min(std::max(r, 0.0), 1.0); }
  double blueMax() const { return blueMax_; }

  //---

  // defined colors

  // get number of defined colors
  int numDefinedColors() const { return definedColors_.size(); }

  // get defined color
  const DefinedColors &definedColors() const { return definedColors_; }
  const ColorMap &definedValueColors() const { return definedValueColors_; }

  // get defined color
  const QColor &definedColor(int i) const {
    assert(i >= 0 && i < int(definedColors_.size()));
    return definedColors_[i].c;
  }

  // get defined color value
  double definedColorValue(int i) const {
    assert(i >= 0 && i < int(definedColors_.size()));
    return definedColors_[i].v;
  }

  // is existing defned color
  bool isDefinedColor(double v) const;

  // add new defined color
  void addDefinedColor(double v, const QColor &c);

  // clear defined colors
  void resetDefinedColors();

  // set individual defined color
  void setDefinedColor(int i, const QColor &c);

  // set defined colors
  void setDefinedColors(const ColorMap &colors);
  void setDefinedColors(const DefinedColors &colors);

  // map/unmap defined x (in range 0.0->1.0) to/from min/max
  double mapDefinedColorX(double x) const;
  double unmapDefinedColorX(double x) const;

  // are defined colors meant to be used as distinct values
  bool isDistinct() const { return definedDistinct_; }
  void setDistinct(bool b) { definedDistinct_ = b; }

  //---

  // get/set default value for number of colors
  int defaultNumColors() const { return defaultNumColors_; }
  void setDefaultNumColors(int i) { defaultNumColors_ = i; }

  //---

  //! get indexed color at i (from n colors)
  QColor getColor(int i, int n=-1, WrapMode wrapMode=WrapMode::NONE) const;

  //! interpolate color at x (if scaled then input x has been adjusted to min/max range)
  QColor getColor(double x, bool scale=false) const;

  //---

  //! interpolate color for model ind and x value
  static double interpModel(int ind, double x);

  //---

 public:
  // color models
  static int numModels() { return 37; }

  static std::string modelName(int model);

  //---

  // functions
  const std::string &redFunction() const { return rf_.fn; }
  void setRedFunction(const std::string &fn);

  const std::string &greenFunction() const { return gf_.fn; }
  void setGreenFunction(const std::string &fn);

  const std::string &blueFunction() const { return bf_.fn; }
  void setBlueFunction(const std::string &fn);

  void setFunctions(const std::string &rf, const std::string &gf, const std::string &bf);

  //---

  // cube helix
  void setCubeHelix(double start, double cycles, double saturation);

  double cbStart() const;
  void setCbStart(double r);

  double cbCycles() const;
  void setCbCycles(double r);

  double cbSaturation() const;
  void setCbSaturation(double r);

  bool isCubeNegative() const;
  void setCubeNegative(bool b);

  CCubeHelix *cubeHelix() const;

  //---

  //! read file containing defined colors
  bool readFile(const std::string &filename);

  //! save file containing defined colors
  bool saveFile(const std::string &filename);

  //---

  QImage getGradientImage(const QSize &size);

 private:
  bool readFileLines(const QStringList &lines);

  //---

 public:
  //! unset palette
  void unset();

  //---

  //! set linear gradient
  void setLinearGradient(QLinearGradient &lg, double a=1.0) const;

  //---

  //void show(std::ostream &os) const;

  //void showGradient(std::ostream &os) const;
  //void showRGBFormulae(std::ostream &os) const;
  //void showPaletteValues(std::ostream &os, int n, bool is_float, bool is_int);

  //---

  // util
 private:
  static double interpValue(double v1, double v2, double f) {
    return v1*(1 - f) + v2*f;
  }

 public:
  //! interpolate between two RGB colors
  static QColor interpRGB(const QColor &c1, const QColor &c2, double f) {
    qreal r1, g1, b1, a1;
    qreal r2, g2, b2, a2;

    c1.getRgbF(&r1, &g1, &b1, &a1);
    c2.getRgbF(&r2, &g2, &b2, &a2);

    return QColor::fromRgbF(interpValue(r1, r2, f),
                            interpValue(g1, g2, f),
                            interpValue(b1, b2, f));
  }

  //! interpolate between two HSV colors
  static QColor interpHSV(const QColor &c1, const QColor &c2, double f) {
    qreal h1, s1, v1, a1;
    qreal h2, s2, v2, a2;

    c1.getHsvF(&h1, &s1, &v1, &a1);
    c2.getHsvF(&h2, &s2, &v2, &a2);

    // fix invalid hue (gray)
    if      (h1 < 0 && h2 < 0) { h1 = 0.0; h2 = 0.0; }
    else if (h1 < 0)           { h1 = h2; }
    else if (h2 < 0)           { h2 = h1; }

    return QColor::fromHsvF(interpValue(h1, h2, f),
                            interpValue(s1, s2, f),
                            interpValue(v1, v2, f));
  }

 protected:
  void init();

  void initFunctions();

  CQTcl *qtcl() const;

 signals:
  void colorsChanged();

 protected:
  struct ColorFn {
    std::string fn;
  };

  QString      name_; //!< name
  QString      desc_; //!< description

  // Color Calculation Type
  ColorType    colorType_     { ColorType::MODEL }; //!< color type

  // Color Model Type
  ColorModel   colorModel_    { ColorModel::RGB };  //!< color model

  // Model
  int          rModel_        { 7 };     //!< red model number
  int          gModel_        { 5 };     //!< green model number
  int          bModel_        { 15 };    //!< blue model number
  bool         gray_          { false }; //!< is gray
  bool         redNegative_   { false }; //!< is red negated
  bool         greenNegative_ { false }; //!< is green negated
  bool         blueNegative_  { false }; //!< is blue negated
  double       redMin_        { 0.0 };   //!< red minimum
  double       redMax_        { 1.0 };   //!< red maximum
  double       greenMin_      { 0.0 };   //!< green minimum
  double       greenMax_      { 1.0 };   //!< green maximum
  double       blueMin_       { 0.0 };   //!< blue minimum
  double       blueMax_       { 1.0 };   //!< blue maximum

  // Functions
  ColorFn       rf_;                       //!< red color tcl function
  ColorFn       gf_;                       //!< green color tcl function
  ColorFn       bf_;                       //!< blue color tcl function
  CQTcl*        qtcl_         { nullptr }; //!< qtcl pointer for functions

  // CubeHelix
  CCubeHelix*   cubeHelix_    { nullptr }; //!< cube helix data
  bool          cubeNegative_ { false };   //!< is cube helix negated

  // Defined
  DefinedColors definedColors_;                 //!< array of defined colors
  ColorMap      definedValueColors_;            //!< map of defined colors by value
  double        definedMin_          { 0.0 };   //!< colors min value (for scaling)
  double        definedMax_          { 0.0 };   //!< colors max value (for scaling)
  bool          definedDistinct_     { false }; //!< prefer use distinct colors
  int           defaultNumColors_    { 100 };   //!< default number of colors for interp

#if 0
  // Misc
  double        gamma_ { 1.5 }; //!< gamma value
#endif

  QImage gradientImage_;               //!< gradient image (of size)
  bool   gradientImageDirty_ { true }; //!< is gradient image invalid
};

using CQColorsPaletteP = std::unique_ptr<CQColorsPalette>;

#endif
