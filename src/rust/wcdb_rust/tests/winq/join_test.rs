use wcdb_core::winq::column::Column;
use wcdb_core::winq::identifier::IdentifierTrait;
use wcdb_core::winq::statement_select::StatementSelect;

pub struct JoinTest {}
impl JoinTest {
    pub fn new() -> JoinTest {
        JoinTest {}
    }

    fn select1(&self) -> StatementSelect {
        let column = Column::new("column1");
        let columns = vec![column];
        let ret = StatementSelect::new();
        ret.select_with_result_column_convertible_trait(&columns)
            .from("testTable1");
        ret
    }

    fn select1sql(&self) -> String {
        let statement_select = self.select1();
        let mut ret: String = "".to_string();
        ret = format!("{}{}{}", "(", statement_select.get_description(), ")");
        ret
    }

    fn select2(&self) -> StatementSelect {
        let column = Column::new("column2");
        let columns = vec![column];
        let ret = StatementSelect::new();
        ret.select_with_result_column_convertible_trait(&columns)
            .from("testTable2");
        ret
    }

    fn select2sql(&self) -> String {
        let statement_select = self.select2();
        let mut ret: String = "".to_string();
        ret = format!("{}{}{}", "(", statement_select.get_description(), ")");
        ret
    }
}

#[cfg(test)]
pub mod join_test {
    use crate::base::winq_tool::WinqTool;
    use crate::winq::join_test::JoinTest;
    use wcdb_core::winq::column::Column;
    use wcdb_core::winq::expression_operable_trait::ExpressionOperableTrait;
    use wcdb_core::winq::join;
    use wcdb_core::winq::join::Join;

    #[test]
    pub fn test() {
        let test_table_name1 = "testTable1";
        let test_table_name2 = "testTable2";

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        WinqTool::winq_equal(
            join_left.join_with_table_name(test_table_name2),
            "testTable1 JOIN testTable2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left.join_with_table_or_subquery_convertible(&join_test.select2()),
            &*format!("{}{}{}", select1sql, " JOIN ", select2sql).to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        WinqTool::winq_equal(
            join_left.with_table_name(test_table_name2),
            "testTable1, testTable2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left.with_table_or_subquery_convertible(&join_test.select2()),
            &*format!("{}{}{}", select1sql, ", ", select2sql).to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        WinqTool::winq_equal(
            join_left.left_join_with_table_name(test_table_name2),
            "testTable1 LEFT JOIN testTable2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left.left_join_with_table_or_subquery_convertible(&join_test.select2()),
            &*format!("{}{}{}", select1sql, " LEFT JOIN ", select2sql).to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        WinqTool::winq_equal(
            join_left.left_outer_join_with_table_name(test_table_name2),
            "testTable1 LEFT OUTER JOIN testTable2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left.left_outer_join_with_table_or_subquery_convertible(&join_test.select2()),
            &*format!("{}{}{}", select1sql, " LEFT OUTER JOIN ", select2sql).to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        WinqTool::winq_equal(
            join_left.inner_join_with_table_name(test_table_name2),
            "testTable1 INNER JOIN testTable2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left.inner_join_with_table_or_subquery_convertible(&join_test.select2()),
            &*format!("{}{}{}", select1sql, " INNER JOIN ", select2sql).to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        WinqTool::winq_equal(
            join_left.cross_join_with_table_name(test_table_name2),
            "testTable1 CROSS JOIN testTable2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left.cross_join_with_table_or_subquery_convertible(&join_test.select2()),
            &*format!("{}{}{}", select1sql, " CROSS JOIN ", select2sql).to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        WinqTool::winq_equal(
            join_left.natural_join_with_table_name(test_table_name2),
            "testTable1 NATURAL JOIN testTable2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left.natural_join_with_table_or_subquery_convertible(&join_test.select2()),
            &*format!("{}{}{}", select1sql, " NATURAL JOIN ", select2sql).to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        WinqTool::winq_equal(
            join_left.natural_left_join_with_table_name(test_table_name2),
            "testTable1 NATURAL LEFT JOIN testTable2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left.natural_left_join_with_table_or_subquery_convertible(&join_test.select2()),
            &*format!("{}{}{}", select1sql, " NATURAL LEFT JOIN ", select2sql).to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        WinqTool::winq_equal(
            join_left.natural_left_outer_join_with_table_name(test_table_name2),
            "testTable1 NATURAL LEFT OUTER JOIN testTable2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left
                .natural_left_outer_join_with_table_or_subquery_convertible(&join_test.select2()),
            &*format!(
                "{}{}{}",
                select1sql, " NATURAL LEFT OUTER JOIN ", select2sql
            )
            .to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        WinqTool::winq_equal(
            join_left.natural_inner_join_with_table_name(test_table_name2),
            "testTable1 NATURAL INNER JOIN testTable2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left.natural_inner_join_with_table_or_subquery_convertible(&join_test.select2()),
            &*format!("{}{}{}", select1sql, " NATURAL INNER JOIN ", select2sql).to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        WinqTool::winq_equal(
            join_left.natural_cross_join_with_table_name(test_table_name2),
            "testTable1 NATURAL CROSS JOIN testTable2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left.natural_cross_join_with_table_or_subquery_convertible(&join_test.select2()),
            &*format!("{}{}{}", select1sql, " NATURAL CROSS JOIN ", select2sql).to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let join_right = Join::new_with_table_name(test_table_name2);
        let column1 = Column::new("column1");
        let column2 = Column::new("column2");
        WinqTool::winq_equal(
            join_left
                .join_with_table_name(test_table_name2)
                .on(&column1.eq_expression_convertible(&column2)),
            "testTable1 JOIN testTable2 ON column1 == column2",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let column1 = Column::new("column1");
        let column2 = Column::new("column2");
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        WinqTool::winq_equal(
            join_left
                .join_with_table_or_subquery_convertible(&join_test.select2())
                .on(&column1.eq_expression_convertible(&column2)),
            &*format!(
                "{}{}{}{}",
                select1sql, " JOIN ", select2sql, " ON column1 == column2"
            )
            .to_string(),
        );

        let join_left = Join::new_with_table_name(test_table_name1);
        let column1 = Column::new("column1");
        let column2 = Column::new("column2");
        let mut column_vec: Vec<Column> = Vec::new();
        column_vec.push(column1);
        column_vec.push(column2);
        WinqTool::winq_equal(
            join_left
                .join_with_table_name(test_table_name2)
                .using_with_column_obj_vector(&column_vec),
            "testTable1 JOIN testTable2 USING(column1, column2)",
        );

        let join_test = JoinTest::new();
        let join_left = Join::new_with_table_or_subquery_convertible(&join_test.select1());
        let join_right = Join::new_with_table_or_subquery_convertible(&join_test.select2());
        let column1 = Column::new("column1");
        let column2 = Column::new("column2");
        let select1sql = join_test.select1sql();
        let select2sql = join_test.select2sql();
        let mut column_vec: Vec<Column> = Vec::new();
        column_vec.push(column1);
        column_vec.push(column2);
        WinqTool::winq_equal(
            join_left
                .join_with_table_or_subquery_convertible(&join_test.select2())
                .using_with_column_obj_vector(&column_vec),
            &*format!(
                "{}{}{}{}",
                select1sql, " JOIN ", select2sql, " USING(column1, column2)"
            ),
        );
    }
}
