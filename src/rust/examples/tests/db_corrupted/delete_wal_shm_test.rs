use crate::db_corrupted::corrupted_base_test_case::CorruptedBaseTestCase;
use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
use wcdb::core::table_orm_operation::TableORMOperationTrait;

struct DeleteWalTest {
    test_case: CorruptedBaseTestCase,
}

impl DeleteWalTest {
    pub fn new(db_name: &str, auto_backup: bool) -> DeleteWalTest {
        let test_case = CorruptedBaseTestCase::new(db_name, auto_backup);
        DeleteWalTest { test_case }
    }

    pub fn test_case(&self) -> &CorruptedBaseTestCase {
        &self.test_case
    }

    pub fn delete_wal_shm(&self) {
        let db_name = self.test_case.db_name();
        let path = format!("./target/tmp/{}-wal", db_name);
        if std::path::Path::new(path.as_str()).exists() {
            std::fs::remove_file(path.as_str()).unwrap();
        }

        let path = format!("./target/tmp/{}-shm", db_name);
        if std::path::Path::new(path.as_str()).exists() {
            std::fs::remove_file(path.as_str()).unwrap();
        }
    }
}

#[cfg(test)]
pub mod delete_wal_shm_exception_test {
    use crate::db_corrupted::delete_wal_shm_test::DeleteWalTest;
    use wcdb::base::wcdb_exception::WCDBException;
    use wcdb::core::handle_orm_operation::HandleORMOperationTrait;

    // 测试删除 wal 文件的效果
    // #[test] //todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_check_delete_wal() {
        // 只删除 wal 文件
        // 报错 1 no such table: main.table_goods_object
        // 报错 2 file unlinked while open: /path/to/wcdb_rust/target/tmp/db_for_delete_wal.db
        // 表现：数据库可以正常的插入、查询数据，但是无法被数据库软件打开
        let db_name = "db_for_check_delete_wal.db";
        let delete_wal_test = DeleteWalTest::new(db_name, false);
        let table_name = delete_wal_test.test_case().table_name().clone();
        let data_num = 100;

        let ret = delete_wal_test.test_case().database().trace_exception(Some(
            move |exception: WCDBException| {
                let msg = exception.message();
                // println!("trace_exception: {}",msg);

                // 检测报错 1
                if msg.starts_with("no such table") {
                    // "no such table: main.table_goods_object"
                    let expect_msg = format!("no such table: main.{}", table_name);
                    assert_eq!(msg, expect_msg);
                    return;
                }

                // 检测报错 2
                if msg.starts_with("file unlinked while open") {
                    assert!(true);
                    return;
                }

                // 遇到其他错误则测试失败
                assert!(false);
            },
        ));
        assert!(ret.is_ok());

        delete_wal_test.test_case().setup();
        delete_wal_test.test_case().insert_objects(data_num);

        delete_wal_test.test_case().database().backup().unwrap();

        // delete_wal_test.delete_wal();
        // 验证可以正常插入和查询数据
        delete_wal_test.test_case().insert_objects(100);
        assert_eq!(delete_wal_test.test_case().get_all_objects().len(), 200);

        delete_wal_test.test_case().teardown(true);
    }

    // warning: 本用例手动执行两次做完整的测试
    // 备份后之后删除 wal，打开数据库后恢复。结论：表可以恢复，但是数据没了
    // 第一次运行，没有备份，用例直接跳过
    // 第二次运行，存在备份了，再做修复逻辑
    // #[test] //todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_retrieve_delete_wal_empty_data() {
        // 当前进程结束之后，wcdb 才能生成备份文件，如果第一次打开就主动损坏，那就没有备份文件，就无法恢复数据库
        // 使用之后第二次 打开，才能恢复数据

        // 开启自动备份
        let db_name = "db_for_retrieve_delete_wal_empty_data.db";
        let delete_wal_test = DeleteWalTest::new(db_name, true);
        let table_name = delete_wal_test.test_case().table_name().clone();
        let has_back_up = delete_wal_test.test_case().has_back_up();
        let data_num = 100;

        let ret = delete_wal_test.test_case().database().trace_exception(Some(
            move |exception: WCDBException| {
                let msg = exception.message();
                println!("trace_exception: {}", msg);

                // 恢复数据报错
                // trace_exception: Acquired page number: 4 exceeds the page count: 1.
                // trace_exception: Acquired page number: 5 exceeds the page count: 1.
                // trace_exception: Acquired page number: 6 exceeds the page count: 1.
            },
        ));
        assert!(ret.is_ok());

        delete_wal_test.test_case().setup();
        delete_wal_test.test_case().insert_objects(data_num);

        if !has_back_up {
            // 没备份，只插入了 100 个数据
            assert_eq!(
                delete_wal_test.test_case().get_all_objects().len() as i32,
                data_num
            );
            delete_wal_test.test_case().database().backup().unwrap();
        } else {
            delete_wal_test.delete_wal_shm();

            // 主动删除 wal 之后验证可以正常插入和查询数据
            delete_wal_test.test_case().insert_objects(data_num);
            assert!(delete_wal_test.test_case().get_all_objects().len() > 0);

            delete_wal_test
                .test_case()
                .database()
                .retrieve(Some(move |percentage: f64, increment: f64| {
                    println!(
                        "Database retrieve percentage:{} , increment:{}",
                        percentage, increment
                    );
                    if percentage >= 1.0 {
                        println!("Database retrieve complete");
                    }
                    true
                }))
                .unwrap();

            // 恢复成功后，所有数据都没了，重新插入并验证数量
            delete_wal_test.test_case().insert_objects(data_num);
            assert_eq!(
                delete_wal_test.test_case().get_all_objects().len() as i32,
                data_num
            );

            delete_wal_test.test_case().teardown(true);
        }
    }
}

