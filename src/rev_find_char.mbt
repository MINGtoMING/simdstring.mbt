///|
pub extern "c" fn rev_find_char(haystack : @string.View, needle : Char) -> Int? = "rev_find_char"

///|
test "rev_find_char" {
  inspect(rev_find_char("hello", 'h'), content="Some(0)")
  inspect(rev_find_char("hello", 'e'), content="Some(1)")
  inspect(rev_find_char("hello", 'o'), content="Some(4)")
  inspect(rev_find_char("hello", 'x'), content="None")
}

///|
test "rev_find_char/empty" {
  inspect(rev_find_char("", 'a'), content="None")
  inspect(rev_find_char(" ", ' '), content="Some(0)")
}

///|
test "rev_find_char/unicode" {
  inspect(rev_find_char("你好", '你'), content="Some(0)")
  inspect(rev_find_char("你好", '好'), content="Some(1)")
  inspect(rev_find_char("hello世界", '世'), content="Some(5)")
  inspect(rev_find_char("hello🌍", '🌍'), content="Some(5)")
}

///|
test "rev_find_char/fuzz" {
  let iter_num = 100
  let haystack_len = 500
  let rand = @random.Rand::new()
  for _ in 0..<iter_num {
    let gen_char = fn() {
      loop rand.uint(limit=0x10FFFF) {
        0..=0xD7FF | 0xE000..=0x10FFFF as cp =>
          break cp.reinterpret_as_int().to_char().unwrap()
        _ => continue rand.uint(limit=0x10FFFF)
      }
    }
    let needle = gen_char()
    let haystack_without_needle = String::from_array(
      Array::makei(haystack_len, fn(_) {
        loop gen_char() {
          char if char == needle => continue gen_char()
          char => break char
        }
      }),
    )
    inspect(rev_find_char(haystack_without_needle, needle), content="None")
    let mut target = 0
    for i in 0..<haystack_len {
      let haystack_with_needle = haystack_without_needle.to_array()
      haystack_with_needle[i] = needle
      let haystack_with_needle = String::from_array(haystack_with_needle)
      inspect(
        rev_find_char(haystack_with_needle, needle),
        content="Some(\{target})",
      )
      match haystack_without_needle.to_array()[i].to_int() {
        0..=0xFFFF => target += 1
        _ => target += 2
      }
    }
  }
}
