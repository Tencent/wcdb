use darling::FromMeta;

#[repr(i32)]
#[derive(Debug, Default, FromMeta, PartialEq)]
pub enum FTSVersion {
    #[default]
    NONE,
    FTS3,
    FTS4,
    FTS5,
}

impl FTSVersion {
    pub fn to_string(&self) -> String {
        match self {
            FTSVersion::NONE => "NONE".to_string(),
            FTSVersion::FTS3 => "FTS3".to_string(),
            FTSVersion::FTS4 => "FTS4".to_string(),
            FTSVersion::FTS5 => "FTS5".to_string(),
        }
    }

    pub fn from_str(s: &str) -> Self {
        match s {
            "NONE" => FTSVersion::NONE,
            "FTS3" => FTSVersion::FTS3,
            "FTS4" => FTSVersion::FTS4,
            "FTS5" => FTSVersion::FTS5,
            _ => panic!(
                "Invalid FTS version {} : only support NONE/FTS3/FTS4/FTS5 ",
                s
            ),
        }
    }
}
