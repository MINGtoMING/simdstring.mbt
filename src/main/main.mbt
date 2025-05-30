///|
fn main {
  let str = "z".repeat(10000) + "😀"
  try {
    let it = @bench.new()
    it.bench(fn() { it.keep(str.contains_char('😀')) })
    it.bench(fn() { it.keep(@simdstring.contains_char(str, '😀')) })
    match @json.parse(it.dump_summaries()) {
      [{ "mean": Number(mean1), .. }, { "mean": Number(mean2), .. }, ..] =>
        println("mean1: \{mean1}  mean2: \{mean2}")
      _ => panic()
    }
  } catch {
    e => println(e)
  }
}
