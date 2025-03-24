use crate::macros::fts_version::FTSVersion;
use darling::FromMeta;
use syn::LitStr;

#[derive(Debug, FromMeta)]
pub struct FTSModule {
    #[darling(default)]
    version: FTSVersion,
    #[darling(default)]
    tokenizer: String,
    #[darling(default)]
    tokenizer_parameters: Vec<LitStr>,
    #[darling(default)]
    external_table: Vec<LitStr>,
}

impl FTSModule {
    pub fn new() -> Self {
        FTSModule {
            version: FTSVersion::NONE,
            tokenizer: "".to_string(),
            tokenizer_parameters: vec![],
            external_table: vec![],
        }
    }

    pub fn version(&self) -> &FTSVersion {
        &self.version
    }

    pub fn tokenizer(&self) -> &str {
        &self.tokenizer
    }
}
