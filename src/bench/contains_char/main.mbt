///|
fn main {
  for needle in ['a', '😀'] {
    let haystack = String::make(100000, '😍') + needle.to_string()
    let it = @bench.new()
    it.bench(name="core/string", fn() {
      it.keep(haystack.contains_char(needle))
    })
    it.bench(name="scalar", fn() {
      it.keep(@scalar.contains_char(haystack, needle))
    })
    it.bench(name="sse4.2", fn() {
      it.keep(@sse4_2.contains_char(haystack, needle))
    })
    it.bench(name="avx2", fn() {
      it.keep(@avx2.contains_char(haystack, needle))
    })
    let calc_speed = fn(str : String, elapsed : Double) {
      str.length().to_double() * 2.0 / 1024 / 1024 / 1024 * 1000_000 / elapsed
    }
    match @json.parse?(it.dump_summaries()).unwrap() {
      [
        { "name": String(name1), "mean": Number(mean1), .. },
        { "name": String(name2), "mean": Number(mean2), .. },
        { "name": String(name3), "mean": Number(mean3), .. },
        { "name": String(name4), "mean": Number(mean4), .. },
        ..,
      ] => {
        let speed1 = calc_speed(haystack, mean1)
        let speed2 = calc_speed(haystack, mean2)
        let speed3 = calc_speed(haystack, mean3)
        let speed4 = calc_speed(haystack, mean4)
        let info =
          $|haystack length: \{haystack.length()} 
          $|needle: `\{needle}`
          $|
          $|               %-20s %-20s %-20s %-20s
          $|elapsed(us)    %-20.4f %-20.4f %-20.4f %-20.4f
          $|speed(GB/s)    %-20.4f %-20.4f %-20.4f %-20.4f
          $|
        let formatter = @fmt.Formatter::new(info)
        println(
          formatter.fmt([
            @fmt.Args_Type::String(name1),
            @fmt.Args_Type::String(name2),
            @fmt.Args_Type::String(name3),
            @fmt.Args_Type::String(name4),
            @fmt.Args_Type::Double(mean1),
            @fmt.Args_Type::Double(mean2),
            @fmt.Args_Type::Double(mean3),
            @fmt.Args_Type::Double(mean4),
            @fmt.Args_Type::Double(speed1),
            @fmt.Args_Type::Double(speed2),
            @fmt.Args_Type::Double(speed3),
            @fmt.Args_Type::Double(speed4),
          ]),
        )
      }
      _ => panic()
    }
  }
}
