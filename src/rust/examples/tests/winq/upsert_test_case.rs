#[cfg(test)]
pub mod upsert_test {
    use crate::base::winq_tool::WinqTool;
    use wcdb::winq::column::Column;
    use wcdb::winq::expression_operable::ExpressionOperableTrait;
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
                .indexed_by(vec![&Column::new("column1", None)])
                .do_no_thing(),
            "ON CONFLICT(column1) DO NOTHING",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .indexed_by(vec![&Column::new("column1", None)])
                .where_(&Column::new("column1", None).eq(1))
                .do_no_thing(),
            "ON CONFLICT(column1) WHERE column1 == 1 DO NOTHING",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set(vec![Column::new("column1", None)])
                .to(None),
            "ON CONFLICT DO UPDATE SET column1 = NULL",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set(vec![Column::new("column1", None)])
                .to(true),
            "ON CONFLICT DO UPDATE SET column1 = TRUE",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set(vec![Column::new("column1", None)])
                .to(1),
            "ON CONFLICT DO UPDATE SET column1 = 1",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set(vec![Column::new("column1", None)])
                .to("abc"),
            "ON CONFLICT DO UPDATE SET column1 = 'abc'",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set(vec![Column::new("column1", None)])
                .to(1)
                .set(column_vec)
                .to(2),
            "ON CONFLICT DO UPDATE SET column1 = 1, (column2, column3) = 2",
        );
        WinqTool::winq_equal(
            Upsert::new()
                .on_conflict()
                .do_update()
                .set(vec![Column::new("column1", None)])
                .to(1)
                .where_(&Column::new("column1", None).eq(2)),
            "ON CONFLICT DO UPDATE SET column1 = 1 WHERE column1 == 2",
        );
    }
}
