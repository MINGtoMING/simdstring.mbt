///|
pub extern "c" fn contains_char(haystack : @string.View, needle : Char) -> Bool = "contains_char"

///|
test "contains_char/basic" {
  inspect(contains_char("hello", 'h'), content="true")
  inspect(contains_char("hello", 'o'), content="true")
  inspect(contains_char("hello", 'l'), content="true")
  inspect(contains_char("hello", 'x'), content="false")
  inspect(contains_char("aaa", 'a'), content="true")
  inspect(contains_char("ababa", 'b'), content="true")
  inspect(contains_char("Hello", 'h'), content="false")
  inspect(contains_char("Hello", 'H'), content="true")
}

///|
test "contains_char/edge_cases" {
  inspect(contains_char("", 'a'), content="false")
  inspect(contains_char("a", 'a'), content="true")
  inspect(contains_char("a", 'b'), content="false")
  inspect(contains_char(" ", ' '), content="true")
  inspect(contains_char("\t", '\t'), content="true")
  inspect(contains_char("\n", '\n'), content="true")
}

///|
test "contains_char/unicode" {
  inspect(contains_char("こんにちは", 'こ'), content="true")
  inspect(contains_char("你好", '你'), content="true")
  inspect(contains_char("Привет", 'П'), content="true")
  inspect(contains_char("😊", '😊'), content="true")
  inspect(contains_char("a😊b", '😊'), content="true")
}

///|
test "contains_char/case_sensitivity" {
  inspect(contains_char("Hello", 'h'), content="false")
  inspect(contains_char("Hello", 'H'), content="true")
  inspect(contains_char("hElLo", 'E'), content="true")
  inspect(contains_char("hElLo", 'e'), content="false")
}

///|
test "contains_char/special_chars" {
  inspect(contains_char("a\\b", '\\'), content="true")
  inspect(contains_char("a\"b", '\"'), content="true")
  inspect(contains_char("a'b", '\''), content="true")
  inspect(contains_char("a\u0000b", '\u0000'), content="true")
}

///|
test "contains_char/repeated_chars" {
  inspect(contains_char("aaaaa", 'a'), content="true")
  inspect(contains_char("aabaa", 'b'), content="true")
  inspect(contains_char("abcba", 'b'), content="true")
}

///|
test "contains_char/numeric" {
  inspect(contains_char("12345", '1'), content="true")
  inspect(contains_char("12345", '5'), content="true")
  inspect(contains_char("12345", '0'), content="false")
  inspect(contains_char("1a2b3", 'a'), content="true")
}

///|
test "contains_char/fuzz" {
  let iter_num = 100
  let haystack_len = 500
  let rand = @random.Rand::new()
  for _ in 0..<iter_num {
    let gen_char = fn() {
      loop rand.uint(limit=0x10FFFF) {
        0..=0xD7FF | 0xE000..=0x10FFFF as cp =>
          cp.reinterpret_as_int().to_char().unwrap()
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
    inspect(contains_char(haystack_without_needle, needle), content="false")
    for i in 0..<haystack_len {
      let haystack_with_needle = haystack_without_needle.to_array()
      haystack_with_needle[i] = needle
      let haystack_with_needle = String::from_array(haystack_with_needle)
      inspect(contains_char(haystack_with_needle, needle), content="true")
    }
  }
}
