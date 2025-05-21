///|
pub extern "C" fn has_suffix(
  haystack : @string.View,
  needle : @string.View
) -> Bool = "avx2_has_suffix"

///|
test "has_suffix/ture" {
  for _ in 0..<100 {
    let needle = String::from_array(@quickcheck.samples(1000))
    let haystack = String::from_array(@quickcheck.samples(1000)) + needle
    assert_eq!(has_suffix(haystack, needle), true)
  }
}

///|
test "has_suffix/false" {
  for _ in 0..<100 {
    let needle = String::from_array(@quickcheck.samples(1000))
    let haystack = String::from_array(@quickcheck.samples(1000)) +
      needle.substring(start=1)
    assert_eq!(has_suffix(haystack, needle), false)
  }
}
