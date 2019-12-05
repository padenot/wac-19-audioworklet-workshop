#include <cmath>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <assert.h>

template <typename T>
T clamp(T v, T lower_bound, T higher_bound)
{
  if (v < lower_bound) {
    return lower_bound;
  } else if (v > higher_bound) {
    return higher_bound;
  }

  return v;
}

size_t gcd(size_t a, size_t b) {
  if (a < b) {
    std::swap(a, b);
  }

  while (b != 0) {
    std::swap(a, b);
    b %= a;
  }

  return a;
}

bool coprime(size_t a, size_t b)
{
  return gcd(a, b) == 1;
}


bool coprime_with_series(size_t proposed, const std::vector<size_t>& series)
{
  for (size_t i = 0; i < series.size(); i++) {
    if (!coprime(series[i], proposed)) {
      return false;
    }
  }
  return true;
}

/// Find a series of `count` number that are set coprime, and start at `start`, with a geometric
/// progression of ratio `factor`
std::vector<size_t> coprime_with_progression(size_t start, float factor, size_t count)
{
  std::vector<size_t> series;
  series.reserve(count);
  size_t current = static_cast<size_t>(start * factor);

  series.push_back(start);

  while (series.size() != count) {
    if (coprime_with_series(current, series)) {
      series.push_back(current);
      current = current * factor;
      continue;
    }
    while (!coprime_with_series(current, series)) {
      current++;
    }
    for (size_t i = 0; i < series.size(); i++) {
      if (std::abs((static_cast<float>(current) / series[i] - 2.0)) > 0.05) {
        // too close, nudging by 5%
        current *= 1.05;
      }
    }
    series.push_back(current);
    current = current * factor;
  }

  return series;
}

size_t idx(size_t x, size_t y, size_t width) {
  return y * width + x;
}

// http://en.wikipedia.org/wiki/Hadamard_matrix sylvester construction
std::vector<float> hadamard(size_t order)
{
  if ((order == 0 || order & (order - 1)) != 0) {
    assert(false && "Error, order must be a even");
  }
  std::vector<float> mat(order * order, 0.0);
  mat[0] = 1.0;

  size_t n = 1;
  while (n < order) {
    for (size_t x = 0; x < n; x++) {
      for (size_t y = 0; y < n; y++) {
        mat[idx(x + n, y, order)] = mat[idx(x, y, order)];
        mat[idx(x, y + n, order)] = mat[idx(x, y, order)];
        mat[idx(x + n, y + n, order)] = -mat[idx(x, y, order)];
      }
    }
    n += n;
  }

  return mat;
}

template <size_t N>
void matrix_vector_multiply(float vector[N], float matrix[N*N], float output[N])
{
  for (size_t i = 0; i < N; i++) {
    for (size_t j = 0; j < N; j++) {
      output[i] += matrix[i * N + j] * vector[j];
    }
  }
}
