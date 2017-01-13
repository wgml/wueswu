
#ifndef WUESWU_KALMANFILTER_H
#define WUESWU_KALMANFILTER_H

#include <array>

namespace kalman_filter {
/**
 * Implements Kalman filter to estimate input signal state.
 * @see report for details
 *
 * Create date:
 *    16/12/18
 * Last modification date:
 *    16/12/25
 *
 * @authors Anna Musiał, Wojciech Gumuła
 *
 * @param measurements input data container
 * @param prediction output data contaier
 */
void predict(const std::vector<double> &measurements, std::vector<double> &prediction) {
  // subscribing is Mat_t[row][col]
  using Mat_t = std::array<std::array<double, 3>, 3>;
  using Col_t = std::array<double, 3>;
  const double Q = 0.02, R = 0.1;

  Mat_t P = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
  Col_t X_predict = {0, 0, 0};
  Col_t X_predict_prev;

  for (auto i = 0; i < measurements.size(); i++) {
    auto signal = measurements[i];

    if (i > 0) {
      X_predict = {{
                       2 * X_predict_prev[0] - X_predict_prev[1],
                       X_predict_prev[0],
                       X_predict_prev[1]}};
    }

    Mat_t P_predict = {{
                           {
                               4 * P[0][0] - 2 * P[1][0] - 2 * P[0][1] + P[1][1] + Q,
                               2 * P[0][0] - P[1][0],
                               2 * P[0][1] - P[1][1]
                           },
                           {
                               2 * P[0][0] - P[0][1],
                               P[0][0],
                               P[0][1]
                           },
                           {
                               2 * P[1][0] - P[1][1],
                               P[1][0],
                               P[1][1]
                           }
                       }};
    Col_t K = {{
                   P_predict[0][0] / (P_predict[0][0] + R),
                   P_predict[1][0] / (P_predict[0][0] + R),
                   P_predict[2][0] / (P_predict[0][0] + R)
               }};
    Col_t Xk = {{
                    X_predict[0] + K[0] * (signal - X_predict[0]),
                    X_predict[1] + K[1] * (signal - X_predict[0]),
                    X_predict[2] + K[2] * (signal - X_predict[0])
                }};

    P = {{
             {
                 P_predict[0][0] * (1 - K[0]),
                 P_predict[0][1] * (1 - K[0]),
                 P_predict[0][2] * (1 - K[0])
             },
             {
                 -P_predict[1][0] * K[1] + P_predict[1][0],
                 -P_predict[1][1] * K[1] + P_predict[1][1],
                 -P_predict[1][2] * K[1] + P_predict[1][2]
             },
             {
                 -P_predict[2][0] * K[2] + P_predict[2][0],
                 -P_predict[2][1] * K[2] + P_predict[2][1],
                 -P_predict[2][2] * K[2] + P_predict[2][2]
             }
         }};
    X_predict_prev = Xk;
    prediction[i] = Xk[0];
  }
}
};

#endif //WUESWU_KALMANFILTER_H
