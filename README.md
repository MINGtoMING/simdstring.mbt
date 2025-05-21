# simdstring 🚀

**SIMD-accelerated string utilities** for Moonbit native backend.

## 📊 Benchmarks

### `contains_char` Performance

| Metric     | core/string | Scalar    | SSE4.2     | AVX2       |
|------------|-------------|-----------|------------|------------|
| Time (μs)  | 684.69      | 74.88     | 10.16      | 8.24       |
| Throughput | 0.54 GB/s   | 4.98 GB/s | 36.68 GB/s | 45.21 GB/s |

*Test environment: AMD Ryzen 7 3750H, Moonbit native backend*  
*Run benchmark:*

```shell
moon run ./src/bench/contains_char --release --target native
```

## 🛠️ Installation

Add as dependency:

```shell
moon add MINGtoMING/simdstring
```

Then include in your `moon.pkg.json`:

```json
{
  "import": [
    "MINGtoMING/simdstring"
  ]
}
```

## 📚 API Coverage

### Current Implementation Status

| Method          | AVX2 | SSE4.2 | Scalar |
|-----------------|------|--------|--------|
| `contains_char` | ✅    | ✅      | ✅      |
| `contains`      | ⏳    | ⏳      | ⏳      |
| `find`          | ⏳    | ⏳      | ⏳      |
| `has_prefix`    | ✅    | ✅      | ✅      |
| `has_suffix`    | ✅    | ✅      | ✅      |
| `to_upper`      | ⏳    | ⏳      | ⏳      |
| `to_lower`      | ⏳    | ⏳      | ⏳      |

*✅(Implemented)  ⏳(Planned)*

## 📜 License

MIT Licensed. See [LICENSE](LICENSE) for details.
