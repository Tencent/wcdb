#[cfg(test)]
pub mod expression_test {
    use wcdb_core::winq::column::Column;
    use wcdb_core::winq::expression::Expression;
    use wcdb_core::winq::expression_operable_trait::ExpressionOperableTrait;
    use wcdb_core::winq::identifier::IdentifierTrait;

    #[test]
    pub fn test_expression_binary_operation() {
        let expression_left = Expression::new_with_column(Column::new("left"));
        let expression_right = Expression::new_with_column(Column::new("right"));
    }

    #[test]
    pub fn test_binary_operation() {
        let mut column_left = Column::new("left");
        let column_right = Column::new("right");

        let desc = column_left.or(&column_right).get_description();
        assert_eq!(desc.as_str(), "left OR right");
        let desc = column_left.and(&column_right).get_description();
        assert_eq!(desc.as_str(), "left AND right");

        // multiply assert
        let desc = column_left
            .multiply_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left * right");
        let operand: i32 = 1;
        let desc = column_left.multiply_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left * ".to_owned() + operand.to_string().as_str()
        );
        let operand: f64 = 1.1;
        let desc = column_left.multiply_double(operand).get_description();
        assert_eq!(desc.as_str(), "left * 1.1000000000000001");
        let operand: i8 = 1;
        let desc = column_left.multiply_byte(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left * ".to_owned() + operand.to_string().as_str()
        );
        let operand: i16 = 1;
        let desc = column_left.multiply_short(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left * ".to_owned() + operand.to_string().as_str()
        );

