use proc_macro::TokenStream;

#[proc_macro_derive(TableBinding)]
pub fn table_binding_macro(input: TokenStream) -> TokenStream {
    //     let input = parse_macro_input!(input as DeriveInput);
    //
    //     // 获取结构体的名称
    //     let struct_name = input.ident;
    //
    //     // 提取 `wcdb` 属性信息
    //     let mut table_constraints = vec![];
    //     let mut column_definitions = vec![];
    //     if let Some(attrs) = input.attrs.iter().find(|attr| attr.path.is_ident("wcdb")) {
    //         if let Ok(Meta::List(MetaList { nested, .. })) = attrs.parse_meta() {
    //             for meta in nested {
    //                 if let NestedMeta::Meta(Meta::NameValue(pair)) = meta {
    //                     let key = pair.path.get_ident().unwrap().to_string();
    //                     let value = match pair.lit {
    //                         Lit::Str(ref lit_str) => lit_str.value(),
    //                         _ => panic!("Unsupported wcdb attribute type"),
    //                     };
    //
    //                     match key.as_str() {
    //                         "multiIndexes" | "multiPrimaries" | "multiUnique" |
    //                         "tableName" | "tableConstraint" | "virtualTable" => {
    //                             table_constraints.push((key, value));
    //                         }
    //                         _ => panic!("Unsupported wcdb attribute key: {}", key),
    //                     }
    //                 }
    //             }
    //         }
    //     }
    //
    //     // 生成列的绑定
    //     if let syn::Data::Struct(data) = &input.data {
    //         if let syn::Fields::Named(fields) = &data.fields {
    //             for field in &fields.named {
    //                 let field_name = field.ident.as_ref().unwrap();
    //                 column_definitions.push(quote! {
    //                     println!("Binding column: {}", stringify!(#field_name));
    //                 });
    //             }
    //         }
    //     }
    //
    //     // 生成绑定逻辑代码
    //     let gen = quote! {
    //         impl #struct_name {
    //             pub fn create_binding() {
    //                 #(#column_definitions)*
    //
    //                 #(#table_constraints.iter().for_each(|(k, v)| {
    //                     println!("Constraint {}: {}", k, v);
    //                 });)*
    //             }
    //         }
    //     };
    //
    TokenStream::new()
}
