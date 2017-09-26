//___FILEHEADER___

#import "WCTTokenizer+___VARIABLE_productName___.h"
#import <WCDB/WCDB.h>
#import <WCDB/fts_module.hpp>

namespace WCDB {

namespace FTS {

#pragma mark - Tokenizer
class ___VARIABLE_productName___TokenizerInfo : public TokenizerInfoBase {
public:
    ___VARIABLE_productName___TokenizerInfo(int argc, const char *const *argv)
        : TokenizerInfoBase(argc, argv)
    {
        /* <#Init Your Tokenizer Info#> */
    }
    ~___VARIABLE_productName___TokenizerInfo()
    {
        /* <#Deinit Your Tokenizer Info#> */
    }

protected:
    /* <#Your Tokenizer Info#> */
};

#pragma mark - Cursor
class ___VARIABLE_productName___CursorInfo : public CursorInfoBase {
public:
    ___VARIABLE_productName___CursorInfo(const char *input, int bytes, TokenizerInfoBase *tokenizerInfo)
        : CursorInfoBase(input, bytes, tokenizerInfo)
    {
        /* <#Init Your Cursor Info#> */
    }

    ~___VARIABLE_productName___CursorInfo()
    {
        /* <#Deinit Your Cursor Info#> */
    }

    virtual int step(const char **ppToken,
                     int *pnBytes,
                     int *piStartOffset,
                     int *piEndOffset,
                     int *piPosition) override
    {
        ___VARIABLE_productName___TokenizerInfo *tokenizerInfo = (___VARIABLE_productName___TokenizerInfo *) m_tokenizerInfo;

        if (/* <#Is It Done Yet ?#> */) {

            /* <#Your Cursor Step#> */

            return SQLITE_OK;
        }
        return SQLITE_DONE;
    }

protected:
    /* <#Your Cursor Info#> */
};

#pragma mark - Module
class ___VARIABLE_productName___Module {
public:
    constexpr static const char Name[] = "___VARIABLE_productName___";

private:
    static const std::nullptr_t s_register;
};

constexpr const char ___VARIABLE_productName___Module::Name[];

const std::nullptr_t ___VARIABLE_productName___Module::s_register = []() {
    FTS::Module<___VARIABLE_productName___Module::Name, ___VARIABLE_productName___TokenizerInfo, ___VARIABLE_productName___CursorInfo>::Register();
    return nullptr;
}();

} //namespace FTS

} //namespace WCDB

NSString *const WCTTokenizerName___VARIABLE_productName___ = @(WCDB::FTS::___VARIABLE_productName___Module::Name);
