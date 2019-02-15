//___FILEHEADER___

#import "___VARIABLE_productName___Tokenizer.h"

#pragma mark - Tokenizer Info
___VARIABLE_productName___TokenizerInfo::___VARIABLE_productName___TokenizerInfo(int argc, const char *const *argv)
: WCDB::AbstractTokenizerInfo(argc, argv)
{
}

___VARIABLE_productName___TokenizerInfo::~___VARIABLE_productName___TokenizerInfo()
{
}

#pragma mark - Tokenizer Cursor Info
___VARIABLE_productName___TokenizerCursorInfo::___VARIABLE_productName___TokenizerCursorInfo(const char *input, int inputLength, WCDB::AbstractTokenizerInfo *tokenizerInfo)
: WCDB::AbstractTokenizerCursorInfo(input, inputLength, tokenizerInfo)
, m_tokenizerInfo(static_cast<___VARIABLE_productName___TokenizerInfo *>(tokenizerInfo))
{
}

___VARIABLE_productName___TokenizerCursorInfo::~___VARIABLE_productName___TokenizerCursorInfo()
{
}

int ___VARIABLE_productName___CursorInfo::step(const char **ppToken,
                                               int *pnBytes,
                                               int *piStartOffset,
                                               int *piEndOffset,
                                               int *piPosition)
{
    if (/* done */) {
        return WCTErrorCodeDone;
    }
    return WCTErrorCodeOK;
}
