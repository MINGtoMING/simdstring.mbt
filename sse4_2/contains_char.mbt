///|
pub extern "C" fn contains_char(str : @string.View, cp : Char) -> Bool = "sse4_2_contains_char"

///|
fn arr_to_string(arr : FixedArray[UInt16]) -> String = "%identity"

///|
test "contains_char/bmp" {
  let arr : Array[UInt16] = Array::make(1000, 0)
  for i in 0..<1000 {
    arr[i] = 98
    assert_eq!(
      contains_char(arr_to_string(FixedArray::from_array(arr)), 'b'),
      true,
    )
    arr[i] = 0
    assert_eq!(
      contains_char(arr_to_string(FixedArray::from_array(arr)), 'b'),
      false,
    )
  }
}

///|
test "contains_char/surrogate" {
  let arr : Array[UInt16] = Array::make(1000, 0)
  for i in 0..<999 {
    arr[i] = 55357
    arr[i + 1] = 56832
    assert_eq!(
      contains_char(arr_to_string(FixedArray::from_array(arr)), '😀'),
      true,
    )
    arr[i] = 0
    assert_eq!(
      contains_char(arr_to_string(FixedArray::from_array(arr)), '😀'),
      false,
    )
    arr[i + 1] = 0
    assert_eq!(
      contains_char(arr_to_string(FixedArray::from_array(arr)), '😀'),
      false,
    )
  }
}
