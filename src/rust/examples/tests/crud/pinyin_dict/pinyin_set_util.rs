use crate::crud::pinyin_dict::pinyin_set::PINYIN_SET;

pub struct PinyinOptions {
    pub allow_prefix: bool,
    pub allow_initials: bool,
}

// 单个汉字的拼音最大长度
const PINYIN_MAX_LENGTH: usize = 6;

/// 输入不带空格拼音 → 输出 WCDB 可用 token 列表
pub fn generate_pinyin_tokens(input: &str, options: &PinyinOptions) -> Vec<String> {
    let s = input.to_lowercase();
    let chars: Vec<char> = s.chars().collect();
    let n = chars.len();
    let mut result = Vec::new();
    let mut i = 0;

    while i < n {
        let mut found = None;
        for len in (1..=PINYIN_MAX_LENGTH).rev() {
            if i + len <= n {
                let slice: String = chars[i..i + len].iter().collect();
                if PINYIN_SET.contains(slice.as_str()) {
                    found = Some(slice);
                    i += len;
                    break;
                }
            }
        }

        if let Some(py) = found {
            result.push(py);
        } else {
            if options.allow_initials {
                // 把单个字母当作首字母 token
                result.push(chars[i].to_string());
            } else if options.allow_prefix {
                // 把剩余部分当作前缀 token
                result.push(chars[i..].iter().collect());
                break;
            }
            i += 1;
        }
    }

    result
}

#[cfg(test)]
mod generate_pinyin_tokens_test {
    use crate::crud::pinyin_dict::pinyin_set_util::{generate_pinyin_tokens, PinyinOptions};

    #[test]
    fn test() {
        let options = PinyinOptions {
            allow_prefix: true,
            allow_initials: true,
        };

        let input = "zhongqinghuoguo"; // 完整拼音
        let token_vec = generate_pinyin_tokens(input, &options);
        assert_eq!(token_vec.join(" "), "zhong qing huo guo");

        let input = "zqhg"; // 首字母
        let token_vec = generate_pinyin_tokens(input, &options);
        assert_eq!(token_vec.join(" "), "z q h g");

        let input = "zhon"; // 前半部分
        let token_vec = generate_pinyin_tokens(input, &options);
        assert_eq!(token_vec.join(" "), "zh o n");

        let input = "bj"; // 北京缩写
        let token_vec = generate_pinyin_tokens(input, &options);
        assert_eq!(token_vec.join(" "), "b j");
    }
}
