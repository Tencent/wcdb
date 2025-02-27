use crate::compiler::resolved_info::fts_module_info::FTSModuleInfo;
use crate::compiler::resolved_info::multi_indexes_info::MultiIndexesInfo;
use crate::compiler::resolved_info::multi_primary_info::MultiPrimaryInfo;
use crate::compiler::resolved_info::multi_unique_info::MultiUniqueInfo;
use crate::WCDBTable;

pub struct TableConfigInfo {
    multi_indexes: Option<Vec<MultiIndexesInfo>>,
    multi_primaries: Option<Vec<MultiPrimaryInfo>>,
    multi_unique: Option<Vec<MultiUniqueInfo>>,
    fts_module: Option<FTSModuleInfo>,
    is_without_row_id: bool,
}

impl TableConfigInfo {
    pub fn new() -> Self {
        TableConfigInfo {
            multi_indexes: None,
            multi_primaries: None,
            multi_unique: None,
            fts_module: None,
            is_without_row_id: false,
        }
    }

    pub fn resolve(table: &WCDBTable, fts_module_opt: Option<FTSModuleInfo>) -> TableConfigInfo {
        let mut resolved_annotation = TableConfigInfo::new();
        resolved_annotation.is_without_row_id = table.is_without_row_id();
        // todo dengxudong 是否去掉？
        // for multi_indexes_item in table.multi_indexes() {
        //     resolved_annotation
        //         .multi_indexes
        //         .get_or_insert(vec![])
        //         .push(MultiIndexesInfo::resolve(&multi_indexes_item));
        // }
        for multi_primary in table.get_multi_primary_vec() {
            resolved_annotation
                .multi_primaries
                .get_or_insert(vec![])
                .push(MultiPrimaryInfo::resolve(&multi_primary));
        }
        for x in table.get_multi_unique_vec() {
            resolved_annotation
                .multi_unique
                .get_or_insert(vec![])
                .push(MultiUniqueInfo::resolve(&x))
        }
        // todo dengxudong fts 逻辑补全
        // resolved_annotation.fts_module = fts_module_opt;
        resolved_annotation
    }

    pub fn multi_indexes(&self) -> &Option<Vec<MultiIndexesInfo>> {
        &self.multi_indexes
    }

    pub fn multi_primaries(&self) -> &Option<Vec<MultiPrimaryInfo>> {
        &self.multi_primaries
    }

    pub fn multi_unique(&self) -> &Option<Vec<MultiUniqueInfo>> {
        &self.multi_unique
    }

    pub fn fts_module(&self) -> &Option<FTSModuleInfo> {
        &self.fts_module
    }

    pub fn is_without_row_id(&self) -> bool {
        self.is_without_row_id
    }
}
