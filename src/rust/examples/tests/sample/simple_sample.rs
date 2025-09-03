#[cfg(test)]
pub mod simple_sample {
    use crate::base::random_tool::RandomTool;
    use crate::base::test_object::{DbTestObject, TestObject, DB_TEST_OBJECT_INSTANCE};
    use wcdb::core::database::Database;
    use wcdb::core::handle::Handle;
    use wcdb::core::handle_operation::HandleOperationTrait;
    use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb::core::table_orm_operation::TableORMOperationTrait;
    use wcdb::winq::ordering_term::Order;

    #[test]
    pub fn sample() {
        let database = Database::new("./tests/sample/demoDatabase.sqlite3", None);
        //  database.setCipherKey("abc".getBytes(), 4096, Database.CipherVersion.version4);
        //  database.setConfig("自定义配置名", new Database.Config() {
        //             @Override
        //             public void onInvocation(@NotNull Handle handle) thows WCDBException {
        //                 // Pragma secure_delete = true
        //                 handle.execute(new StatementPragma().pragma(Pragma.secureDelete).toValue(true));
        //             }
        //         });
        // 建表，不用判断表是否存在，底下会判断
        database
            .create_table("testTable", &*DB_TEST_OBJECT_INSTANCE)
            .unwrap();
        let table = database.get_table("testTable", &*DB_TEST_OBJECT_INSTANCE);

        let test_table = TestObject::new(String::from("abc"));
        table
            .insert_object(test_table, Some(DbTestObject::all_fields()))
            .unwrap();
        let mut messages = Vec::new();
        for x in 0..100 {
            let test_table = TestObject::new(RandomTool::string_by_length(x));
            messages.push(test_table);
        }
        // 批量插入，自动开事务
        table
            .insert_objects(messages, Some(DbTestObject::all_fields()))
            .unwrap();
        let test_table = TestObject::new(String::from("updateContent"));

        // 更新，可以用一个数据、一行数据、一个对象为单位去更新，后面还可以跟 order，limit，offset 参数
        let test_table = TestObject::create_object(200, String::from("updateContent2"));
        let id = DB_TEST_OBJECT_INSTANCE.id;
        let filed_id = unsafe { &*id };
        let content = DB_TEST_OBJECT_INSTANCE.content;
        let filed_content = unsafe { &*content };
        let express_content = filed_content.get_column().eq_string("updateContent");
        let express = filed_id.get_column().eq_long(100).and(&express_content);
        let ret = table.update_object(
            test_table,
            Some(vec![filed_id]),
            Some(express),
            None,
            None,
            None,
        );
        match ret {
            Ok(_) => {}
            Err(error) => {
                println!("update_object_by_field_expression error {:?}", error);
            }
        }

        // 删除
        let id = DB_TEST_OBJECT_INSTANCE.id;
        let filed_id = unsafe { &*id };
        let express = filed_id.get_column().lt_int(10);
        // table.delete_objects_by_expression(express).unwrap();
        let ordering_term = filed_id.get_column().order(Order::Desc);
        let ret = table.delete_objects(None, Some(ordering_term), Some(10), None);
        match ret {
            Ok(_) => {}
            Err(error) => {
                println!("delete_objects_by_order_limit error {:?}", error);
            }
        }

        // 读取
        let data = table
            .get_all_objects(Some(DbTestObject::all_fields()), None, None, None, None)
            .unwrap();
        // let id = DB_TEST_OBJECT_INSTANCE.id;
        // let filed_id = unsafe { &*id };
        // let expression = filed_id.get_column().gt_int(100);
        // table
        //     .get_all_objects_by_expression_order_limit(
        //         expression,
        //         filed_id.get_column().order(Order::Desc),
        //         10,
        //     )
        //     .unwrap();

        // 执行事务
        let ret = database.run_transaction(move |handle: &Handle| {
            let test_table = TestObject::new(String::from("run_transaction"));
            table
                .insert_object(test_table, Some(DbTestObject::all_fields()))
                .unwrap();
            return true; //返回 false 回滚整个事务
        });
        match ret {
            Ok(_) => {}
            Err(error) => {
                println!("run_transaction-->insert_object error {:?}", error);
            }
        }
        database
            .delete_objects("testTable", None, None, None, None)
            .unwrap()
    }
}
