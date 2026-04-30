use crate::macros::fts_version::FTSVersion;
use darling::FromMeta;
use syn::LitStr;

#[derive(Debug, FromMeta, Default)]
pub struct FTSModule {
    #[darling(default)]
    version: String, // darling 无法解析枚举值 FTSVersion，故通过 String 反转为 FTSVersion
    #[darling(default)]
    tokenizer: String, // todo qixinbing 考虑如何支持 BuiltinTokenizer 内的常量
    #[darling(default)]
    tokenizer_parameters: Vec<LitStr>,
    #[darling(default)]
    external_table: String,
}

impl FTSModule {
    pub fn new() -> Self {
        FTSModule {
            version: FTSVersion::NONE.to_string(),
            tokenizer: "".to_string(),
            tokenizer_parameters: vec![],
            external_table: "".to_string(),
        }
    }

    pub fn version(&self) -> FTSVersion {
        FTSVersion::from_str(&self.version)
    }

    pub fn tokenizer(&self) -> &str {
        &self.tokenizer
    }

    pub fn tokenizer_parameters(&self) -> &Vec<LitStr> {
        &self.tokenizer_parameters
    }

    pub fn external_table(&self) -> &str {
        &self.external_table
    }
}
