const double b[] = {0.00024132, 0.00048264, 0.00024132};
const double a[] = {1.95558189, -0.95654717};

double butterworth2(double y1, double y2, double x0, double x1, double x2) {
  return (a[0] * y1 + a[1] * y2 +
          b[0] * x0 + b[1] * x1 + b[2] * x2);
}