pub mod delete_wal_shm_success_test {
    use crate::db_corrupted::delete_wal_shm_test::DeleteWalTest;
    use wcdb::base::wcdb_exception::WCDBException;

    // 手动回写用例连续调用两次做完整的测试
    // 第一次写入数据，并手动回写 wal 文件
    // 第二次删除 wal，恢复数据库，并验证之前的数据恢复
    // #[test] //todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_retrieve_delete_wal_manual_success() {
        let db_name = "db_for_retrieve_delete_wal_manual_success.db";
        let delete_wal_test = DeleteWalTest::new(db_name, true);
        let has_back_up = delete_wal_test.test_case().has_back_up();
        let data_num = 100;

        let ret = delete_wal_test.test_case().database().trace_exception(Some(
            move |exception: WCDBException| {
                let msg = exception.message();
                println!("trace_exception: {}", msg);
            },
        ));
        assert!(ret.is_ok());

        if !has_back_up {
            // 第一次插入 100 个数据
            delete_wal_test.test_case().setup();
            delete_wal_test.test_case().insert_objects(data_num);
            // 主动备份
            delete_wal_test.test_case().database().backup().unwrap();
            assert_eq!(
                delete_wal_test.test_case().get_all_objects().len() as i32,
                data_num
            );
            // 把 wal shm 数据写回 db 文件，确保删除 wal shm 可以正常恢复数据
            // 如果 wal 文件过大，会导致，回写会有性能问题，需要设计回写策略
            let sql = "PRAGMA wal_checkpoint(FULL);";
            delete_wal_test
                .test_case()
                .database()
                .execute_sql(sql)
                .unwrap();
        } else {
            delete_wal_test.delete_wal_shm();
            delete_wal_test
                .test_case()
                .database()
                .retrieve(Some(move |percentage: f64, increment: f64| {
                    println!(
                        "Database retrieve percentage:{} , increment:{}",
                        percentage, increment
                    );
                    if percentage >= 1.0 {
                        println!("Database retrieve complete");
                    }
                    true
                }))
                .unwrap();

            // 恢复后再插入 100 个数据，检查是否为 200 个
            delete_wal_test.test_case().insert_objects(data_num);
            assert_eq!(
                delete_wal_test.test_case().get_all_objects().len() as i32,
                data_num * 2
            );

            delete_wal_test.test_case().teardown(true);
        }
    }

    // wal shm 回写主库之后，不管有没有开启备份，删除 wal shm，都可以正常恢复数据库
    // 设置自动回写之后，超过 page_size 的 wal shm 文件，会自动回写主库，这部分数据不会丢
    // 低于 page_size 的数据是存在 wal shm 文件中，需要手动回写主库，才能恢复数据，不会写则会丢失
    // 第一次写入数据并关闭
    // 第二次删除 wal，恢复数据库，并验证之前的数据恢复
    // #[test] //todo qixinbing: 本地运行正常，ci 运行卡死，原因待查
    pub fn test_retrieve_delete_wal_write_back_success() {
        let db_name = "db_for_retrieve_delete_wal_page_size_success.db";
        let delete_wal_test = DeleteWalTest::new(db_name, false);

        // 100 * 4k = 400k，即超过 400k 的 wal shm 文件，会自动回写主库
        // 默认应该设置为 1000，可以根据设备硬件动态调整，比如低端硬件可以降低 page_size
        let page_size = 100;
        let sql = format!("PRAGMA wal_autocheckpoint={};", page_size);
        delete_wal_test
            .test_case()
            .database()
            .execute_sql(sql.as_str())
            .unwrap();

        let data_num = 1000;
        delete_wal_test.test_case().setup();
        delete_wal_test.test_case().insert_objects(data_num);

        // 第一次只插入 1000 个数据
        if delete_wal_test.test_case().get_all_objects().len() <= (data_num as usize) {
            return;
        }

        println!(
            "database retrieve count before {}",
            delete_wal_test.test_case().get_all_objects().len()
        );

        // 第二次运行手动删除 wal shm 文件，并验证是否恢复成功
        delete_wal_test.delete_wal_shm();
        delete_wal_test
            .test_case()
            .database()
            .retrieve(Some(move |percentage: f64, increment: f64| {
                println!(
                    "Database retrieve percentage:{} , increment:{}",
                    percentage, increment
                );
                if percentage >= 1.0 {
                    println!("Database retrieve complete");
                }
                true
            }))
            .unwrap();

        delete_wal_test.test_case().insert_objects(data_num);
        println!(
            "database retrieve count after {}",
            delete_wal_test.test_case().get_all_objects().len()
        );
        assert_eq!(
            delete_wal_test.test_case().get_all_objects().len() as i32,
            data_num * 2
        );

        delete_wal_test.test_case().teardown(true);
    }
}
