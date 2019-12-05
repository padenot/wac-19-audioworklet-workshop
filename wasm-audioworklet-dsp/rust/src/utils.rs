use std::mem;

pub fn clamp<T>(v: T, lower_bound: T, higher_bound: T) -> T
where
    T: std::cmp::PartialOrd,
{
    if v < lower_bound {
        return lower_bound;
    } else if v > higher_bound {
        return higher_bound;
    } else {
        return v;
    }
}

pub fn max<T>(a: T, b: T) -> T
where
    T: std::cmp::PartialOrd,
{
    if a > b {
        a
    } else {
        b
    }
}

pub fn gcd(mut a: u64, mut b: u64) -> u64 {
    if a < b {
        mem::swap(&mut a, &mut b);
    }

    while b != 0 {
        mem::swap(&mut a, &mut b);
        b %= a;
    }

    a
}

pub fn coprime(a: u64, b: u64) -> bool {
    gcd(a, b) == 1
}

pub fn coprime_with_series(proposed: u64, series: &[u64]) -> bool {
    for i in series.iter() {
        if !coprime(*i, proposed) {
            return false;
        }
    }

    true
}

/// Find a series of `count` number that are set coprime, and start at `start`, with a geometric
/// progression of ratio `factor`
pub fn coprime_with_progression(start: u64, factor: f32, count: usize) -> Vec<u64> {
    let mut series = Vec::with_capacity(count);
    let mut current = (start as f32 * factor) as u64;

    series.push(start);

    while series.len() != count {
        if coprime_with_series(current, &series) {
            series.push(current);
            continue;
        }
        while !coprime_with_series(current, &series) {
            current += 1;
        }
        for i in series.iter() {
            println!("testing {} and {}", current, *i);
            if (((current as f32) / *i as f32) - 2.0).abs() > 0.05 {
                println!("{} is too close to {}, nudging", current, *i);
                current = (current as f32 * 1.05) as u64;
            }
        }
        series.push(current);
        current = (current as f32 * factor) as u64;
    }
    return series;
}

// http://en.wikipedia.org/wiki/Hadamard_matrix sylvester construction
pub fn hadamard(order: usize) -> Result<Vec<f32>, ()> {
    fn idx(x: usize, y: usize, w: usize) -> usize {
        y * w + x
    }
    if order == 0 || (order & (order - 1)) != 0 {
        return Err(());
    }
    let mut mat = Vec::<f32>::with_capacity(order * order);
    mat.resize(order * order, 0.);
    mat[0] = 1.0;

    let mut n = 1;
    while n < order {
        for x in 0..n {
            for y in 0..n {
                mat[idx(x + n, y, order)] = mat[idx(x, y, order)];
                mat[idx(x, y + n, order)] = mat[idx(x, y, order)];
                mat[idx(x + n, y + n, order)] = -mat[idx(x, y, order)];
            }
        }
        n += n;
    }

    return Ok(mat);
}

pub fn matrix_vector_multiply(v: &[f32; 4], m: &[f32; 16]) -> [f32; 4] {
    let mut r = [0.; 4];
    for i in 0..4 {
        for j in 0..4 {
            r[i] += m[i * 4 + j] * v[j];
        }
    }
    r
}
