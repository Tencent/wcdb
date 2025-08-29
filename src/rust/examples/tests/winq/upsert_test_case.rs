#[cfg(test)]
pub mod upsert_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::upsert::Upsert;

    #[test]
    pub fn test() {
        let column_vec = vec![Column::new("column2", None), Column::new("column3", None)];
        WinqTool::winq_equal(
            Upsert::new().on_conflict().do_no_thing(),
            "ON CONFLICT DO NOTHING",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .indexed_by_indexed_column_convertible_trait(vec![&Column::new("column1", None)])
                .do_no_thing(),
            "ON CONFLICT(column1) DO NOTHING",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .indexed_by_indexed_column_convertible_trait(vec![&Column::new("column1", None)])
                .where_(&Column::new("column1", None).eq(1))
                .do_no_thing(),
            "ON CONFLICT(column1) WHERE column1 == 1 DO NOTHING",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set_with_columns(&vec![Column::new("column1", None)])
                .to_expression_convertible_trait::<Column>(None),
            "ON CONFLICT DO UPDATE SET column1 = NULL",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set_with_columns(&vec![Column::new("column1", None)])
                .to_bool(true),
            "ON CONFLICT DO UPDATE SET column1 = TRUE",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set_with_columns(&vec![Column::new("column1", None)])
                .to_i32(1),
            "ON CONFLICT DO UPDATE SET column1 = 1",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set_with_columns(&vec![Column::new("column1", None)])
                .to_string("abc".parse().unwrap()),
            "ON CONFLICT DO UPDATE SET column1 = 'abc'",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set_with_columns(&vec![Column::new("column1", None)])
                .to_i32(1)
                .set_with_columns(&column_vec)
                .to_i32(2),
            "ON CONFLICT DO UPDATE SET column1 = 1, (column2, column3) = 2",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set_with_columns(&vec![Column::new("column1", None)])
                .to_i32(1)
                .where_(&Column::new("column1", None).eq(2)),
            "ON CONFLICT DO UPDATE SET column1 = 1 WHERE column1 == 2",
        );
    }
}
