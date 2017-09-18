//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//___COPYRIGHT___
//

#import "WCTTokenizer+___FILEBASENAME___.h"
#import <WCDB/WCDB.h>
#import <WCDB/fts_module.hpp>

namespace WCDB {

namespace FTS {

#pragma mark - Module
class ___FILEBASENAMEASIDENTIFIER___Module {
public:
    constexpr static const char Name[] = "___FILEBASENAMEASIDENTIFIER___";

private:
    static const std::nullptr_t s_register;
};

constexpr const char ___FILEBASENAMEASIDENTIFIER___Module::Name[];

const std::nullptr_t ___FILEBASENAMEASIDENTIFIER___Module::s_register = []() {
    FTS::Module<___FILEBASENAMEASIDENTIFIER___Module::Name>::Register();
    return nullptr;
}();

//#pragma mark - Tokenizer - Optional
//class ___FILEBASENAMEASIDENTIFIER___TokenizerInfo {
//public:
//    <#Your Tokenizer Info#>
//};
//
//template <>
//bool Module<___FILEBASENAMEASIDENTIFIER___Module::Name>::ShouldCreateTokenizerInfo()
//{
//    return true;
//}
//
//template <>
//void *Module<___FILEBASENAMEASIDENTIFIER___Module::Name>::CreateTokenizerInfo(int argc, const char *const *argv)
//{
//    ___FILEBASENAMEASIDENTIFIER___TokenizerInfo *tokenizerInfo = (___FILEBASENAMEASIDENTIFIER___TokenizerInfo *) sqlite3_malloc(sizeof(___FILEBASENAMEASIDENTIFIER___TokenizerInfo));
//    if (tokenizerInfo) {
//
//        <#Init Your Tokenizer Info#>
//
//    }
//    return tokenizerInfo;
//}
//
//template <>
//void Module<___FILEBASENAMEASIDENTIFIER___Module::Name>::DestroyTokenizerInfo(void *info)
//{
//    if (info) {
//        ___FILEBASENAMEASIDENTIFIER___TokenizerInfo *cursorInfo = (___FILEBASENAMEASIDENTIFIER___TokenizerInfo *) info;
//
//        <#Deinit Your Tokenizer Info #>
//
//        sqlite3_free(cursorInfo);
//    }
//}

#pragma mark - Cursor
class ___FILEBASENAMEASIDENTIFIER___CursorInfo {
public:
    <#Your Cursor Info#>
};

template <>
void *Module<___FILEBASENAMEASIDENTIFIER___Module::Name>::CreateCursorInfo(const char *pInput, int nBytes)
{
    ___FILEBASENAMEASIDENTIFIER___CursorInfo *cursorInfo = (___FILEBASENAMEASIDENTIFIER___CursorInfo *) sqlite3_malloc(sizeof(___FILEBASENAMEASIDENTIFIER___CursorInfo));
    if (cursorInfo) {

        <#Init Your Cursor Info#>
        
    }
    return cursorInfo;
}

template <>
int Module<___FILEBASENAMEASIDENTIFIER___Module::Name>::CursorStep(void *info,
                                                                   const char **ppToken,
                                                                   int *pnBytes,
                                                                   int *piStartOffset,
                                                                   int *piEndOffset,
                                                                   int *piPosition)
{
    if (!info) {
        return SQLITE_ERROR;
    }

    ___FILEBASENAMEASIDENTIFIER___CursorInfo *cursorInfo = (___FILEBASENAMEASIDENTIFIER___CursorInfo *) info;

    if (<#Is It Done Yet ?#>) {

        <#Your Cursor Step #>

        return SQLITE_OK;
    }
    return SQLITE_DONE;
}

template <>
void Module<___FILEBASENAMEASIDENTIFIER___Module::Name>::DestroyCursorInfo(void *info)
{
    if (info) {
        ___FILEBASENAMEASIDENTIFIER___CursorInfo *cursorInfo = (___FILEBASENAMEASIDENTIFIER___CursorInfo *) info;

        <#Deinit Your Cursor Info #>

        sqlite3_free(cursorInfo);
    }
}

} //namespace FTS

} //namespace WCDB

NSString *const WCTTokenizerName___FILEBASENAMEASIDENTIFIER___ = @(WCDB::FTS::___FILEBASENAMEASIDENTIFIER___Module::Name);
