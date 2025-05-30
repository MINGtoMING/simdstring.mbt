///|
test (it : @bench.T) {
  let str = "z".repeat(10000) + "😀"
  it.bench(fn() { it.keep(str.contains_char('😀')) })
  it.bench(fn() { it.keep(@simdstring.contains_char(str, '😀')) })
}
