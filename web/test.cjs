// node web/test.cjs  -- checks the algorithms behind the browser demo
const assert = require('assert');
const CG = require('./cg.js');

const rnd = (n) => Math.floor(Math.random() * (2 * n + 1)) - n;

for (let t = 0; t < 2000; t++) {
  const [x0, y0, x1, y1] = [rnd(40), rnd(40), rnd(40), rnd(40)];
  const steps = Math.max(Math.abs(x1 - x0), Math.abs(y1 - y0));
  for (const name of ['dda', 'bresenham']) {
    const pts = CG[name](x0, y0, x1, y1).points;
    assert.deepStrictEqual(pts[0], [x0, y0], `${name} start`);
    assert.deepStrictEqual(pts.at(-1), [x1, y1], `${name} end ${[x0, y0, x1, y1]}`);
    assert.strictEqual(pts.length, steps + 1, `${name} one pixel per step`);
    for (let i = 1; i < pts.length; i++) {
      const ax = Math.abs(pts[i][0] - pts[i - 1][0]), ay = Math.abs(pts[i][1] - pts[i - 1][1]);
      assert(Math.max(ax, ay) === 1, `${name} 8-connected`);
    }
    // every pixel is within half a pixel of the true line, along the minor axis
    for (const [x, y] of pts) {
      const steep = Math.abs(y1 - y0) > Math.abs(x1 - x0);
      const ideal = steep ? x0 + (x1 - x0) * (y - y0) / (y1 - y0) : y0 + (y1 - y0) * (x - x0) / (x1 - x0);
      if (steps) assert(Math.abs((steep ? x : y) - ideal) <= 0.5 + 1e-9, `${name} close to the line`);
    }
  }
}

for (let r = 0; r <= 60; r++) {
  const steps = CG.midpointCircle(r);
  assert.deepStrictEqual([steps[0].x, steps[0].y], [0, r]);
  for (const { x, y } of steps) {
    assert(Math.abs(Math.hypot(x, y) - r) < 0.75, `circle r=${r} point (${x},${y}) near the rim`);
    assert(x <= y + 1, 'stays in the first octant');
  }
}

let c = CG.liangBarsky(0, 5, 10, 5, 2, 0, 8, 10);
assert.deepStrictEqual(c.clipped, [2, 5, 8, 5], 'horizontal line clipped both sides');
assert(!CG.liangBarsky(0, 20, 10, 20, 2, 0, 8, 10).accepted, 'parallel and outside: rejected');
assert(!CG.liangBarsky(0, 0, 1, 1, 2, 2, 8, 8).accepted, 'stops before the window: rejected');
assert.deepStrictEqual(CG.liangBarsky(3, 3, 4, 4, 2, 2, 8, 8).clipped, [3, 3, 4, 4], 'inside: unchanged');
assert.deepStrictEqual(CG.liangBarsky(0, 0, 10, 10, 2, 2, 8, 8).clipped, [2, 2, 8, 8], 'diagonal through corners');

const near = (a, b) => a.every((v, i) => Math.abs(v - b[i]) < 1e-9);
const { mat } = CG;
assert(near(mat.apply(mat.rotate(90), [[1, 0]])[0], [0, 1]), 'rotate 90');
assert(near(mat.apply(mat.reflectLine(1, 0), [[2, 0]])[0], [0, 2]), 'reflect about y = x');
assert(near(mat.apply(mat.reflectLine(0, 3), [[1, 5]])[0], [1, 1]), 'reflect about y = 3');
assert(near(mat.apply(mat.about(mat.rotate(37), 4, 7), [[4, 7]])[0], [4, 7]), 'pivot stays put');
assert(near(mat.apply(mat.about(mat.scale(2, 2), 1, 1), [[2, 3]])[0], [3, 5]), 'scale about a pivot');

console.log('cg.js: all checks passed');
