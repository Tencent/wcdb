use crate::base::base_test_case::TestCaseTrait;
use crate::base::database_test_case::{DatabaseTestCase, Expect};
use wcdb::base::wcdb_exception::WCDBResult;

pub struct FTSTest {
    database_test_case: DatabaseTestCase,
    table_name: String,
}

impl TestCaseTrait for FTSTest {
    fn setup(&self) -> WCDBResult<()> {
        self.database_test_case.setup()?;
        self.database_test_case.set_expect_mode(Expect::SomeSQLs);
        Ok(())
    }

    fn teardown(&self) -> WCDBResult<()> {
        self.database_test_case.teardown()
    }
}

impl FTSTest {
    pub fn new() -> Self {
        Self {
            database_test_case: DatabaseTestCase::new(),
            table_name: "ftsTable".to_string(),
        }
    }
}

#[cfg(test)]
pub mod fts_test {
    use crate::base::base_test_case::TestCaseTrait;
    use crate::base::pinyin_object::{DbPinyinObject, PinyinObject, DB_PINYIN_OBJECT_INSTANCE};
    use crate::base::test_object::{TestObject, DB_TEST_OBJECT_INSTANCE};
    use crate::base::traditional_chinese_object::{
        DbTraditionalChineseObject, TraditionalChineseObject,
        DB_TRADITIONAL_CHINESE_OBJECT_INSTANCE,
    };
    use crate::crud::fts_test::FTSTest;
    use crate::orm::testclass::fts3_test_object::{
        DbFts3TestObject, Fts3TestObject, DB_FTS3_TEST_OBJECT_INSTANCE,
    };
    use crate::orm::testclass::fts5_test_object::{
        DbFts5TestObject, Fts5TestObject, DB_FTS5_TEST_OBJECT_INSTANCE,
    };
    use crate::orm::testclass::mmicu_test_object::{
        DbMmicuTestObject, MmicuTestObject, DB_MMICU_TEST_OBJECT_INSTANCE,
    };
    use std::collections::HashMap;
    use wcdb::core::database::Database;
    use wcdb::core::handle_orm_operation::HandleORMOperationTrait;
    use wcdb::core::table_orm_operation::TableORMOperationTrait;
    use wcdb::fts::builtin_tokenizer::BuiltinTokenizer;
    use wcdb::winq::expression_operable::ExpressionOperableTrait;

    fn setup(fts_test: &FTSTest) {
        fts_test.setup().unwrap();
    }

    fn teardown(fts_test: &FTSTest) {
        fts_test.teardown().unwrap();
    }

    #[test]
    fn test_fts3() {
        let fts_test = FTSTest::new();
        setup(&fts_test);

        let arc_db = fts_test.database_test_case.get_database();
        let database = arc_db.read().unwrap();

        database.add_tokenizer(BuiltinTokenizer::ONE_OR_BINARY);
        database
            .create_virtual_table(&fts_test.table_name, &*DB_FTS3_TEST_OBJECT_INSTANCE)
            .unwrap();
        let table = database.get_table(&fts_test.table_name, &*DB_FTS3_TEST_OBJECT_INSTANCE);
        let english_object = Fts3TestObject::new(1, "This is English test content");
        let chinese_object = Fts3TestObject::new(2, "这是中文测试内容");
        let numeric_object = Fts3TestObject::new(1, "123456");
        let symbolic_object = Fts3TestObject::new(1, "abc..def");

        let obj_vec = vec![
            english_object.clone(),
            chinese_object.clone(),
            numeric_object.clone(),
            symbolic_object.clone(),
        ];

        table.insert_objects(obj_vec, None).unwrap();

        // English
        let exp = DbFts3TestObject::content().match_("Engl*");
        let vec = table
            .get_all_objects(None, Some(&exp), None, None, None)
            .unwrap();
        assert_eq!(vec.len(), 1);
        assert_eq!(vec[0].content, english_object.content);

        // Chinese
        let exp = DbFts3TestObject::content().match_("中文");
        let vec = table
            .get_all_objects(None, Some(&exp), None, None, None)
            .unwrap();
        assert_eq!(vec.len(), 1);
        assert_eq!(vec[0].content, chinese_object.content);

        // Numeric
        let exp = DbFts3TestObject::content().match_("123*");
        let vec = table
            .get_all_objects(None, Some(&exp), None, None, None)
            .unwrap();
        assert_eq!(vec.len(), 1);
        assert_eq!(vec[0].content, numeric_object.content);

        // Symbolic
        let exp = DbFts3TestObject::content().match_("def*");
        let vec = table
            .get_all_objects(None, Some(&exp), None, None, None)
            .unwrap();
        assert_eq!(vec.len(), 1);
        assert_eq!(vec[0].content, symbolic_object.content);

        teardown(&fts_test);
    }

