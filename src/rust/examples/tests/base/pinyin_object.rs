use wcdb_derive::WCDBTableCoding;

#[derive(WCDBTableCoding, PartialEq, Clone)]
#[WCDBTable(fts_module(version = "FTS5", tokenizer = "wcdb_pinyin"))]
pub struct PinyinObject {
    #[WCDBField]
    pub content: String,
}

impl PinyinObject {
    pub fn new() -> Self {
        Self {
            content: String::new(),
        }
    }
}

use lazy_static::lazy_static;
use std::collections::HashSet;

lazy_static! {
    /// 拼音表（不带声调）
    static ref PINYIN_SET: HashSet<&'static str> = {
        let items = vec![
            "a","ai","an","ang","ao","ba","bai","ban","bang","bao","bei","ben","beng","bi","bian",
            "biao","bie","bin","bing","bo","bu",
            "ca","cai","can","cang","cao","ce","cen","ceng","cha","chai","chan","chang","chao",
            "che","chen","cheng","chi","chong","chou","chu","chua","chuai","chuan","chuang","chui",
            "chun","chuo","ci","cong","cou","cu","cuan","cui","cun","cuo",
            "da","dai","dan","dang","dao","de","dei","den","deng","di","dia","dian","diao","die",
            "ding","diu","dong","dou","du","duan","dui","dun","duo",
            "e","en","eng","er",
            "fa","fan","fang","fei","fen","feng","fo","fou","fu",
            "ga","gai","gan","gang","gao","ge","gei","gen","geng","gong","gou","gu","gua","guai",
            "guan","guang","gui","gun","guo",
            "ha","hai","han","hang","hao","he","hei","hen","heng","hong","hou","hu","hua","huai",
            "huan","huang","hui","hun","huo",
            "ji","jia","jian","jiang","jiao","jie","jin","jing","jiong","jiu","ju","juan","jue","jun",
            "ka","kai","kan","kang","kao","ke","ken","keng","kong","kou","ku","kua","kuai","kuan",
            "kuang","kui","kun","kuo",
            "la","lai","lan","lang","lao","le","lei","leng","li","lia","lian","liang","liao","lie",
            "lin","ling","liu","long","lou","lu","lv","luan","lue","lun","luo",
            "ma","mai","man","mang","mao","me","mei","men","meng","mi","mian","miao","mie","min",
            "ming","miu","mo","mou","mu",
            "na","nai","nan","nang","nao","ne","nei","nen","neng","ni","nian","niang","niao","nie",
            "nin","ning","niu","nong","nou","nu","nv","nuan","nue","nuo",
            "o","ou",
            "pa","pai","pan","pang","pao","pei","pen","peng","pi","pian","piao","pie","pin","ping",
            "po","pou","pu",
            "qi","qia","qian","qiang","qiao","qie","qin","qing","qiong","qiu","qu","quan","que","qun",
            "ran","rang","rao","re","ren","reng","ri","rong","rou","ru","ruan","rui","run","ruo",
            "sa","sai","san","sang","sao","se","sen","seng","sha","shai","shan","shang","shao","she",
            "shen","sheng","shi","shou","shu","shua","shuai","shuan","shuang","shui","shun","shuo",
            "si","song","sou","su","suan","sui","sun","suo",
            "ta","tai","tan","tang","tao","te","teng","ti","tian","tiao","tie","ting","tong","tou",
            "tu","tuan","tui","tun","tuo",
            "wa","wai","wan","wang","wei","wen","weng","wo","wu",
            "xi","xia","xian","xiang","xiao","xie","xin","xing","xiong","xiu","xu","xuan","xue","xun",
            "ya","yan","yang","yao","ye","yi","yin","ying","yo","yong","you","yu","yuan","yue","yun",
            "za","zai","zan","zang","zao","ze","zei","zen","zeng","zha","zhai","zhan","zhang","zhao",
            "zhe","zhen","zheng","zhi","zhong","zhou","zhu","zhua","zhuai","zhuan","zhuang","zhui",
            "zhun","zhuo","zi","zong","zou","zu","zuan","zui","zun","zuo"
        ];
        items.into_iter().collect()
    };
}

pub struct PinyinOptions {
    pub allow_prefix: bool,
    pub allow_initials: bool,
}

/// 输入不带空格拼音 → 输出 WCDB 可用 token 列表
pub fn generate_pinyin_tokens(input: &str, options: &PinyinOptions) -> Vec<String> {
    let s = input.to_lowercase();
    let chars: Vec<char> = s.chars().collect();
    let n = chars.len();
    let mut result = Vec::new();
    let mut i = 0;

    while i < n {
        let mut found = None;
        // 最大匹配 6 个字母
        for len in (1..=6).rev() {
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
    use crate::base::pinyin_object::{generate_pinyin_tokens, PinyinOptions};

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
        assert_eq!(token_vec.join(" "), "z h o n");

        let input = "bj"; // 北京缩写
        let token_vec = generate_pinyin_tokens(input, &options);
        assert_eq!(token_vec.join(" "), "b j");
    }
}
