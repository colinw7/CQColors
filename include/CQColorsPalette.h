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

#define CQCOLORS_TCL 1

class CCubeHelix;
#ifdef CQCOLORS_TCL
class CQTcl;
#endif
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
    CUBEHELIX,
    CUSTOM
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

  using ColorMap = std::map<double, QColor>;

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
    else if (str == "custom"   ) return ColorType::CUSTOM;

    return ColorType::NONE;
  }

  static QString colorTypeToString(ColorType type) {
    switch (type) {
      case ColorType::MODEL    : return "model";
      case ColorType::DEFINED  : return "defined";
      case ColorType::FUNCTIONS: return "functions";
      case ColorType::CUBEHELIX: return "cubehelix";
      case ColorType::CUSTOM   : return "custom";
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

  int redModel() const { return modelData_.rModel; }
  void setRedModel(int r) { modelData_.rModel = r; }

  int greenModel() const { return modelData_.gModel; }
  void setGreenModel(int r) { modelData_.gModel = r; }

  int blueModel() const { return modelData_.bModel; }
  void setBlueModel(int r) { modelData_.bModel = r; }

  bool isGray() const { return modelData_.gray; }
  void setGray(bool b) { modelData_.gray = b; }

  bool isRedNegative() const { return modelData_.redNegative; }
  void setRedNegative(bool b) { modelData_.redNegative = b; }

  bool isGreenNegative() const { return modelData_.greenNegative; }
  void setGreenNegative(bool b) { modelData_.greenNegative = b; }

  bool isBlueNegative() const { return modelData_.blueNegative; }
  void setBlueNegative(bool b) { modelData_.blueNegative = b; }

  void setRedMin(double r) { modelData_.redMin = std::min(std::max(r, 0.0), 1.0); }
  double redMin() const { return modelData_.redMin; }
  void setRedMax(double r) { modelData_.redMax = std::min(std::max(r, 0.0), 1.0); }
  double redMax() const { return modelData_.redMax; }

  void setGreenMin(double r) { modelData_.greenMin = std::min(std::max(r, 0.0), 1.0); }
  double greenMin() const { return modelData_.greenMin; }
  void setGreenMax(double r) { modelData_.greenMax = std::min(std::max(r, 0.0), 1.0); }
  double greenMax() const { return modelData_.greenMax; }

  void setBlueMin(double r) { modelData_.blueMin = std::min(std::max(r, 0.0), 1.0); }
  double blueMin() const { return modelData_.blueMin; }
  void setBlueMax(double r) { modelData_.blueMax = std::min(std::max(r, 0.0), 1.0); }
  double blueMax() const { return modelData_.blueMax; }

  //---

  // defined colors

  // get number of defined colors
  int numDefinedColors() const;

  // get defined color
  DefinedColors definedColors() const;
  ColorMap definedValueColors() const;

  // get defined color
  QColor definedColor(int i) const;

  // get defined color value
  double definedColorValue(int i) const;

  // get defined color data
  DefinedColor definedColorData(int i) const;

  // is existing defined color
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
  bool isDistinct() const;
  void setDistinct(bool b);

  // are defined colors inverted
  bool isInverted() const;
  void setInverted(bool b);

  //---

  // get/set default value for number of colors
  int defaultNumColors() const { return defaultNumColors_; }
  void setDefaultNumColors(int i) { defaultNumColors_ = i; }

  //---

  //! get indexed color at i (from n colors)
  QColor getColor(int i, int n=-1, WrapMode wrapMode=WrapMode::NONE) const;

  //! interpolate color at x (if scaled then input x has been adjusted to min/max range)
  QColor getColor(double x, bool scale=false, bool invert=false) const;

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
  const std::string &redFunction() const { return tclFnData_.rf.fn; }
  void setRedFunction(const std::string &fn);

  const std::string &greenFunction() const { return tclFnData_.gf.fn; }
  void setGreenFunction(const std::string &fn);

  const std::string &blueFunction() const { return tclFnData_.bf.fn; }
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
  void setLinearGradient(QLinearGradient &lg, double a=1.0,
                         double xmin=0.0, double xmax=1.0, bool enabled=true) const;

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

#ifdef CQCOLORS_TCL
  CQTcl *qtcl() const;
#endif

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
  struct ModelData {
    int    rModel        { 7 };     //!< red model number
    int    gModel        { 5 };     //!< green model number
    int    bModel        { 15 };    //!< blue model number
    bool   gray          { false }; //!< is gray
    bool   redNegative   { false }; //!< is red negated
    bool   greenNegative { false }; //!< is green negated
    bool   blueNegative  { false }; //!< is blue negated
    double redMin        { 0.0 };   //!< red minimum
    double redMax        { 1.0 };   //!< red maximum
    double greenMin      { 0.0 };   //!< green minimum
    double greenMax      { 1.0 };   //!< green maximum
    double blueMin       { 0.0 };   //!< blue minimum
    double blueMax       { 1.0 };   //!< blue maximum
  };

  ModelData modelData_;

  // Functions
  struct TclFnData {
    ColorFn rf; //!< red color tcl function
    ColorFn gf; //!< green color tcl function
    ColorFn bf; //!< blue color tcl function
  };

  TclFnData tclFnData_;

#ifdef CQCOLORS_TCL
  CQTcl* qtcl_ { nullptr }; //!< qtcl pointer for functions
#endif

  // CubeHelix
  CCubeHelix* cubeHelix_    { nullptr }; //!< cube helix data
  bool        cubeNegative_ { false };   //!< is cube helix negated

  // Defined
  struct DefinedData {
    DefinedColors definedColors;                //!< array of defined colors
    ColorMap      definedValueColors;           //!< map of defined colors by value
    double        definedMin         { 0.0 };   //!< colors min value (for scaling)
    double        definedMax         { 0.0 };   //!< colors max value (for scaling)
    bool          definedDistinct    { false }; //!< prefer use distinct colors
    bool          definedInverted    { false }; //!< invert color order
  };

  DefinedData definedData_;

  int defaultNumColors_ { 100 };   //!< default number of colors for interp

#if 0
  // Misc
  double gamma_ { 1.5 }; //!< gamma value
#endif

  QImage gradientImage_;               //!< gradient image (of size)
  bool   gradientImageDirty_ { true }; //!< is gradient image invalid
};

using CQColorsPaletteP = std::unique_ptr<CQColorsPalette>;

#endif
