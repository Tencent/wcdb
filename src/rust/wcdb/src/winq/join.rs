use crate::base::cpp_object::{CppObject, CppObjectTrait};
use crate::base::cpp_object_convertible::CppObjectConvertibleTrait;
use crate::utils::ToCString;
use crate::winq::column::Column;
use crate::winq::expression::Expression;
use crate::winq::identifier::{CPPType, Identifier, IdentifierTrait};
use crate::winq::identifier_convertible::IdentifierConvertibleTrait;
use crate::winq::indexed_column_convertible::IndexedColumnConvertibleTrait;
use crate::winq::table_or_subquery_convertible_trait::TableOrSubqueryConvertibleTrait;
use core::ffi::c_size_t;
use std::ffi::{c_char, c_int, c_void, CString};

extern "C" {
    fn WCDBRustJoin_createCppObj(
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    ) -> *mut c_void;

    fn WCDBRustJoin_configWith(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithLeftOuterJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithLeftJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithInnerJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithCrossJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithNaturalJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithNaturalLeftOuterJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithNaturalLeftJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithNaturalInnerJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configWithNaturalCrossJoin(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        object: *mut c_void,
        table_name: *const c_char,
    );

    fn WCDBRustJoin_configOn(cpp_obj: *mut c_void, expression: *mut c_void);

    fn WCDBRustJoin_configUsingColumn(
        cpp_obj: *mut c_void,
        cpp_type: c_int,
        columns: *const *mut c_void,
        column_names: *const *const c_char,
        vec_len: c_size_t,
    );
}

pub struct Join {
    identifier: Identifier,
}

impl CppObjectTrait for Join {
    fn set_cpp_obj(&mut self, cpp_obj: *mut c_void) {
        self.identifier.set_cpp_obj(cpp_obj)
    }

    fn get_cpp_obj(&self) -> *mut c_void {
        self.identifier.get_cpp_obj()
    }

    fn release_cpp_object(&mut self) {
        self.identifier.release_cpp_object()
    }
}

impl CppObjectConvertibleTrait for Join {
    fn as_cpp_object(&self) -> &CppObject {
        self.identifier.as_cpp_object()
    }
}

impl IdentifierTrait for Join {
    fn get_type(&self) -> CPPType {
        self.identifier.get_type()
    }

    fn get_description(&self) -> String {
        self.identifier.get_description()
    }
}

impl IdentifierConvertibleTrait for Join {
    fn as_identifier(&self) -> &Identifier {
        self.identifier.as_identifier()
    }
}

impl TableOrSubqueryConvertibleTrait for Join {}

impl Join {
    // pub fn new_with_table_name(table_name: &str) -> Self {
    //     let cstr = table_name.to_cstring();
    //     let cpp_obj = unsafe {
    //         WCDBRustJoin_createCppObj(CPPType::String as c_int, 0 as *mut c_void, cstr.as_ptr())
    //     };
    //     Join {
    //         identifier: Identifier::new_with_obj(cpp_obj),
    //     }
    // }
    //
    // pub fn new_with_table_or_subquery_convertible<T>(table_or_subquery: &T) -> Self
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     let cpp_obj = unsafe {
    //         WCDBRustJoin_createCppObj(
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         )
    //     };
    //     Join {
    //         identifier: Identifier::new_with_obj(cpp_obj),
    //     }
    // }
    //
    // pub fn with_table_name(&self, table_name: &str) -> &Join {
    //     let cstr = table_name.to_cstring();
    //     unsafe {
    //         WCDBRustJoin_configWith(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             cstr.as_ptr(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn with_table_or_subquery_convertible<T>(&self, table_or_subquery: &T) -> &Join
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustJoin_configWith(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn join_with_table_name(&self, table_name: &str) -> &Join {
    //     let cstr = table_name.to_cstring();
    //     unsafe {
    //         WCDBRustJoin_configWithJoin(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             cstr.as_ptr(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn join_with_table_or_subquery_convertible<T>(&self, table_or_subquery: &T) -> &Join
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustJoin_configWithJoin(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn left_outer_join_with_table_name(&self, table_name: &str) -> &Join {
    //     let cstr = table_name.to_cstring();
    //     unsafe {
    //         WCDBRustJoin_configWithLeftOuterJoin(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             cstr.as_ptr(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn left_outer_join_with_table_or_subquery_convertible<T>(
    //     &self,
    //     table_or_subquery: &T,
    // ) -> &Join
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustJoin_configWithLeftOuterJoin(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn left_join_with_table_name(&self, table_name: &str) -> &Join {
    //     let cstr = table_name.to_cstring();
    //     unsafe {
    //         WCDBRustJoin_configWithLeftJoin(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             cstr.as_ptr(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn left_join_with_table_or_subquery_convertible<T>(&self, table_or_subquery: &T) -> &Join
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustJoin_configWithLeftJoin(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn inner_join_with_table_name(&self, table_name: &str) -> &Join {
    //     let cstr = table_name.to_cstring();
    //     unsafe {
    //         WCDBRustJoin_configWithInnerJoin(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             cstr.as_ptr(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn inner_join_with_table_or_subquery_convertible<T>(&self, table_or_subquery: &T) -> &Join
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustJoin_configWithInnerJoin(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn cross_join_with_table_name(&self, table_name: &str) -> &Join {
    //     let cstr = table_name.to_cstring();
    //     unsafe {
    //         WCDBRustJoin_configWithCrossJoin(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             cstr.as_ptr(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn cross_join_with_table_or_subquery_convertible<T>(&self, table_or_subquery: &T) -> &Join
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustJoin_configWithCrossJoin(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn natural_join_with_table_name(&self, table_name: &str) -> &Join {
    //     let cstr = table_name.to_cstring();
    //     unsafe {
    //         WCDBRustJoin_configWithNaturalJoin(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             cstr.as_ptr(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn natural_join_with_table_or_subquery_convertible<T>(&self, table_or_subquery: &T) -> &Join
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustJoin_configWithNaturalJoin(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn natural_left_outer_join_with_table_name(&self, table_name: &str) -> &Join {
    //     let cstr = table_name.to_cstring();
    //     unsafe {
    //         WCDBRustJoin_configWithNaturalLeftOuterJoin(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             cstr.as_ptr(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn natural_left_outer_join_with_table_or_subquery_convertible<T>(
    //     &self,
    //     table_or_subquery: &T,
    // ) -> &Join
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustJoin_configWithNaturalLeftOuterJoin(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn natural_left_join_with_table_name(&self, table_name: &str) -> &Join {
    //     let cstr = table_name.to_cstring();
    //     unsafe {
    //         WCDBRustJoin_configWithNaturalLeftJoin(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             cstr.as_ptr(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn natural_left_join_with_table_or_subquery_convertible<T>(
    //     &self,
    //     table_or_subquery: &T,
    // ) -> &Join
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustJoin_configWithNaturalLeftJoin(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn natural_inner_join_with_table_name(&self, table_name: &str) -> &Join {
    //     let cstr = table_name.to_cstring();
    //     unsafe {
    //         WCDBRustJoin_configWithNaturalInnerJoin(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             cstr.as_ptr(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn natural_inner_join_with_table_or_subquery_convertible<T>(
    //     &self,
    //     table_or_subquery: &T,
    // ) -> &Join
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustJoin_configWithNaturalInnerJoin(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn natural_cross_join_with_table_name(&self, table_name: &str) -> &Join {
    //     let cstr = table_name.to_cstring();
    //     unsafe {
    //         WCDBRustJoin_configWithNaturalCrossJoin(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             0 as *mut c_void,
    //             cstr.as_ptr(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn natural_cross_join_with_table_or_subquery_convertible<T>(
    //     &self,
    //     table_or_subquery: &T,
    // ) -> &Join
    // where
    //     T: IndexedColumnConvertibleTrait + IdentifierStaticTrait + CppObjectTrait,
    // {
    //     unsafe {
    //         WCDBRustJoin_configWithNaturalCrossJoin(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(table_or_subquery) as c_int,
    //             CppObject::get(table_or_subquery),
    //             null(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn on(&self, expression: &Expression) -> &Join {
    //     unsafe {
    //         WCDBRustJoin_configOn(self.get_cpp_obj(), CppObject::get(expression));
    //     }
    //     self
    // }
    //
    // pub fn using_with_column_name(&self, column: &str) -> &Join {
    //     let cstr = column.to_cstring();
    //     let mut vec: Vec<*const c_char> = Vec::new();
    //     vec.push(cstr.as_ptr());
    //     unsafe {
    //         WCDBRustJoin_configUsingColumn(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             null(),
    //             vec.as_ptr(),
    //             0,
    //         );
    //     }
    //     self
    // }
    //
    // pub fn using_with_column_obj(&self, column: &Column) -> &Join {
    //     let mut vec: Vec<*mut c_void> = Vec::new();
    //     vec.push(CppObject::get(column));
    //     unsafe {
    //         WCDBRustJoin_configUsingColumn(
    //             self.get_cpp_obj(),
    //             Identifier::get_cpp_type(column),
    //             vec.as_ptr(),
    //             null(),
    //             0,
    //         );
    //     }
    //     self
    // }
    //
    // pub fn using_with_column_name_vector(&self, column_vec: &Vec<String>) -> &Join {
    //     let c_strings: Vec<CString> = column_vec.iter().map(|x| x.to_cstring()).collect();
    //     let vec: Vec<*const c_char> = c_strings.iter().map(|cs| cs.as_ptr()).collect();
    //
    //     unsafe {
    //         WCDBRustJoin_configUsingColumn(
    //             self.get_cpp_obj(),
    //             CPPType::String as c_int,
    //             null(),
    //             vec.as_ptr(),
    //             vec.len(),
    //         );
    //     }
    //     self
    // }
    //
    // pub fn using_with_column_obj_vector(&self, column_vec: &Vec<Column>) -> &Join {
    //     if column_vec.is_empty() {
    //         return self;
    //     }
    //     let mut vec: Vec<*mut c_void> = Vec::new();
    //     for x in column_vec {
    //         vec.push(CppObject::get(x));
    //     }
    //     unsafe {
    //         WCDBRustJoin_configUsingColumn(
    //             self.get_cpp_obj(),
    //             CPPType::Column as c_int,
    //             vec.as_ptr(),
    //             null(),
    //             vec.len(),
    //         );
    //     }
    //     self
    // }
}
