///|
pub extern "C" fn has_prefix(
  haystack : @string.View,
  needle : @string.View
) -> Bool = "sse4_2_has_prefix"

///|
test "has_prefix/ture" {
  for _ in 0..<100 {
    let needle = String::from_array(@quickcheck.samples(1000))
    let haystack = needle + String::from_array(@quickcheck.samples(1000))
    assert_eq!(has_prefix(haystack, needle), true)
  }
}

///|
test "has_prefix/false" {
  for _ in 0..<100 {
    let needle = String::from_array(@quickcheck.samples(1000))
    let haystack = needle.substring(start=1) +
      String::from_array(@quickcheck.samples(1000))
    assert_eq!(has_prefix(haystack, needle), false)
  }
}
