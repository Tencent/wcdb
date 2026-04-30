#[cfg(test)]
pub mod pragma_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::pragma::Pragma;

    #[test]
    pub fn test() {
        WinqTool::winq_equal(&Pragma::new("testPragma"), "testPragma");
        WinqTool::winq_equal(&Pragma::application_id(), "application_id");
        WinqTool::winq_equal(&Pragma::auto_vacuum(), "auto_vacuum");
        WinqTool::winq_equal(&Pragma::automatic_index(), "automatic_index");
        WinqTool::winq_equal(&Pragma::busy_timeout(), "busy_timeout");
        WinqTool::winq_equal(&Pragma::cache_size(), "cache_size");
        WinqTool::winq_equal(&Pragma::cache_spill(), "cache_spill");
        WinqTool::winq_equal(&Pragma::case_sensitive_like(), "case_sensitive_like");
        WinqTool::winq_equal(&Pragma::cell_size_check(), "cell_size_check");
        WinqTool::winq_equal(&Pragma::checkpoint_fullfsync(), "checkpoint_fullfsync");
        WinqTool::winq_equal(&Pragma::cipher(), "cipher");
        WinqTool::winq_equal(&Pragma::cipher_add_random(), "cipher_add_random");
        WinqTool::winq_equal(
            &Pragma::cipher_default_kdf_iter(),
            "cipher_default_kdf_iter",
        );
        WinqTool::winq_equal(
            &Pragma::cipher_default_page_size(),
            "cipher_default_page_size",
        );
        WinqTool::winq_equal(
            &Pragma::cipher_default_use_hmac(),
            "cipher_default_use_hmac",
        );
        WinqTool::winq_equal(&Pragma::cipher_migrate(), "cipher_migrate");
        WinqTool::winq_equal(&Pragma::cipher_profile(), "cipher_profile");
        WinqTool::winq_equal(&Pragma::cipher_provider(), "cipher_provider");
        WinqTool::winq_equal(
            &Pragma::cipher_provider_version(),
            "cipher_provider_version",
        );
        WinqTool::winq_equal(&Pragma::cipher_use_hmac(), "cipher_use_hmac");
        WinqTool::winq_equal(&Pragma::cipher_version(), "cipher_version");
        WinqTool::winq_equal(&Pragma::cipher_page_size(), "cipher_page_size");
        WinqTool::winq_equal(&Pragma::collation_list(), "collation_list");
        WinqTool::winq_equal(&Pragma::data_version(), "data_version");
        WinqTool::winq_equal(&Pragma::database_list(), "database_list");
        WinqTool::winq_equal(&Pragma::defer_foreign_keys(), "defer_foreign_keys");
        WinqTool::winq_equal(&Pragma::encoding(), "encoding");
        WinqTool::winq_equal(&Pragma::foreign_key_check(), "foreign_key_check");
        WinqTool::winq_equal(&Pragma::foreign_key_list(), "foreign_key_list");
        WinqTool::winq_equal(&Pragma::foreign_keys(), "foreign_keys");
        WinqTool::winq_equal(&Pragma::freelist_count(), "freelist_count");
        WinqTool::winq_equal(&Pragma::fullfsync(), "fullfsync");
        WinqTool::winq_equal(&Pragma::function_list(), "function_list");
        WinqTool::winq_equal(
            &Pragma::ignore_check_constraints(),
            "ignore_check_constraints",
        );
        WinqTool::winq_equal(&Pragma::incremental_vacuum(), "incremental_vacuum");
        WinqTool::winq_equal(&Pragma::index_info(), "index_info");
        WinqTool::winq_equal(&Pragma::index_list(), "index_list");
        WinqTool::winq_equal(&Pragma::index_x_info(), "index_xinfo");
        WinqTool::winq_equal(&Pragma::integrity_check(), "integrity_check");
        WinqTool::winq_equal(&Pragma::journal_mode(), "journal_mode");
        WinqTool::winq_equal(&Pragma::journal_size_limit(), "journal_size_limit");
        WinqTool::winq_equal(&Pragma::key(), "key");
        WinqTool::winq_equal(&Pragma::kdf_iter(), "kdf_iter");
        WinqTool::winq_equal(&Pragma::legacy_file_format(), "legacy_file_format");
        WinqTool::winq_equal(&Pragma::locking_mode(), "locking_mode");
        WinqTool::winq_equal(&Pragma::max_page_count(), "max_page_count");
        WinqTool::winq_equal(&Pragma::mmap_size(), "mmap_size");
        WinqTool::winq_equal(&Pragma::module_list(), "module_list");
        WinqTool::winq_equal(&Pragma::optimize(), "optimize");
        WinqTool::winq_equal(&Pragma::page_count(), "page_count");
        WinqTool::winq_equal(&Pragma::page_size(), "page_size");
        WinqTool::winq_equal(&Pragma::parser_trace(), "parser_trace");
        WinqTool::winq_equal(&Pragma::pragma_list(), "pragma_list");
        WinqTool::winq_equal(&Pragma::query_only(), "query_only");
        WinqTool::winq_equal(&Pragma::quick_check(), "quick_check");
        WinqTool::winq_equal(&Pragma::read_uncommitted(), "read_uncommitted");
        WinqTool::winq_equal(&Pragma::recursive_triggers(), "recursive_triggers");
        WinqTool::winq_equal(&Pragma::rekey(), "rekey");
        WinqTool::winq_equal(
            &Pragma::reverse_unordered_selects(),
            "reverse_unordered_selects",
        );
        WinqTool::winq_equal(&Pragma::schema_version(), "schema_version");
        WinqTool::winq_equal(&Pragma::secure_delete(), "secure_delete");
        WinqTool::winq_equal(&Pragma::shrink_memory(), "shrink_memory");
        WinqTool::winq_equal(&Pragma::soft_heap_limit(), "soft_heap_limit");
        WinqTool::winq_equal(&Pragma::stats(), "stats");
        WinqTool::winq_equal(&Pragma::synchronous(), "synchronous");
        WinqTool::winq_equal(&Pragma::table_info(), "table_info");
        WinqTool::winq_equal(&Pragma::temp_store(), "temp_store");
        WinqTool::winq_equal(&Pragma::threads(), "threads");
        WinqTool::winq_equal(&Pragma::user_version(), "user_version");
        WinqTool::winq_equal(&Pragma::vdbe_addoptrace(), "vdbe_addoptrace");
        WinqTool::winq_equal(&Pragma::vdbe_debug(), "vdbe_debug");
        WinqTool::winq_equal(&Pragma::vdbe_listing(), "vdbe_listing");
        WinqTool::winq_equal(&Pragma::vdbe_trace(), "vdbe_trace");
        WinqTool::winq_equal(&Pragma::wal_autocheckpoint(), "wal_autocheckpoint");
        WinqTool::winq_equal(&Pragma::wal_checkpoint(), "wal_checkpoint");
        WinqTool::winq_equal(&Pragma::writable_schema(), "writable_schema");
    }
}
