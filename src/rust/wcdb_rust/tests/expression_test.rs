#[cfg(test)]
pub mod expression_test {
    use wcdb_core::winq::column::Column;

    #[test]
    pub fn test_expression() {
        let name: &str = "testColumn";
        let column: Column = Column::new(name);
    }

    pub fn test_binary_operation() {
        let left_str: &str = "left";
        let right_str: &str = "right";
        let mut left: Column = Column::new(left_str);
        left.or(right_str);
    }
}