        // divide assert
        let desc = column_left
            .divide_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left / right");
        let operand: i32 = 1;
        let desc = column_left.divide_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left / ".to_owned() + operand.to_string().as_str()
        );
        let operand: f64 = 1.1;
        let desc = column_left.divide_double(operand).get_description();
        assert_eq!(desc.as_str(), "left / 1.1000000000000001");

        // mod assert
        let desc = column_left
            .mod_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left % right");

        let operand: i32 = 1;
        let desc = column_left.mod_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left % ".to_owned() + operand.to_string().as_str()
        );

        let operand: f64 = 1.1;
        let desc = column_left.mod_double(operand).get_description();
        assert_eq!(desc.as_str(), "left % 1.1000000000000001");

        // add assert
        let desc = column_left
            .add_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left + right");

        let operand: i32 = 1;
        let desc = column_left.add_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left + ".to_owned() + operand.to_string().as_str()
        );

        let operand: f64 = 1.1;
        let desc = column_left.add_double(operand).get_description();
        assert_eq!(desc.as_str(), "left + 1.1000000000000001");

        // minus assert
        let desc = column_left
            .minus_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left - right");

        let operand: i32 = 1;
        let desc = column_left.minus_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left - ".to_owned() + operand.to_string().as_str()
        );

        let operand: f64 = 1.1;
        let desc = column_left.minus_double(operand).get_description();
        assert_eq!(desc.as_str(), "left - 1.1000000000000001");

        // left shift assert
        let desc = column_left
            .left_shift_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left << right");

        let operand: i32 = 1;
        let desc = column_left.left_shift_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left << ".to_owned() + operand.to_string().as_str()
        );

        // right shift assert
        let desc = column_left
            .right_shift_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left >> right");

        let operand: i32 = 1;
        let desc = column_left.right_shift_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left >> ".to_owned() + operand.to_string().as_str()
        );

        // bit and assert
        let desc = column_left
            .bit_and_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left & right");

        let operand: i32 = 1;
        let desc = column_left.bit_and_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left & ".to_owned() + operand.to_string().as_str()
        );

        // bit or assert
        let desc = column_left
            .bit_or_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left | right");

        let operand: i32 = 1;
        let desc = column_left.bit_or_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left | ".to_owned() + operand.to_string().as_str()
        );

        // lt or assert
        let desc = column_left
            .lt_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left < right");

        let operand: i32 = 1;
        let desc = column_left.lt_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left < ".to_owned() + operand.to_string().as_str()
        );

        let desc = column_left.lt_double(1.1).get_description();
        assert_eq!(desc.as_str(), "left < 1.1000000000000001");

        let desc = column_left.lt_string("abc").get_description();
        assert_eq!(desc.as_str(), "left < 'abc'");

        // le or assert
        let desc = column_left
            .le_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left <= right");

        let operand: i32 = 1;
        let desc = column_left.le_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left <= ".to_owned() + operand.to_string().as_str()
        );

        let desc = column_left.le_double(1.1).get_description();
        assert_eq!(desc.as_str(), "left <= 1.1000000000000001");

        let desc = column_left.le_string("abc").get_description();
        assert_eq!(desc.as_str(), "left <= 'abc'");

        // gt or assert
        let desc = column_left
            .gt_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left > right");

        let operand: i32 = 1;
        let desc = column_left.gt_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left > ".to_owned() + operand.to_string().as_str()
        );

        let desc = column_left.gt_double(1.1).get_description();
        assert_eq!(desc.as_str(), "left > 1.1000000000000001");

        let desc = column_left.gt_string("abc").get_description();
        assert_eq!(desc.as_str(), "left > 'abc'");

        // ge or assert
        let desc = column_left
            .ge_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left >= right");

        let operand: i32 = 1;
        let desc = column_left.ge_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left >= ".to_owned() + operand.to_string().as_str()
        );

        let desc = column_left.ge_double(1.1).get_description();
        assert_eq!(desc.as_str(), "left >= 1.1000000000000001");

        let desc = column_left.ge_string("abc").get_description();
        assert_eq!(desc.as_str(), "left >= 'abc'");

        // eq or assert
        let desc = column_left
            .eq_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left == right");

        let desc = column_left.eq_bool(false).get_description();
        assert_eq!(desc.as_str(), "left == FALSE");

        let operand: i32 = 1;
        let desc = column_left.eq_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left == ".to_owned() + operand.to_string().as_str()
        );

        let desc = column_left.eq_double(1.1).get_description();
        assert_eq!(desc.as_str(), "left == 1.1000000000000001");

        let desc = column_left.eq_string("abc").get_description();
        assert_eq!(desc.as_str(), "left == 'abc'");

        //not eq
        let desc = column_left
            .not_eq_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left != right");

        let desc = column_left.not_eq_bool(false).get_description();
        assert_eq!(desc.as_str(), "left != FALSE");

        let operand: i32 = 1;
        let desc = column_left.not_eq_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left != ".to_owned() + operand.to_string().as_str()
        );

        let desc = column_left.not_eq_double(1.1).get_description();
        assert_eq!(desc.as_str(), "left != 1.1000000000000001");

        let desc = column_left.not_eq_string("abc").get_description();
        assert_eq!(desc.as_str(), "left != 'abc'");

        // concat
        let desc = column_left
            .concat_expression_convertible(&column_right)
            .get_description();
        assert_eq!(desc.as_str(), "left || right");

        let operand: i32 = 1;
        let desc = column_left.concat_int(operand).get_description();
        assert_eq!(
            desc.as_str(),
            "left || ".to_owned() + operand.to_string().as_str()
        );

        let desc = column_left.concat_double(1.1).get_description();
        assert_eq!(desc.as_str(), "left || 1.1000000000000001");

        let desc = column_left.concat_string("abc").get_description();
        assert_eq!(desc.as_str(), "left || 'abc'");
    }

    #[test]
    pub fn test_between_operation() {
        let column = Column::new("testColumn");
        let start = Column::new("start");
        let end = Column::new("end");

        let desc = column.between_expr_expr(&start, &end).get_description();
        assert_eq!(desc.as_str(), "testColumn BETWEEN start AND end");
        let desc = column.between_expr_long(&start, 1).get_description();
        assert_eq!(desc.as_str(), "testColumn BETWEEN start AND 1");
        let desc = column.between_expr_double(&start, 1.1).get_description();
        assert_eq!(
            desc.as_str(),
            "testColumn BETWEEN start AND 1.1000000000000001"
        );
        let desc = column.between_expr_string(&start, "abc").get_description();
        assert_eq!(desc.as_str(), "testColumn BETWEEN start AND 'abc'");

        let desc = column.between_long_expr(1, &end).get_description();
        assert_eq!(desc.as_str(), "testColumn BETWEEN 1 AND end");
        let desc = column.between_long_long(1, 1).get_description();
        assert_eq!(desc.as_str(), "testColumn BETWEEN 1 AND 1");
        let desc = column.between_long_double(1, 1.1).get_description();
        assert_eq!(desc.as_str(), "testColumn BETWEEN 1 AND 1.1000000000000001");
        let desc = column.between_long_string(1, "abc").get_description();
        assert_eq!(desc.as_str(), "testColumn BETWEEN 1 AND 'abc'");

        let desc = column.between_string_expr("abc", &end).get_description();
        assert_eq!(desc.as_str(), "testColumn BETWEEN 'abc' AND end");
        let desc = column.between_string_long("abc", 1).get_description();
        assert_eq!(desc.as_str(), "testColumn BETWEEN 'abc' AND 1");
        let desc = column.between_string_double("abc", 1.1).get_description();
        assert_eq!(
            desc.as_str(),
            "testColumn BETWEEN 'abc' AND 1.1000000000000001"
        );
        let desc = column.between_string_string("abc", "abc").get_description();
        assert_eq!(desc.as_str(), "testColumn BETWEEN 'abc' AND 'abc'");

        let desc = column.not_between_expr_expr(&start, &end).get_description();
        assert_eq!(desc.as_str(), "testColumn NOT BETWEEN start AND end");
        let desc = column.not_between_expr_long(&start, 1).get_description();
        assert_eq!(desc.as_str(), "testColumn NOT BETWEEN start AND 1");
        let desc = column
            .not_between_expr_double(&start, 1.1)
            .get_description();
        assert_eq!(
            desc.as_str(),
            "testColumn NOT BETWEEN start AND 1.1000000000000001"
        );
        let desc = column
            .not_between_expr_string(&start, "abc")
            .get_description();
        assert_eq!(desc.as_str(), "testColumn NOT BETWEEN start AND 'abc'");

        let desc = column.not_between_long_expr(1, &end).get_description();
        assert_eq!(desc.as_str(), "testColumn NOT BETWEEN 1 AND end");
        let desc = column.not_between_long_long(1, 1).get_description();
        assert_eq!(desc.as_str(), "testColumn NOT BETWEEN 1 AND 1");
        let desc = column.not_between_long_double(1, 1.1).get_description();
        assert_eq!(
            desc.as_str(),
            "testColumn NOT BETWEEN 1 AND 1.1000000000000001"
        );
        let desc = column.not_between_long_string(1, "abc").get_description();
        assert_eq!(desc.as_str(), "testColumn NOT BETWEEN 1 AND 'abc'");

        let desc = column
            .not_between_string_expr("abc", &end)
            .get_description();
        assert_eq!(desc.as_str(), "testColumn NOT BETWEEN 'abc' AND end");
        let desc = column.not_between_string_long("abc", 1).get_description();
        assert_eq!(desc.as_str(), "testColumn NOT BETWEEN 'abc' AND 1");
        let desc = column
            .not_between_string_double("abc", 1.1)
            .get_description();
        assert_eq!(
            desc.as_str(),
            "testColumn NOT BETWEEN 'abc' AND 1.1000000000000001"
        );
        let desc = column
            .not_between_string_string("abc", "abc")
            .get_description();
        assert_eq!(desc.as_str(), "testColumn NOT BETWEEN 'abc' AND 'abc'");
    }

    #[test]
    pub fn test_in_operation() {
        let column = Column::new("testColumn");

        let operands: Vec<i16> = vec![1, 2, 3];
        let desc = column.in_short(operands).get_description();
        assert_eq!(desc.as_str(), "testColumn IN(1, 2, 3)");

        let operands: Vec<i32> = vec![1, 2, 3];
        let desc = column.in_int(operands).get_description();
        assert_eq!(desc.as_str(), "testColumn IN(1, 2, 3)");

        let operands: Vec<i64> = vec![1, 2, 3];
        let desc = column.in_long(operands).get_description();
        assert_eq!(desc.as_str(), "testColumn IN(1, 2, 3)");

        let operands: Vec<f32> = vec![1.1f32, 2.1f32, 3.1f32];
        let desc = column.in_float(operands).get_description();
        assert_eq!(
            desc.as_str(),
            "testColumn IN(1.1000000238418579, 2.0999999046325684, 3.0999999046325684)"
        );

        let operands: Vec<f64> = vec![1.1f64, 2.1f64, 3.1f64];
        let desc = column.in_double(operands).get_description();
        assert_eq!(
            desc.as_str(),
            "testColumn IN(1.1000000000000001, 2.1000000000000001, 3.1000000000000001)"
        );

        let mut operands: Vec<&str> = Vec::new();
        operands.push("abc");
        operands.push("def");
        operands.push("ghi");
        let desc = column.in_string(operands).get_description();
        assert_eq!(desc.as_str(), "testColumn IN('abc', 'def', 'ghi')");
    }

    #[test]
    pub fn test_not_in_operation() {
        let column = Column::new("testColumn");

        let operands: Vec<i16> = vec![1, 2, 3];
        let desc = column.not_in_short(operands).get_description();
        assert_eq!(desc.as_str(), "testColumn NOT IN(1, 2, 3)");

        let operands: Vec<i32> = vec![1, 2, 3];
        let desc = column.not_in_int(operands).get_description();
        assert_eq!(desc.as_str(), "testColumn NOT IN(1, 2, 3)");

        let operands: Vec<i64> = vec![1, 2, 3];
        let desc = column.not_in_long(operands).get_description();
        assert_eq!(desc.as_str(), "testColumn NOT IN(1, 2, 3)");

        let operands: Vec<f32> = vec![1.1f32, 2.1f32, 3.1f32];
        let desc = column.not_in_float(operands).get_description();
        assert_eq!(
            desc.as_str(),
            "testColumn NOT IN(1.1000000238418579, 2.0999999046325684, 3.0999999046325684)"
        );

        let operands: Vec<f64> = vec![1.1f64, 2.1f64, 3.1f64];
        let desc = column.not_in_double(operands).get_description();
        assert_eq!(
            desc.as_str(),
            "testColumn NOT IN(1.1000000000000001, 2.1000000000000001, 3.1000000000000001)"
        );

        let mut operands: Vec<&str> = Vec::new();
        operands.push("abc");
        operands.push("def");
        operands.push("ghi");
        let desc = column.not_in_string(operands).get_description();
        assert_eq!(desc.as_str(), "testColumn NOT IN('abc', 'def', 'ghi')");
    }

    #[test]
    pub fn test_collate() {
        let column = Column::new("testColumn");
        let desc = column.collate("BINARY").get_description();
        assert_eq!(desc.as_str(), "testColumn COLLATE BINARY");
    }

    #[test]
    pub fn test_function() {
        let left = Column::new("left");
        let right: &str = "right";

        let desc = left.substr_int(1, 2).get_description();
        assert_eq!(desc.as_str(), "SUBSTR(left, 1, 2)");

        let desc = left.like(right).get_description();
        assert_eq!(desc.as_str(), "left LIKE 'right'");

        let desc = left.glob(right).get_description();
        assert_eq!(desc.as_str(), "left GLOB 'right'");

        let desc = left.match_string(right).get_description();
        assert_eq!(desc.as_str(), "left MATCH 'right'");

        let desc = left.regexp(right).get_description();
        assert_eq!(desc.as_str(), "left REGEXP 'right'");

        let desc = left.not_like(right).get_description();
        assert_eq!(desc.as_str(), "left NOT LIKE 'right'");

        let desc = left.not_glob(right).get_description();
        assert_eq!(desc.as_str(), "left NOT GLOB 'right'");

        let desc = left.not_match(right).get_description();
        assert_eq!(desc.as_str(), "left NOT MATCH 'right'");

        let desc = left.not_regexp(right).get_description();
        assert_eq!(desc.as_str(), "left NOT REGEXP 'right'");

        let desc = left.like(right).escape("%").get_description();
        assert_eq!(desc.as_str(), "left LIKE 'right' ESCAPE '%'");

        let desc = left.glob(right).escape("%").get_description();
        assert_eq!(desc.as_str(), "left GLOB 'right' ESCAPE '%'");

        let desc = left.match_string(right).escape("%").get_description();
        assert_eq!(desc.as_str(), "left MATCH 'right' ESCAPE '%'");

        let desc = left.regexp(right).escape("%").get_description();
        assert_eq!(desc.as_str(), "left REGEXP 'right' ESCAPE '%'");

        let desc = left.not_like(right).escape("%").get_description();
        assert_eq!(desc.as_str(), "left NOT LIKE 'right' ESCAPE '%'");

        let desc = left.not_glob(right).escape("%").get_description();
        assert_eq!(desc.as_str(), "left NOT GLOB 'right' ESCAPE '%'");

        let desc = left.not_match(right).escape("%").get_description();
        assert_eq!(desc.as_str(), "left NOT MATCH 'right' ESCAPE '%'");

        let desc = left.not_regexp(right).escape("%").get_description();
        assert_eq!(desc.as_str(), "left NOT REGEXP 'right' ESCAPE '%'");

        //is
        let desc = left.is_string(right).get_description();
        assert_eq!(desc.as_str(), "left IS 'right'");

        let desc = left.is_not_string(right).get_description();
        assert_eq!(desc.as_str(), "left IS NOT 'right'");

        let desc = left.avg().get_description();
        assert_eq!(desc.as_str(), "AVG(left)");

        let desc = left.count().distinct().get_description();
        assert_eq!(desc.as_str(), "COUNT(DISTINCT left)");

        let desc = left.group_concat().get_description();
        assert_eq!(desc.as_str(), "GROUP_CONCAT(left)");

        let desc = left.group_concat_string("-").distinct().get_description();
        assert_eq!(desc.as_str(), "GROUP_CONCAT(DISTINCT left, '-')");

        let desc = left.max().get_description();
        assert_eq!(desc.as_str(), "MAX(left)");

        let desc = left.min().get_description();
        assert_eq!(desc.as_str(), "MIN(left)");

        let desc = left.sum().get_description();
        assert_eq!(desc.as_str(), "SUM(left)");

        let desc = left.total().get_description();
        assert_eq!(desc.as_str(), "TOTAL(left)");

        let desc = left.abs().get_description();
        assert_eq!(desc.as_str(), "ABS(left)");

        let desc = left.hex().get_description();
        assert_eq!(desc.as_str(), "HEX(left)");

        let desc = left.length().get_description();
        assert_eq!(desc.as_str(), "LENGTH(left)");

        let desc = left.lower().get_description();
        assert_eq!(desc.as_str(), "LOWER(left)");

        let desc = left.upper().get_description();
        assert_eq!(desc.as_str(), "UPPER(left)");

        let desc = left.round().get_description();
        assert_eq!(desc.as_str(), "ROUND(left)");

        let desc = left.match_info().get_description();
        assert_eq!(desc.as_str(), "matchInfo(left)");

        let desc = left.offsets().get_description();
        assert_eq!(desc.as_str(), "offsets(left)");

        let desc = left.snippet().get_description();
        assert_eq!(desc.as_str(), "snippet(left)");

        let desc = left.bm25().get_description();
        assert_eq!(desc.as_str(), "bm25(left)");

        let desc = left.highlight().get_description();
        assert_eq!(desc.as_str(), "highlight(left)");

        let desc = left.substring_match_info().get_description();
        assert_eq!(desc.as_str(), "substring_match_info(left)");
    }
}
