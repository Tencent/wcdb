//___FILEHEADER___

#import <Foundation/Foundation.h>
#import <WCDB/WCDB.h>

class ___VARIABLE_productName___TokenizerInfo final : public WCDB::AbstractTokenizerInfo {
public:
    ___VARIABLE_productName___TokenizerInfo(int argc, const char *const *argv);
    ~___VARIABLE_productName___TokenizerInfo();
};

class ___VARIABLE_productName___TokenizerCursorInfo final
: public WCDB::AbstractTokenizerCursorInfo {
public:
    ___VARIABLE_productName___TokenizerCursorInfo(const char *input,
                                                  int inputLength,
                                                  WCDB::AbstractTokenizerInfo *tokenizerInfo);
    ~___VARIABLE_productName___TokenizerCursorInfo();

    int step(const char **ppToken,
             int *pnBytes,
             int *piStartOffset,
             int *piEndOffset,
             int *piPosition) override final;

protected:
    ___VARIABLE_productName___TokenizerInfo *m_tokenizerInfo;
};
