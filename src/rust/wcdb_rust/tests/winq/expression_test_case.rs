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
    }
}