    // todo qixinbing 该用例建表失败，导致插入失败，原因待查
    // #[test]
    // fn test_mmicu() {
    //     let fts_test = FTSTest::new();
    //     setup(&fts_test);
    //
    //     let arc_db = fts_test.database_test_case.get_database();
    //     let database = arc_db.read().unwrap();
    //     let table_name = fts_test.table_name.as_str();
    //     database.add_tokenizer(BuiltinTokenizer::MMICU);
    //     database.create_virtual_table(table_name, &*DB_MMICU_TEST_OBJECT_INSTANCE).unwrap();
    //     let table = database.get_table(&fts_test.table_name, &*DB_MMICU_TEST_OBJECT_INSTANCE);
    //     let english_object = MmicuTestObject::new(1, "This is English test content");
    //     let chinese_object = MmicuTestObject::new(2, "这是中文测试内容");
    //     let numeric_object = MmicuTestObject::new(1, "123456");
    //     let symbolic_object = MmicuTestObject::new(1, "abc..def");
    //
    //     let obj_vec = vec![
    //         english_object.clone(),
    //         chinese_object.clone(),
    //         numeric_object.clone(),
    //         symbolic_object.clone(),
    //     ];
    //
    //     table.insert_objects(obj_vec, None).unwrap();
    //
    //     // English
    //     let exp = DbMmicuTestObject::content().match_("Engl*");
    //     let vec = table
    //         .get_all_objects(None, Some(&exp), None, None, None)
    //         .unwrap();
    //     assert_eq!(vec.len(), 1);
    //     assert_eq!(vec[0].content, english_object.content);
    //
    //     // Chinese
    //     let exp = DbMmicuTestObject::content().match_("中文");
    //     let vec = table
    //         .get_all_objects(None, Some(&exp), None, None, None)
    //         .unwrap();
    //     assert_eq!(vec.len(), 1);
    //     assert_eq!(vec[0].content, chinese_object.content);
    //
    //     // Numeric
    //     let exp = DbMmicuTestObject::content().match_("123*");
    //     let vec = table
    //         .get_all_objects(None, Some(&exp), None, None, None)
    //         .unwrap();
    //     assert_eq!(vec.len(), 1);
    //     assert_eq!(vec[0].content, numeric_object.content);
    //
    //     // Symbolic
    //     let exp = DbMmicuTestObject::content().match_("def*");
    //     let vec = table
    //         .get_all_objects(None, Some(&exp), None, None, None)
    //         .unwrap();
    //     assert_eq!(vec.len(), 1);
    //     assert_eq!(vec[0].content, symbolic_object.content);
    //
    //     teardown(&fts_test);
    // }

    #[test]
    fn test_fts5() {
        let fts_test = FTSTest::new();
        setup(&fts_test);
        let arc_db = fts_test.database_test_case.get_database();
        let database = arc_db.read().unwrap();
        let table_name = &fts_test.table_name;

        database.add_tokenizer(BuiltinTokenizer::VERBATIM);
        database
            .create_virtual_table(table_name, &*DB_FTS5_TEST_OBJECT_INSTANCE)
            .unwrap();
        let table = database.get_table(table_name, &*DB_FTS5_TEST_OBJECT_INSTANCE);
        let english_object = Fts5TestObject::new(1, "This is English test content");
        let chinese_object = Fts5TestObject::new(2, "这是中文测试内容");
        let numeric_object = Fts5TestObject::new(3, "123456");
        let symbolic_object = Fts5TestObject::new(4, "abc..def");

        let fts5_obj_vec = vec![
            english_object.clone(),
            chinese_object.clone(),
            numeric_object.clone(),
            symbolic_object.clone(),
        ];

        table.insert_objects(fts5_obj_vec.clone(), None).unwrap();

        // External content object
        database
            .create_table("contentTable", &*DB_TEST_OBJECT_INSTANCE)
            .unwrap();
        let content_table = database.get_table("contentTable", &*DB_TEST_OBJECT_INSTANCE);
        let mut all_content_objects = vec![];
        for object in fts5_obj_vec {
            let content_object = TestObject::create_object(object.id, object.content);
            all_content_objects.push(content_object);
        }
        content_table
            .insert_objects(all_content_objects, None)
            .unwrap();

        // English
        let exp = DbFts5TestObject::content().match_("Engl*");
        let vec = table
            .get_all_objects(None, Some(&exp), None, None, None)
            .unwrap();
        assert_eq!(vec.len(), 1);
        assert_eq!(vec[0].content, english_object.content);

        // Chinese
        let exp = DbFts5TestObject::content().match_("中文");
        let vec = table
            .get_all_objects(None, Some(&exp), None, None, None)
            .unwrap();
        assert_eq!(vec.len(), 1);
        assert_eq!(vec[0].content, chinese_object.content);

        // Numeric
        let exp = DbFts5TestObject::content().match_("123*");
        let vec = table
            .get_all_objects(None, Some(&exp), None, None, None)
            .unwrap();
        assert_eq!(vec.len(), 1);
        assert_eq!(vec[0].content, numeric_object.content);

        // Symbolic
        let exp = DbFts5TestObject::content().match_("def*");
        let vec = table
            .get_all_objects(None, Some(&exp), None, None, None)
            .unwrap();
        assert_eq!(vec.len(), 1);
        assert_eq!(vec[0].content, symbolic_object.content);

        teardown(&fts_test);
    }

