use crate::base::cpp_object::CppObjectTrait;
use crate::base::wcdb_exception::{ExceptionCode, ExceptionLevel, WCDBException, WCDBResult};
use crate::winq::identifier::Identifier;
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

impl Pragma {
    pub fn new(name: &str) -> WCDBResult<Self> {
        let c_name = CString::new(name);
        match c_name {
            Ok(name) => {
                let cpp_obj = unsafe { WCDBRustPragma_create(name.into_raw()) };
                Ok(Pragma {
                    identifier: Identifier::new_with_obj(cpp_obj),
                })
            }
            Err(error) => Err(WCDBException::new_with_message(
                ExceptionLevel::Error,
                ExceptionCode::Error,
                error.to_string(),
            )),
        }
    }

    pub fn application_id() -> WCDBResult<Self> {
        Pragma::new("application_id")
    }
    pub fn auto_vacuum() -> WCDBResult<Self> {
        Pragma::new("auto_vacuum")
    }
    pub fn automatic_index() -> WCDBResult<Self> {
        Pragma::new("automatic_index")
    }
    pub fn busy_timeout() -> WCDBResult<Self> {
        Pragma::new("busy_timeout")
    }
    pub fn cache_size() -> WCDBResult<Self> {
        Pragma::new("cache_size")
    }
    pub fn cache_spill() -> WCDBResult<Self> {
        Pragma::new("cache_spill")
    }
    pub fn case_sensitive_like() -> WCDBResult<Self> {
        Pragma::new("case_sensitive_like")
    }
    pub fn cell_size_check() -> WCDBResult<Self> {
        Pragma::new("cell_size_check")
    }
    pub fn checkpoint_fullfsync() -> WCDBResult<Self> {
        Pragma::new("checkpoint_fullfsync")
    }
    pub fn function_list() -> WCDBResult<Self> {
        Pragma::new("function_list")
    }
    pub fn cipher() -> WCDBResult<Self> {
        Pragma::new("cipher")
    }
    pub fn cipher_add_random() -> WCDBResult<Self> {
        Pragma::new("cipher_add_random")
    }
    pub fn cipher_default_kdf_iter() -> WCDBResult<Self> {
        Pragma::new("cipher_default_kdf_iter")
    }
    pub fn cipher_default_page_size() -> WCDBResult<Self> {
        Pragma::new("cipher_default_page_size")
    }
    pub fn cipher_default_use_hmac() -> WCDBResult<Self> {
        Pragma::new("cipher_default_use_hmac")
    }
    pub fn cipher_migrate() -> WCDBResult<Self> {
        Pragma::new("cipher_migrate")
    }
    pub fn cipher_profile() -> WCDBResult<Self> {
        Pragma::new("cipher_profile")
    }
    pub fn cipher_provider() -> WCDBResult<Self> {
        Pragma::new("cipher_provider")
    }
    pub fn cipher_provider_version() -> WCDBResult<Self> {
        Pragma::new("cipher_provider_version")
    }
    pub fn cipher_use_hmac() -> WCDBResult<Self> {
        Pragma::new("cipher_use_hmac")
    }
    pub fn cipher_version() -> WCDBResult<Self> {
        Pragma::new("cipher_version")
    }
    pub fn cipher_page_size() -> WCDBResult<Self> {
        Pragma::new("cipher_page_size")
    }
    pub fn collation_list() -> WCDBResult<Self> {
        Pragma::new("collation_list")
    }
    pub fn compile_options() -> WCDBResult<Self> {
        Pragma::new("compile_options")
    }
    pub fn count_changes() -> WCDBResult<Self> {
        Pragma::new("count_changes")
    }
    pub fn data_store_directory() -> WCDBResult<Self> {
        Pragma::new("data_store_directory")
    }
    pub fn data_version() -> WCDBResult<Self> {
        Pragma::new("data_version")
    }
    pub fn database_list() -> WCDBResult<Self> {
        Pragma::new("database_list")
    }
    pub fn default_cache_size() -> WCDBResult<Self> {
        Pragma::new("default_cache_size")
    }
    pub fn defer_foreign_keys() -> WCDBResult<Self> {
        Pragma::new("defer_foreign_keys")
    }
    pub fn empty_result_callbacks() -> WCDBResult<Self> {
        Pragma::new("empty_result_callbacks")
    }
    pub fn encoding() -> WCDBResult<Self> {
        Pragma::new("encoding")
    }
    pub fn foreign_key_check() -> WCDBResult<Self> {
        Pragma::new("foreign_key_check")
    }
    pub fn foreign_key_list() -> WCDBResult<Self> {
        Pragma::new("foreign_key_list")
    }
    pub fn foreign_keys() -> WCDBResult<Self> {
        Pragma::new("foreign_keys")
    }
    pub fn freelist_count() -> WCDBResult<Self> {
        Pragma::new("freelist_count")
    }
    pub fn full_column_names() -> WCDBResult<Self> {
        Pragma::new("full_column_names")
    }
    pub fn fullfsync() -> WCDBResult<Self> {
        Pragma::new("fullfsync")
    }
    pub fn ignore_check_constraints() -> WCDBResult<Self> {
        Pragma::new("ignore_check_constraints")
    }
    pub fn incremental_vacuum() -> WCDBResult<Self> {
        Pragma::new("incremental_vacuum")
    }
    pub fn index_info() -> WCDBResult<Self> {
        Pragma::new("index_info")
    }
    pub fn index_list() -> WCDBResult<Self> {
        Pragma::new("index_list")
    }
    pub fn index_x_info() -> WCDBResult<Self> {
        Pragma::new("index_xinfo")
    }
    pub fn integrity_check() -> WCDBResult<Self> {
        Pragma::new("integrity_check")
    }
    pub fn journal_mode() -> WCDBResult<Self> {
        Pragma::new("journal_mode")
    }
    pub fn journal_size_limit() -> WCDBResult<Self> {
        Pragma::new("journal_size_limit")
    }
    pub fn key() -> WCDBResult<Self> {
        Pragma::new("key")
    }
    pub fn kdf_iter() -> WCDBResult<Self> {
        Pragma::new("kdf_iter")
    }
    pub fn legacy_file_format() -> WCDBResult<Self> {
        Pragma::new("legacy_file_format")
    }
    pub fn locking_mode() -> WCDBResult<Self> {
        Pragma::new("locking_mode")
    }
    pub fn max_page_count() -> WCDBResult<Self> {
        Pragma::new("max_page_count")
    }
    pub fn mmap_size() -> WCDBResult<Self> {
        Pragma::new("mmap_size")
    }
    pub fn module_list() -> WCDBResult<Self> {
        Pragma::new("module_list")
    }
    pub fn optimize() -> WCDBResult<Self> {
        Pragma::new("optimize")
    }
    pub fn page_count() -> WCDBResult<Self> {
        Pragma::new("page_count")
    }
    pub fn page_size() -> WCDBResult<Self> {
        Pragma::new("page_size")
    }
    pub fn parser_trace() -> WCDBResult<Self> {
        Pragma::new("parser_trace")
    }
    pub fn pragma_list() -> WCDBResult<Self> {
        Pragma::new("pragma_list")
    }
    pub fn query_only() -> WCDBResult<Self> {
        Pragma::new("query_only")
    }
    pub fn quick_check() -> WCDBResult<Self> {
        Pragma::new("quick_check")
    }
    pub fn read_uncommitted() -> WCDBResult<Self> {
        Pragma::new("read_uncommitted")
    }
    pub fn recursive_triggers() -> WCDBResult<Self> {
        Pragma::new("recursive_triggers")
    }
    pub fn rekey() -> WCDBResult<Self> {
        Pragma::new("rekey")
    }
    pub fn reverse_unordered_selects() -> WCDBResult<Self> {
        Pragma::new("reverse_unordered_selects")
    }
    pub fn schema_version() -> WCDBResult<Self> {
        Pragma::new("schema_version")
    }
    pub fn secure_delete() -> WCDBResult<Self> {
        Pragma::new("secure_delete")
    }
    pub fn short_column_names() -> WCDBResult<Self> {
        Pragma::new("short_column_names")
    }
    pub fn shrink_memory() -> WCDBResult<Self> {
        Pragma::new("shrink_memory")
    }
    pub fn soft_heap_limit() -> WCDBResult<Self> {
        Pragma::new("soft_heap_limit")
    }
    pub fn stats() -> WCDBResult<Self> {
        Pragma::new("stats")
    }
    pub fn synchronous() -> WCDBResult<Self> {
        Pragma::new("synchronous")
    }
    pub fn table_info() -> WCDBResult<Self> {
        Pragma::new("table_info")
    }
    pub fn temp_store() -> WCDBResult<Self> {
        Pragma::new("temp_store")
    }
    pub fn temp_store_directory() -> WCDBResult<Self> {
        Pragma::new("temp_store_directory")
    }
    pub fn threads() -> WCDBResult<Self> {
        Pragma::new("threads")
    }
    pub fn user_version() -> WCDBResult<Self> {
        Pragma::new("user_version")
    }
    pub fn vdbe_addoptrace() -> WCDBResult<Self> {
        Pragma::new("vdbe_addoptrace")
    }
    pub fn vdbe_debug() -> WCDBResult<Self> {
        Pragma::new("vdbe_debug")
    }
    pub fn vdbe_listing() -> WCDBResult<Self> {
        Pragma::new("vdbe_listing")
    }
    pub fn vdbe_trace() -> WCDBResult<Self> {
        Pragma::new("vdbe_trace")
    }
    pub fn wal_autocheckpoint() -> WCDBResult<Self> {
        Pragma::new("wal_autocheckpoint")
    }
    pub fn wal_checkpoint() -> WCDBResult<Self> {
        Pragma::new("wal_checkpoint")
    }
    pub fn writable_schema() -> WCDBResult<Self> {
        Pragma::new("writable_schema")
    }
}
