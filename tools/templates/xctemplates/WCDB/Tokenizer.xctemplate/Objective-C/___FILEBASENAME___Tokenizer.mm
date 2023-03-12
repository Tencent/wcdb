//___FILEHEADER___

#import "___VARIABLE_productName___Tokenizer.h"

#pragma mark - Tokenizer Cursor Info
___VARIABLE_productName___Tokenizer::___VARIABLE_productName___Tokenizer(const char *const *azArg,
                                                                         int nArg,
                                                                         void *pCtx /* pCtx is only used in FTS5 */)
: WCDB::AbstractFTSTokenizer(azArg, nArg, pCtx)
{
}

___VARIABLE_productName___Tokenizer::~___VARIABLE_productName___Tokenizer()
{
}

void ___VARIABLE_productName___Tokenizer::loadInput(const char *pText,
                                                    int nText,
                                                    int flags) // flags is only used in FTS5
{
}

int ___VARIABLE_productName___Tokenizer::nextToken(const char **ppToken,
                                                   int *nToken,
                                                   int *iStart,
                                                   int *iEnd,
                                                   int *tflags,    //tflags is only used in FTS5
                                                   int *iPosition) //iPosition is only used in FTS3/4
{
    if (/* done */) {
        return WCTErrorCodeDone;
    }
    return WCTErrorCodeOK;
}
