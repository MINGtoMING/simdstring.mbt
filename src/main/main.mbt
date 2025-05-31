///|
fn main {
  let str = "😁" + String::make(10000, 'z') + "😀"
  // println(str.find("😀"))
  // println(@simdstring.find_char(str, 'a'))
  try {
    let it = @bench.new()
    // it.bench(fn() { it.keep(str.find("😀")) })
    it.bench(fn() { it.keep(@simdstring.find_char(str, '😀')) })
    it.bench(fn() { it.keep(@simdstring.rev_find_char(str, '😁')) })
    match @json.parse(it.dump_summaries()) {
      [{ "mean": Number(mean1), .. }, { "mean": Number(mean2), .. }, ..] =>
        println("mean1: \{mean1}  mean2: \{mean2}")
      _ => panic()
    }
  } catch {
    e => println(e)
  }

  // let mut x : Int? = None
  // println(x)
  // x = Some(-1)
  // println(x)
}
