// The algorithms from the C++ programs, as pure functions. Used by index.html and test.cjs.
// Coordinates are maths-style: y grows upward, like gluOrtho2D in the originals.
const CG = {
  // DDA: step along the longer axis, add a float increment to the other, round.
  dda(x0, y0, x1, y1) {
    const dx = x1 - x0, dy = y1 - y0;
    const steps = Math.max(Math.abs(dx), Math.abs(dy));
    const xinc = steps ? dx / steps : 0, yinc = steps ? dy / steps : 0;
    const points = [], trace = [];
    let x = x0, y = y0;
    for (let i = 0; i <= steps; i++) {
      const px = Math.round(x) + 0, py = Math.round(y) + 0; // + 0 turns -0 into 0
      points.push([px, py]);
      trace.push({ i, x: +x.toFixed(2), y: +y.toFixed(2), plot: `(${px}, ${py})` });
      x += xinc; y += yinc;
    }
    return { points, trace, info: { dx, dy, steps, xinc: +xinc.toFixed(3), yinc: +yinc.toFixed(3) } };
  },

  // Generalised Bresenham (integers only). p is the decision variable; while it is positive
  // the minor axis steps, every iteration the major axis steps.
  bresenham(x0, y0, x1, y1) {
    let delx = Math.abs(x1 - x0), dely = Math.abs(y1 - y0);
    const s1 = Math.sign(x1 - x0), s2 = Math.sign(y1 - y0);
    let interchange = 0;
    if (dely > delx) { [delx, dely] = [dely, delx]; interchange = 1; }
    let p = 2 * dely - delx, x = x0, y = y0;
    const points = [], trace = [];
    for (let i = 0; i <= delx; i++) {
      points.push([x, y]);
      const pIn = p;
      let minor = false;
      while (p > 0) {
        if (interchange) x += s1; else y += s2;
        p -= 2 * delx;
        minor = true;
      }
      if (interchange) y += s2; else x += s1;
      p += 2 * dely;
      trace.push({ i, plot: `(${points[i][0]}, ${points[i][1]})`, p: pIn, step: minor ? 'both' : 'major' });
    }
    return { points, trace, info: { delx, dely, s1, s2, interchange } };
  },

  // Midpoint circle: walk one octant from (0, r) until x >= y. Points are relative to the centre.
  midpointCircle(r) {
    let x = 0, y = r, P = 1 - r;
    const steps = [{ x, y, P }];
    while (x < y) {
      x += 1;
      if (P < 0) P += 2 * x + 1;
      else { y -= 1; P += 2 * x - 2 * y + 1; }
      steps.push({ x, y, P });
    }
    return steps;
  },

  // The 8 symmetric images of an octant point.
  octants(x, y) {
    return [[x, y], [y, x], [y, -x], [x, -y], [-x, -y], [-y, -x], [-y, x], [-x, y]];
  },

  // Liang-Barsky: P(u) = P0 + u*(P1-P0). Each edge k gives p_k * u <= q_k.
  liangBarsky(x0, y0, x1, y1, xmin, ymin, xmax, ymax) {
    const dx = x1 - x0, dy = y1 - y0;
    const edges = ['left', 'right', 'bottom', 'top'];
    const p = [-dx, dx, -dy, dy];
    const q = [x0 - xmin, xmax - x0, y0 - ymin, ymax - y0];
    let u1 = 0, u2 = 1, rejected = false;
    const rows = edges.map((edge, k) => {
      let role, r = null;
      if (p[k] === 0) {
        role = q[k] < 0 ? 'parallel, outside' : 'parallel, inside';
        if (q[k] < 0) rejected = true;
      } else {
        r = q[k] / p[k];
        if (p[k] < 0) { role = 'entering'; u1 = Math.max(u1, r); }
        else { role = 'leaving'; u2 = Math.min(u2, r); }
      }
      return { edge, p: p[k], q: q[k], r, role };
    });
    const accepted = !rejected && u1 < u2;
    return {
      rows, u1, u2, accepted,
      clipped: accepted ? [x0 + dx * u1, y0 + dy * u1, x0 + dx * u2, y0 + dy * u2] : null,
    };
  },

  // 3x3 homogeneous matrices, row-major [a,b,c, d,e,f, 0,0,1].
  mat: {
    I: () => [1, 0, 0, 0, 1, 0, 0, 0, 1],
    mul(A, B) {
      const C = new Array(9).fill(0);
      for (let i = 0; i < 3; i++)
        for (let j = 0; j < 3; j++)
          for (let k = 0; k < 3; k++) C[i * 3 + j] += A[i * 3 + k] * B[k * 3 + j];
      return C;
    },
    translate: (tx, ty) => [1, 0, tx, 0, 1, ty, 0, 0, 1],
    rotate(deg) {
      const t = deg * Math.PI / 180, c = Math.cos(t), s = Math.sin(t);
      return [c, -s, 0, s, c, 0, 0, 0, 1];
    },
    scale: (sx, sy) => [sx, 0, 0, 0, sy, 0, 0, 0, 1],
    shearX: (sh) => [1, sh, 0, 0, 1, 0, 0, 0, 1],
    // reflection about y = m x + c
    reflectLine(m, c) {
      const d = m * m + 1;
      return [(1 - m * m) / d, 2 * m / d, -2 * m * c / d, 2 * m / d, (m * m - 1) / d, 2 * c / d, 0, 0, 1];
    },
    // apply M around a pivot: T(pivot) . M . T(-pivot)
    about(M, px, py) {
      return CG.mat.mul(CG.mat.translate(px, py), CG.mat.mul(M, CG.mat.translate(-px, -py)));
    },
    apply(M, pts) {
      return pts.map(([x, y]) => [M[0] * x + M[1] * y + M[2], M[3] * x + M[4] * y + M[5]]);
    },
  },
};

if (typeof module !== 'undefined') module.exports = CG;
