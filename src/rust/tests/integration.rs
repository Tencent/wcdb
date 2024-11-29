#[cfg(test)]
pub mod test_main {
    use std::ops::{Deref, DerefMut};

    use wcdb_rust::core::database::Database;

    struct TestTableBinding;

    // impl TableBinding for TestTableBinding {
    //     fn base_binding(&self) -> Binding {
    //         todo!()
    //     }
    // }

    #[test]
    fn open_db() {
        let db = Database::new("test.db");
        let table = TestTableBinding {};
        // db.create_table("test_table", table);

        use std::ffi::c_void;

        struct CppObject {
            cpp_obj: *mut c_void,
        }

        impl Deref for CppObject {
            type Target = *mut c_void;

            fn deref(&self) -> &Self::Target {
                &self.cpp_obj
            }
        }

        impl DerefMut for CppObject {
            fn deref_mut(&mut self) -> &mut Self::Target {
                &mut self.cpp_obj
            }
        }

        // // Simulate memory address and data
        // let mut data1 = 42;
        // let mut data2 = 100;
        //
        // let mut cpp_object = CppObject {
        //     cpp_obj: &mut data1 as *mut _ as *mut c_void,
        // };
        //
        // println!("Initial data: {}", unsafe { *(cpp_object.cpp_obj as *mut i32) });
        // println!("Initial pointer address: {:?}", cpp_object.cpp_obj);
        //
        // // Reassign the pointer to a new address
        // let new_value: *mut c_void = &mut data2 as *mut _ as *mut c_void;
        // *cpp_object = new_value;
        //
        // println!("Updated data: {}", unsafe { *(cpp_object.cpp_obj as *mut i32) });
        // println!("Updated pointer address: {:?}", cpp_object.cpp_obj);
    }
}
