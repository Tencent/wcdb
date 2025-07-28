use crate::base::cpp_object::CppObjectTrait;
use crate::winq::identifier::{CPPType, Identifier, IdentifierStaticTrait, IdentifierTrait};
use std::ffi::{c_char, c_void, CString};

extern "C" {
    fn WCDBRustPragma_create(name: *const c_char) -> *mut c_void;
}

pub struct Pragma {
    identifier: Identifier,
}

impl CppObjectTrait for Pragma {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.identifier.set_cpp_obj(cpp_obj);
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.identifier.release_cpp_object()
    }
}

impl IdentifierTrait for Pragma {
    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierStaticTrait for Pragma {
    fn get_type() -> i32 {
        CPPType::Pragma as i32
    }
}

impl Pragma {
    pub fn new(name: &str) -> Self {
        let c_name = CString::new(name).unwrap_or_default();
        let cpp_obj = unsafe { WCDBRustPragma_create(c_name.as_ptr()) };
        Pragma {
            identifier: Identifier::new_with_obj(cpp_obj),
        }
    }

    pub fn application_id() -> Self {
        Pragma::new("application_id")
    }
    pub fn auto_vacuum() -> Self {
        Pragma::new("auto_vacuum")
    }
    pub fn automatic_index() -> Self {
        Pragma::new("automatic_index")
    }
    pub fn busy_timeout() -> Self {
        Pragma::new("busy_timeout")
    }
    pub fn cache_size() -> Self {
        Pragma::new("cache_size")
    }
    pub fn cache_spill() -> Self {
        Pragma::new("cache_spill")
    }
    pub fn case_sensitive_like() -> Self {
        Pragma::new("case_sensitive_like")
    }
    pub fn cell_size_check() -> Self {
        Pragma::new("cell_size_check")
    }
    pub fn checkpoint_fullfsync() -> Self {
        Pragma::new("checkpoint_fullfsync")
    }
    pub fn function_list() -> Self {
        Pragma::new("function_list")
    }
    pub fn cipher() -> Self {
        Pragma::new("cipher")
    }
    pub fn cipher_add_random() -> Self {
        Pragma::new("cipher_add_random")
    }
    pub fn cipher_default_kdf_iter() -> Self {
        Pragma::new("cipher_default_kdf_iter")
    }
    pub fn cipher_default_page_size() -> Self {
        Pragma::new("cipher_default_page_size")
    }
    pub fn cipher_default_use_hmac() -> Self {
        Pragma::new("cipher_default_use_hmac")
    }
    pub fn cipher_migrate() -> Self {
        Pragma::new("cipher_migrate")
    }
    pub fn cipher_profile() -> Self {
        Pragma::new("cipher_profile")
    }
    pub fn cipher_provider() -> Self {
        Pragma::new("cipher_provider")
    }
    pub fn cipher_provider_version() -> Self {
        Pragma::new("cipher_provider_version")
    }
    pub fn cipher_use_hmac() -> Self {
        Pragma::new("cipher_use_hmac")
    }
    pub fn cipher_version() -> Self {
        Pragma::new("cipher_version")
    }
    pub fn cipher_page_size() -> Self {
        Pragma::new("cipher_page_size")
    }
    pub fn collation_list() -> Self {
        Pragma::new("collation_list")
    }
    pub fn compile_options() -> Self {
        Pragma::new("compile_options")
    }
    pub fn count_changes() -> Self {
        Pragma::new("count_changes")
    }
    pub fn data_store_directory() -> Self {
        Pragma::new("data_store_directory")
    }
    pub fn data_version() -> Self {
        Pragma::new("data_version")
    }
    pub fn database_list() -> Self {
        Pragma::new("database_list")
    }
    pub fn default_cache_size() -> Self {
        Pragma::new("default_cache_size")
    }
    pub fn defer_foreign_keys() -> Self {
        Pragma::new("defer_foreign_keys")
    }
    pub fn empty_result_callbacks() -> Self {
        Pragma::new("empty_result_callbacks")
    }
    pub fn encoding() -> Self {
        Pragma::new("encoding")
    }
    pub fn foreign_key_check() -> Self {
        Pragma::new("foreign_key_check")
    }
    pub fn foreign_key_list() -> Self {
        Pragma::new("foreign_key_list")
    }
    pub fn foreign_keys() -> Self {
        Pragma::new("foreign_keys")
    }
    pub fn freelist_count() -> Self {
        Pragma::new("freelist_count")
    }
    pub fn full_column_names() -> Self {
        Pragma::new("full_column_names")
    }
    pub fn fullfsync() -> Self {
        Pragma::new("fullfsync")
    }
    pub fn ignore_check_constraints() -> Self {
        Pragma::new("ignore_check_constraints")
    }
    pub fn incremental_vacuum() -> Self {
        Pragma::new("incremental_vacuum")
    }
    pub fn index_info() -> Self {
        Pragma::new("index_info")
    }
    pub fn index_list() -> Self {
        Pragma::new("index_list")
    }
    pub fn index_x_info() -> Self {
        Pragma::new("index_xinfo")
    }
    pub fn integrity_check() -> Self {
        Pragma::new("integrity_check")
    }
    pub fn journal_mode() -> Self {
        Pragma::new("journal_mode")
    }
    pub fn journal_size_limit() -> Self {
        Pragma::new("journal_size_limit")
    }
    pub fn key() -> Self {
        Pragma::new("key")
    }
    pub fn kdf_iter() -> Self {
        Pragma::new("kdf_iter")
    }
    pub fn legacy_file_format() -> Self {
        Pragma::new("legacy_file_format")
    }
    pub fn locking_mode() -> Self {
        Pragma::new("locking_mode")
    }
    pub fn max_page_count() -> Self {
        Pragma::new("max_page_count")
    }
    pub fn mmap_size() -> Self {
        Pragma::new("mmap_size")
    }
    pub fn module_list() -> Self {
        Pragma::new("module_list")
    }
    pub fn optimize() -> Self {
        Pragma::new("optimize")
    }
    pub fn page_count() -> Self {
        Pragma::new("page_count")
    }
    pub fn page_size() -> Self {
        Pragma::new("page_size")
    }
    pub fn parser_trace() -> Self {
        Pragma::new("parser_trace")
    }
    pub fn pragma_list() -> Self {
        Pragma::new("pragma_list")
    }
    pub fn query_only() -> Self {
        Pragma::new("query_only")
    }
    pub fn quick_check() -> Self {
        Pragma::new("quick_check")
    }
    pub fn read_uncommitted() -> Self {
        Pragma::new("read_uncommitted")
    }
    pub fn recursive_triggers() -> Self {
        Pragma::new("recursive_triggers")
    }
    pub fn rekey() -> Self {
        Pragma::new("rekey")
    }
    pub fn reverse_unordered_selects() -> Self {
        Pragma::new("reverse_unordered_selects")
    }
    pub fn schema_version() -> Self {
        Pragma::new("schema_version")
    }
    pub fn secure_delete() -> Self {
        Pragma::new("secure_delete")
    }
    pub fn short_column_names() -> Self {
        Pragma::new("short_column_names")
    }
    pub fn shrink_memory() -> Self {
        Pragma::new("shrink_memory")
    }
    pub fn soft_heap_limit() -> Self {
        Pragma::new("soft_heap_limit")
    }
    pub fn stats() -> Self {
        Pragma::new("stats")
    }
    pub fn synchronous() -> Self {
        Pragma::new("synchronous")
    }
    pub fn table_info() -> Self {
        Pragma::new("table_info")
    }
    pub fn temp_store() -> Self {
        Pragma::new("temp_store")
    }
    pub fn temp_store_directory() -> Self {
        Pragma::new("temp_store_directory")
    }
    pub fn threads() -> Self {
        Pragma::new("threads")
    }
    pub fn user_version() -> Self {
        Pragma::new("user_version")
    }
    pub fn vdbe_addoptrace() -> Self {
        Pragma::new("vdbe_addoptrace")
    }
    pub fn vdbe_debug() -> Self {
        Pragma::new("vdbe_debug")
    }
    pub fn vdbe_listing() -> Self {
        Pragma::new("vdbe_listing")
    }
    pub fn vdbe_trace() -> Self {
        Pragma::new("vdbe_trace")
    }
    pub fn wal_autocheckpoint() -> Self {
        Pragma::new("wal_autocheckpoint")
    }
    pub fn wal_checkpoint() -> Self {
        Pragma::new("wal_checkpoint")
    }
    pub fn writable_schema() -> Self {
        Pragma::new("writable_schema")
    }
}
