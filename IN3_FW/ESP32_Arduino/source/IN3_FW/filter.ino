//butterworth2 0.8hz cutoff frequency, 1khz sampling, 2nd order, low pass

const double b[] = {0.000006294162241, 0.00001258832448, 0.000006294162241};
const double a[] = {1.9928914235, -0.9929165938};

double butterworth2(double y1, double y2, double x0, double x1, double x2) {
  return (a[0] * y1 + a[1] * y2 +
          b[0] * x0 + b[1] * x1 + b[2] * x2);
}
