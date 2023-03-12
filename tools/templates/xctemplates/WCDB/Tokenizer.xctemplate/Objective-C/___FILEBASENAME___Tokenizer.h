//___FILEHEADER___

#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

class ___VARIABLE_productName___Tokenizer final : public WCDB::AbstractFTSTokenizer {
public:
    ___VARIABLE_productName___Tokenizer(const char *const *azArg,
                                        int nArg,
                                        void *pCtx /* pCtx is only used in FTS5 */);
    ~___VARIABLE_productName___Tokenizer();

    void loadInput(const char *pText,
                   int nText,
                   int flags /* flags is only used in FTS5 */) override final;

    int nextToken(const char **ppToken,
                  int *nToken,
                  int *iStart,
                  int *iEnd,
                  int *tflags,   //tflags is only used in FTS5
                  int *iPosition //iPosition is only used in FTS3/4
                  ) override final;
};
