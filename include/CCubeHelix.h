#ifndef CCubeHelix_H
#define CCubeHelix_H

#include <COSNaN.h>

//! \brief color helix gradient
class CCubeHelix {
 public:
  struct HSL {
    HSL(double h, double s, double l) :
     h(h), s(s), l(l) {
    }

    double h { 0.0 };
    double s { 1.0 };
    double l { 1.0 };
  };

 public:
  CCubeHelix(const HSL &a=HSL( 300/360.0, 0.5, 0.0),
             const HSL &b=HSL(-240/360.0, 0.5, 1.0)) :
   a_(a), b_(b) {
    init();
  }

  void init() {
    double radians = M_PI/180;

    ah_ = 360*(a_.h + start_)*radians;
    bh_ = 360*(b_.h + start_)*radians - ah_;
    as_ = a_.s;
    bs_ = b_.s - as_;
    al_ = a_.l;
    bl_ = b_.l - al_;

    if (COSNaN::is_nan(bs_)) bs_ = 0, as_ = COSNaN::is_nan(as_) ? b_.s : as_;
    if (COSNaN::is_nan(bh_)) bh_ = 0, ah_ = COSNaN::is_nan(ah_) ? b_.h*360 : ah_;
  }

  double start() const { return start_; }
  void setStart(double r) { start_ = r; init(); }

  double cycles() const { return cycles_; }
  void setCycles(double r) { cycles_ = r; }

  double saturation() const { return saturation_; }

  void setSaturation(double r) {
    saturation_ = r;

    a_.s = r;
    b_.s = r;

    init();
  }

  void reset() {
    start_      = 0;
    cycles_     = 1;
    saturation_ = 1;
  }

  QColor interp(double t, bool negate=false) const {
    double h = ah_ + bh_*t;
    double l = pow(al_ + bl_*t, std::max(cycles_, 0.01));
    double a = (as_ + bs_*t)*l*(1 - l);

    double cosh = cos(h);
    double sinh = sin(h);

    double r = l + a*(-0.14861*cosh + 1.78277*sinh);
    double g = l + a*(-0.29227*cosh - 0.90649*sinh);
    double b = l + a*(+1.97294*cosh);

    if (negate) {
      r = 1.0 - r;
      g = 1.0 - g;
      b = 1.0 - b;
    }

    return QColor(255*r, 255*g, 255*b);
  }

 private:
  double start_      { 1.0/3.0 };
  double cycles_     { 1 };
  double saturation_ { 0.5 };

  HSL    a_, b_;
  double ah_, bh_;
  double as_, bs_;
  double al_, bl_;
};

#endif