    #[test]
    fn test_pinyin() {
        Database::config_pinyin_dict(&HashMap::from([
            ("单", vec!["shan", "dan", "chan"]),
            ("于", vec!["yu"]),
            ("骑", vec!["qi"]),
            ("模", vec!["mo", "mu"]),
            ("具", vec!["ju"]),
            ("车", vec!["che"]),
        ]));

        let fts_test = FTSTest::new();
        setup(&fts_test);

        let arc_db = fts_test.database_test_case.get_database();
        let database = arc_db.read().unwrap();

        database.add_tokenizer(BuiltinTokenizer::PINYIN);
        database
            .create_virtual_table(&fts_test.table_name, &*DB_PINYIN_OBJECT_INSTANCE)
            .unwrap();

        let mut obj = PinyinObject::new();
        let content = "单于骑模具单车";
        obj.content = content.to_string();
        database
            .insert_object(obj, DbPinyinObject::all_fields(), &fts_test.table_name)
            .unwrap();

        // todo 1 拼音如何加空格
        // todo 2 汉语如何分词
        // todo 3 确保端和服务的分词方案一致：同时支持中文搜索和拼音搜索
        let queries = vec![
            "\"shan yu qi mu ju dan che\"",
            "\"chan yu qi mo ju shan che\"",
            "\"dan yu qi mo ju chan che\"",
            "\"dan yu qi mu ju ch\"*",
            "\"dan yu qi mo ju d\"*",
            "\"s y q m j d c\"",
            "\"c y q m j s c\"",
            "\"c y q m j\"",
        ];

        for query in queries {
            let exp = DbPinyinObject::content().match_(query);
            let results = database
                .get_all_objects(
                    DbPinyinObject::all_fields(),
                    &fts_test.table_name,
                    Some(&exp),
                    None,
                    None,
                    None,
                )
                .unwrap();
            assert_eq!(results.len(), 1);
            assert_eq!(results[0].content, content);
        }

        teardown(&fts_test);
    }

    #[test]
    fn test_traditional_chinese() {
        let fts_test = FTSTest::new();
        setup(&fts_test);

        let arc_db = fts_test.database_test_case.get_database();
        let database = arc_db.read().unwrap();
        let table_name = &fts_test.table_name;

        Database::config_traditional_chinese_dict(&HashMap::from([("們", "们"), ("員", "员")]));
        database.add_tokenizer(BuiltinTokenizer::VERBATIM);
        database
            .create_virtual_table(table_name, &*DB_TRADITIONAL_CHINESE_OBJECT_INSTANCE)
            .unwrap();

        let table = database.get_table(table_name, &*DB_TRADITIONAL_CHINESE_OBJECT_INSTANCE);

        let mut obj = TraditionalChineseObject::default();
        obj.content = "我們是程序員".to_string();
        table.insert_object(obj.clone(), None).unwrap();

        let exp = DbTraditionalChineseObject::content().match_("我們是程序員");
        let vec = table
            .get_all_objects(None, Some(&exp), None, None, None)
            .unwrap();
        assert_eq!(vec.len(), 1);
        assert_eq!(vec[0].content, obj.content);

        let exp = DbTraditionalChineseObject::content().match_("我们是程序员");
        let vec = table
            .get_all_objects(None, Some(&exp), None, None, None)
            .unwrap();
        assert_eq!(vec.len(), 1);
        assert_eq!(vec[0].content, obj.content);

        teardown(&fts_test);
    }
}
